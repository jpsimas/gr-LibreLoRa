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

#include <LibreLoRa/calculateHeaderChecksum.h>
#include <LibreLoRa/utilities.h>

#define DEBUG

#ifdef DEBUG
#include <iostream>
#endif

namespace gr {
  namespace LibreLoRa {

    receiverController::sptr
    receiverController::make(size_t SF, bool lowDataRate/*, correlationSync::sptr synchronizer/*, symbolDemod::sptr demodulator, grayEncode::sptr grayEncoder, deinterleave::sptr deinterleaver, decode::sptr decoder, randomize::sptr randomizer*/)
    {
      return gnuradio::get_initial_sptr
        (new receiverController_impl(SF, lowDataRate/*, synchronizer/*, demodulator, grayEncoder, deinterleaver, decoder, randomizer*/));
    }


    /*
     * The private constructor
     */
    receiverController_impl::receiverController_impl(size_t SF, bool lowDataRate/*, correlationSync::sptr synchronizer/*, symbolDemod::sptr demodulator, grayEncode::sptr grayEncoder, deinterleave::sptr deinterleaver, decode::sptr decoder, randomize::sptr randomizer*/)
      : gr::block("receiverController",
		  // gr::io_signature::make2(2, 2, sizeof(uint8_t), sizeof(bool)),
		  gr::io_signature::make(1, 1, sizeof(uint8_t)),
		  gr::io_signature::make(1, 1, sizeof(uint8_t))),
	SF(SF),
	SFcurrent(SF),
	lowDataRate(lowDataRate),
	// synchronizer(synchronizer),
	// symbolSize(symbolSize),
	// demodulator(demodulator),
	// grayEncoder(grayEncoder),
	// deinterleaver(deinterleaver),
	// decoder(decoder),
	// randomizer(randomizer),
	payloadNibblesToRead(0),
	extraNibblesToConsume(0),
	currentState(waitingForSync) {

      //prevent demodulotator producing symbols before start
      // synchronizer->enableFixedMode();
      // synchronizer->setNOutputItemsToProduce(0);

      
      //randomizer->reset();
      

      lfsrStatePort = pmt::string_to_symbol("lfsrStateOut");
      setSFPort = pmt::string_to_symbol("setSFout");
      setCRPort = pmt::string_to_symbol("setCRout");
      synchronizerSetNPort = pmt::string_to_symbol("synchronizerSetN");
      synchronizerResetPort = pmt::string_to_symbol("synchronizerReset");
      payloadLengthPort = pmt::string_to_symbol("payloadLength");
      crcPort = pmt::string_to_symbol("crc");
      
      message_port_register_out(lfsrStatePort);
      message_port_register_out(setSFPort);
      message_port_register_out(setCRPort);
      message_port_register_out(synchronizerSetNPort);
      message_port_register_out(synchronizerResetPort);
      message_port_register_out(payloadLengthPort);
      message_port_register_out(crcPort);

      message_port_register_in(pmt::mp("sync"));
      set_msg_handler(pmt::mp("sync"), [this](pmt::pmt_t msg) {startRx();});
      
      //setSFcurrent(SF-2);

#ifdef DEBUG
      std::cout << "Turbo Encabulator Started. Low Data Rate? " << (lowDataRate? "YES" : "NO") << std::endl;
#endif
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
	//no samples are really needed, but this avoinds work being called when it is not necessary
	ninput_items_required[0] = SFcurrent;//0;
	// ninput_items_required[1] = 1;
	break;
      case readingHeader:
	ninput_items_required[0] = SFcurrent;
	// ninput_items_required[1] = 0;
	break;
      case sendingPayload:
	ninput_items_required[0] = payloadNibblesToRead + extraNibblesToConsume + (payloadCRCPresent? 2*payloadCRCSize : 0);
	// ninput_items_required[1] = 0;
	break;
      }
    }
    
