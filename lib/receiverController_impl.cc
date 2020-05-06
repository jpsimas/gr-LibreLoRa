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
    receiverController::make(size_t SF, correlationSync::sptr synchronizer/*, symbolDemod::sptr demodulator, grayEncode::sptr grayEncoder, deinterleave::sptr deinterleaver, decode::sptr decoder, randomize::sptr randomizer*/)
    {
      return gnuradio::get_initial_sptr
        (new receiverController_impl(SF, synchronizer/*, demodulator, grayEncoder, deinterleaver, decoder, randomizer*/));
    }


    /*
     * The private constructor
     */
    receiverController_impl::receiverController_impl(size_t SF, correlationSync::sptr synchronizer/*, symbolDemod::sptr demodulator, grayEncode::sptr grayEncoder, deinterleave::sptr deinterleaver, decode::sptr decoder, randomize::sptr randomizer*/)
      : gr::block("receiverController",
		  gr::io_signature::make2(2, 2, sizeof(uint8_t), sizeof(bool)),
		  gr::io_signature::make(1, 1, sizeof(uint8_t))),
	SF(SF),
	synchronizer(synchronizer),
	// symbolSize(symbolSize),
	// demodulator(demodulator),
	// grayEncoder(grayEncoder),
	// deinterleaver(deinterleaver),
	// decoder(decoder),
	// randomizer(randomizer),
	currentState(waitingForSync) {

      //prevent demodulotator producing symbols before start
      synchronizer->enableFixedMode();
      synchronizer->setNOutputItemsToProduce(0);
      //randomizer->reset();

      lfsrStatePort = pmt::string_to_symbol("lfsrStateOut");
      setSFPort = pmt::string_to_symbol("setSFout");
      setCRPort = pmt::string_to_symbol("setCRout");
      
      message_port_register_out(lfsrStatePort);
      message_port_register_out(setSFPort);
      message_port_register_out(setCRPort);

      //setSFcurrent(SF-2);
      
      std::cout << "Turbo Encabulator Started" << std::endl;
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
      case readingHeader:
	ninput_items_required[0] = SFcurrent;
	ninput_items_required[1] = 0;
	break;
      case sendingPayload:
	ninput_items_required[0] = payloadNibblesToRead + (payloadCRCPresent? 0 : extraNibblesToConsume);
	ninput_items_required[1] = 0;
	break;
      case sendingCRC:
	ninput_items_required[0] = 2 + extraNibblesToConsume;
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

      case readingHeader:
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
	    payloadNibblesToRead = 2*payloadLength - (SFcurrent - 5);
	    size_t nibblesToRead = payloadNibblesToRead + 2*(payloadCRCPresent? payloadCRCSize : 0);
	    extraNibblesToConsume = SF*ceil(nibblesToRead/float(SF)) - nibblesToRead;
	    std::cout << "nibbles to read: " << payloadNibblesToRead << ", SF = " << SF << std::endl;
	    setSFcurrent(SF);

	    currentState = sendingPayload;
	    synchronizer->setNOutputItemsToProduce((extraNibblesToConsume + nibblesToRead)*(CR + 4)/SF);
	    // randomizer->reset();
	  }
	}
	break;
      case sendingPayload:

	message_port_pub(lfsrStatePort, pmt::from_long(0xff));
	
	for(size_t i = 0; i < payloadNibblesToRead; i++) {
	  nibblesOut[i] = nibblesIn[i];
	  std::cout << "receiverController: produced data nibble:" << std::hex << unsigned(nibblesOut[i]) << std::endl;
	}
	
	produced = payloadNibblesToRead;

	if(payloadCRCPresent) {
	  consume(0, payloadNibblesToRead);
	  currentState = sendingCRC;
	} else {
	  consume(0, payloadNibblesToRead + extraNibblesToConsume);
	  stopRx();
	}

	break;
      case sendingCRC:
	message_port_pub(lfsrStatePort, pmt::from_long(0x00));
	
	for(size_t i = 0; i < 2*payloadCRCSize; i++)
	  nibblesOut[i] = nibblesIn[i];

	produced = 2*payloadCRCSize;
	consume(0, 2*payloadCRCSize + extraNibblesToConsume);
	stopRx();
	break;
      default:
	;
      }
      
      // Tell runtime system how many output items we produced.
      return produced;
    }

    void receiverController_impl::startRx() {
      setSFcurrent(SF-2);
      setCR(4);
      currentState = readingHeader;
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
       // demodulator->setSF(SFcurrent);
       // grayEncoder->setSF(SFcurrent);
       // deinterleaver->setSF(SFcurrent);
      message_port_pub(setSFPort, pmt::from_long(SFcurrent));
    }

    void receiverController_impl::setCR(size_t CRnew) {
      CR = CRnew;
      // deinterleaver->setCR(CR);
      // decoder->setCR(CR);
      message_port_pub(setCRPort, pmt::from_long(CR));
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

