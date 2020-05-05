/* -*- c++ -*- */
/*
 * Copyright 2020 Joao Pedro de O Simas.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "receiverController_impl.h"

#include <iostream>
#include <LibreLoRa/calculateHeaderChecksum.h>

namespace gr {
  namespace LibreLoRa {

    receiverController::sptr
    receiverController::make(size_t SF, size_t symbolSize, correlationSync::sptr synchronizer, symbolDemod::sptr demodulator, grayEncode::sptr grayEncoder, deinterleave::sptr deinterleaver, decode::sptr decoder, randomize::sptr randomizer)
    {
      return gnuradio::get_initial_sptr
        (new receiverController_impl(SF, symbolSize, synchronizer, demodulator, grayEncoder, deinterleaver, decoder, randomizer));
    }


    /*
     * The private constructor
     */
    receiverController_impl::receiverController_impl(size_t SF, size_t symbolSize, correlationSync::sptr synchronizer, symbolDemod::sptr demodulator, grayEncode::sptr grayEncoder, deinterleave::sptr deinterleaver, decode::sptr decoder, randomize::sptr randomizer)
      : gr::block("receiverController",
		  gr::io_signature::make2(2, 2, sizeof(uint8_t), sizeof(bool)),
		  gr::io_signature::make(1, 1, sizeof(uint8_t))),
	SF(SF),
	synchronizer(synchronizer),
	symbolSize(symbolSize),
	demodulator(demodulator),
	grayEncoder(grayEncoder),
	deinterleaver(deinterleaver),
	decoder(decoder),
	randomizer(randomizer),
	currentState(waitingForSync) {
      setSFcurrent(SF-2);

      //prevent demodulotator producing symbols before start
      synchronizer->enableFixedMode();
      synchronizer->setNOutputItemsToProduce(0);
      //randomizer->reset();

      message_port_register_out(pmt::string_to_symbol("startRx"));
    }

    /*
     * Our virtual destructor.
     */
    receiverController_impl::~receiverController_impl()
    {
    }

    void
    receiverController_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      // ninput_items_required[0] = started? (gotHeader? nibblesToRead : SFcurrent) : 0;
      // ninput_items_required[1] = started? 0 : 1;
      switch(currentState) {
      case waitingForSync:
	ninput_items_required[0] = 0;
	ninput_items_required[1] = 1;
	break;
      case decodingHeader:
	ninput_items_required[0] = SFcurrent;
	ninput_items_required[1] = 0;
	break;
      case decodingPayload:
	ninput_items_required[0] = nibblesToRead;
	ninput_items_required[1] = 0;
	break;
      default:
	ninput_items_required[0] = 0;
	ninput_items_required[1] = 0;
      }

      // std::cout << "receiverController: forecast called: nouput_items_required[0] = " << ninput_items_required[0] << std::endl << " nouput_items_required[1] = " << ninput_items_required[1] << std::endl;
      // std::cout << " noutput_items = "<< noutput_items << std::endl;
    }
    
    int
    receiverController_impl::general_work(int noutput_items,
					  gr_vector_int &ninput_items,
					  gr_vector_const_void_star &input_items,
					  gr_vector_void_star &output_items)
    {
      const uint8_t *nibblesIn = (const uint8_t *) input_items[0];
      const bool *syncdIn = (const bool *) input_items[1];
      uint8_t *nibblesOut = (uint8_t *) output_items[0];

      std::cout << "receiverController: work called: noutput_items = " << noutput_items << std::endl;
      
      int produced = 0;

      // Do <+signal processing+>
      switch(currentState) {
      case waitingForSync:
	if(*syncdIn) {
	  startRx();
	}
	consume(1, 1);
	break;

      case decodingHeader:
	//consume(1, 1);
	readHeader(nibblesIn, nibblesOut);
	consume(0, SFcurrent);
	produced = SFcurrent - 5;

	if(!headerCheckSumValid || CR == 0 || CR > 4) {
	  stopRx();
	} else {
	  std::cout << std::dec;
	  std::cout << "Got Valid Header" << ": ";
	  std::cout << "length: " << unsigned(payloadLength) << ", ";	  
	  std::cout << "CR: " << unsigned(CR) << ", ";
	  std::cout << "CRC " << (payloadCRCPresent? "Present" : "Not Present") << std::endl;
	    
	  if(2*payloadLength <= (SFcurrent - 5))
	    stopRx();
	  else {
	    nibblesToRead = 2*(payloadLength + (payloadCRCPresent? 1 : 0)) - (SFcurrent - 5);
	    nibblesToConsume = SF*ceil(nibblesToRead/float(SF));
	    std::cout << "nibbles to read: " << nibblesToRead << ", SF = " << SF << std::endl;
	    setSFcurrent(SF);
	    currentState = decodingPayload;
	    synchronizer->setNOutputItemsToProduce(nibblesToConsume*(CR + 4)/SF);
	    // randomizer->reset();
	  }
	}
	break;
      case decodingPayload:

	// fill it with random bytes

	// send the vector
	message_port_pub(pmt::string_to_symbol("startRx"), pmt::PMT_NIL);
	
	for(size_t i = 0; i < nibblesToRead; i++) {
	  nibblesOut[i] = nibblesIn[i];
	  std::cout << "receiverController: produced data nibble:" << std::hex << unsigned(nibblesOut[i]) << std::endl;
	}
	  
	produced = nibblesToRead;
	consume(0, nibblesToConsume);
	stopRx();	 

	break;
      default:
	;
      }
      
      // Tell runtime system how many output items we produced.
      return produced;
    }

    void receiverController_impl::startRx() {
      setCR(4);
      currentState = decodingHeader;
      synchronizer->setNOutputItemsToProduce(8);
      std::cout << "starting RX" << std::endl;
    }

    void receiverController_impl::stopRx() {
      currentState = waitingForSync;
      setSFcurrent(SF-2);
      synchronizer->reset();
      // demodulator->disable();
      std::cout << "stopping RX" << std::endl;
    }

    void receiverController_impl::setSFcurrent(size_t SFnew) {
      SFcurrent = SFnew;
      demodulator->setSF(SFcurrent);
      grayEncoder->setSF(SFcurrent);
      deinterleaver->setSF(SFcurrent);
    }

    void receiverController_impl::setCR(size_t CRnew) {
      CR = CRnew;
      deinterleaver->setCR(CR);
      decoder->setCR(CR);
    }
    
    void receiverController_impl::readHeader(const uint8_t* nibbles, uint8_t* dataOut) {
      std::cout << "reading header" << std::endl;
      std::cout << "nibbles: ";
      for(size_t i = 0; i < 5; i++)
	std::cout << std::hex << unsigned(nibbles[i]) << " ";
      std::cout << std::endl;
      
      payloadLength = (nibbles[0] << 4)|(nibbles[1]);
      std::cout << "length: " << unsigned(payloadLength) << std::endl;
      payloadCRCPresent = nibbles[2] & 0x01;
      std::cout << "Checksum present: " << (payloadCRCPresent? "yes" : "no") << std::endl;
      CR = nibbles[2] >> 1;
      std::cout << "CR: " << unsigned(CR) << std::endl;
      uint8_t headerCheckSum = (nibbles[3] << 4)|nibbles[4];
      uint8_t headerCheckSumCalculated = calculateHeaderChecksum((*(uint32_t*) nibbles));

      std::cout << "checksum: " << std::hex << unsigned(headerCheckSum) << std::endl;
      std::cout << "calculated checksum: " << std::hex << unsigned(headerCheckSumCalculated) << std::endl;
      
      headerCheckSumValid = (headerCheckSum == headerCheckSumCalculated);
      for(size_t j = 0; j < SFcurrent - 5; j++)
	dataOut[j] = nibbles[5 + j];
    }
  } /* namespace LibreLoRa */
} /* namespace gr */