    int
    receiverController_impl::general_work(int noutput_items,
					  gr_vector_int &ninput_items,
					  gr_vector_const_void_star &input_items,
					  gr_vector_void_star &output_items)
    {
      const uint8_t *nibblesIn = (const uint8_t *) input_items[0];
      // const bool *syncdIn = (const bool *) input_items[1];
      uint8_t *nibblesOut = (uint8_t *) output_items[0];

#ifdef DEBUG
      std::cout << "receiverController: work called: noutput_items = " << noutput_items << std::endl;
#endif
      
      int produced = 0;
      size_t payloadNibblesToProduce = 0;
      // Do <+signal processing+>
      switch(currentState) {
      case waitingForSync:
	// if(*syncdIn)
	//   startRx();
	
	// consume(1, 1);
	break;

      case readingHeader:
	//consume(1, 1);
	readHeader(nibblesIn, nibblesOut);
	//consume(0, SFcurrent);
	//produced = SFcurrent - 5;
	consume(0, 5);
	produced = 0;

	if(!headerCheckSumValid || CR == 0 || CR > 4) {
	  stopRx();
	} else {
#ifdef DEBUG
	  std::cout << std::dec;
	  std::cout << "Got Valid Header" << ": ";
	  std::cout << "length: " << unsigned(payloadLength) << ", ";	  
	  std::cout << "CR: " << unsigned(CR) << ", ";
	  std::cout << "CRC " << (payloadCRCPresent? "Present" : "Not Present") << std::endl;
#endif

	  message_port_pub(lfsrStatePort, pmt::from_long(0xff));
	  message_port_pub(payloadLengthPort, pmt::from_long(payloadLength));

	  // if(2*payloadLength <= (SFcurrent - 5))
	  //   stopRx();
	  // else {
	  payloadNibblesToRead = 2*payloadLength;// - (SFcurrent - 5);
	  size_t nibblesToRead = payloadNibblesToRead + 2*(payloadCRCPresent? payloadCRCSize : 0);
	    // extraNibblesToConsume = SF*ceil(nibblesToRead/float(SF)) - nibblesToRead;
	    //extraNibblesToConsume = (SF - nibblesToRead%SF)%SF;

	  setSFcurrent(lowDataRate? (SF - 2) : SF);
	    //setSFcurrent(SF);
	  if(nibblesToRead > (SF - 7))
	     extraNibblesToConsume = (SFcurrent - (nibblesToRead - (SF - 7))%SFcurrent)%SFcurrent;

#ifdef DEBUG
	    std::cout << "nibbles to read: " << payloadNibblesToRead << ", SF = " << SF << std::endl;
	    std::cout << "extra nibbles: " << extraNibblesToConsume << ", SF = " << SF << std::endl;
#endif

	    currentState = sendingPayload;
	    // synchronizer->setNOutputItemsToProduce((extraNibblesToConsume + nibblesToRead)*(CR + 4)/SF);
	    if(nibblesToRead + extraNibblesToConsume > (SF - 7))	    
	      message_port_pub(synchronizerSetNPort, pmt::from_long((extraNibblesToConsume + nibblesToRead - (SF - 7))*(CR + 4)/SF));
	    // message_port_pub(synchronizerResetPort, pmt::PMT_NIL);
	    // randomizer->reset();
	  // }
	}
	break;
      case sendingPayload:

	//message_port_pub(lfsrStatePort, pmt::from_long(0xff));
	for(size_t i = 0; i < payloadNibblesToRead; i++) {
	  nibblesOut[i] = nibblesIn[i];

#ifdef DEBUG
	  std::cout << "receiverController: produced data nibble:" << std::hex << unsigned(nibblesOut[i]) << std::endl;
#endif
	  
	}
	
	produced = payloadNibblesToRead;

	if(payloadCRCPresent) {
	  uint16_t CRC = nibbles2bytes<uint16_t>(*((uint32_t*)(nibblesIn + payloadNibblesToRead)));

#ifdef DEBUG
	  std::cout << "receiverController: read CRC: " << unsigned(CRC) << std::endl;
#endif
	  
	  message_port_pub(crcPort, pmt::from_long(CRC));

	}

#ifdef DEBUG
	for(size_t i = 0 ; i < extraNibblesToConsume; i++)
	  std::cout << "receiverController: extra nibble:" << std::hex << unsigned(nibblesIn[payloadNibblesToRead + (payloadCRCPresent? 2*payloadCRCSize : 0) + i]) << std::endl;
#endif
	
	consume(0, payloadNibblesToRead + extraNibblesToConsume + (payloadCRCPresent? 2*payloadCRCSize : 0));
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
      //synchronizer->setNOutputItemsToProduce(8);
      message_port_pub(synchronizerSetNPort, pmt::from_long(8));

#ifdef DEBUG
      std::cout << "starting RX" << std::endl;
#endif
      
    }

    void receiverController_impl::stopRx() {
      currentState = waitingForSync;
      setSFcurrent(SF-2);
      //synchronizer->reset();
      message_port_pub(synchronizerResetPort, pmt::PMT_NIL);
      // demodulator->disable();

#ifdef DEBUG
      std::cout << "stopping RX" << std::endl;
#endif
      
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
#ifdef DEBUG
      std::cout << "reading header" << std::endl;
      std::cout << "nibbles: ";
      
      for(size_t i = 0; i < 5; i++)
	std::cout << std::hex << unsigned(nibbles[i]) << " ";
      std::cout << std::endl;
#endif
      
      payloadLength = (nibbles[0] << 4)|(nibbles[1]);

#ifdef DEBUG
      std::cout << "length: " << unsigned(payloadLength) << std::endl;
#endif
      
      payloadCRCPresent = nibbles[2] & 0x01;

#ifdef DEBUG
      std::cout << "Checksum present: " << (payloadCRCPresent? "yes" : "no") << std::endl;
#endif
      
      setCR(nibbles[2] >> 1);

#ifdef DEBUG
      std::cout << "CR: " << unsigned(CR) << std::endl;
#endif
      
      uint8_t headerCheckSum = (nibbles[3] << 4)|nibbles[4];
      uint8_t headerCheckSumCalculated = calculateHeaderChecksum((*(uint32_t*) nibbles));

#ifdef DEBUG
      std::cout << "checksum: " << std::hex << unsigned(headerCheckSum) << std::endl;
      std::cout << "calculated checksum: " << std::hex << unsigned(headerCheckSumCalculated) << std::endl;
#endif
      
      headerCheckSumValid = (headerCheckSum == headerCheckSumCalculated);
      // for(size_t j = 0; j < SFcurrent - 5; j++)
      // 	dataOut[j] = nibbles[5 + j];
    }
  } /* namespace LibreLoRa */
} /* namespace gr */

