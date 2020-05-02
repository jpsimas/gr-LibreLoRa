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
    receiverController::make(size_t SF, size_t symbolSize, correlationSync::sptr synchronizer, symbolDemod::sptr demodulator, grayEncode::sptr grayEncoder, deinterleave::sptr deinterleaver, decode::sptr decoder)
    {
      return gnuradio::get_initial_sptr
        (new receiverController_impl(SF, symbolSize, synchronizer, demodulator, grayEncoder, deinterleaver, decoder));
    }


    /*
     * The private constructor
     */
    receiverController_impl::receiverController_impl(size_t SF, size_t symbolSize, correlationSync::sptr synchronizer, symbolDemod::sptr demodulator, grayEncode::sptr grayEncoder, deinterleave::sptr deinterleaver, decode::sptr decoder)
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
	started(false),
	count(0),
	gotHeader(false) {
      setSFcurrent(SF-2);
      setCR(4);
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
      ninput_items_required[0] = SFcurrent;
      ninput_items_required[1] = 1;
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
      
      int produced = 0;

      // Do <+signal processing+>
      if(!started)
	if(*syncdIn)
	  startRx();
      
      if(started) {
	if(!gotHeader) {
	  gotHeader = true;
	  //consume(1, 1);
	  readHeader(nibblesIn, nibblesOut);
	  consume(0, SFcurrent);
	  produced = SFcurrent - 5;

	  if(!headerCheckSumValid || CR == 0 || CR > 4) {
	    stopRx();
	  } else {
	    setSFcurrent(SF);
	    payloadCount = 0;
	    std::cout << "Got Valid Header" << ": ";
	    std::cout << "length: " << unsigned(payloadLength) << ", ";	  
	    std::cout << "CR: " << unsigned(CR) << ", ";
	    std::cout << "CRC " << (payloadCRCPresent? "Present" : "Not Present") << std::endl;
	    if(payloadLength == 0)
	      stopRx();
	  }
	} else {
	  size_t i;
	  for(i = 0; i < noutput_items; i++) {
	    if(payloadCount + (SFcurrent - 7) > 2*payloadLength) {
	      stopRx();
	      break;
	    }
	    
	    nibblesOut[i] = nibblesIn[i];
	    payloadCount++;
	  }
	  produced = i;
	  consume(0, produced);
	}

	count++;
      }
     
      consume(1, ninput_items[1]);
      // Tell runtime system how many output items we produced.
      return produced;
    }

    void receiverController_impl::startRx() {
      count = 0;
      gotHeader = false;
      started = true;
    }

    void receiverController_impl::stopRx() {
      started = false;
      setSFcurrent(SF-2);
      setCR(4);
      synchronizer->reset();
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
      uint8_t headerCheckSumCalculated = calculateHeaderChecksum(*((uint16_t*) nibbles));

      std::cout << "checksum: " << std::hex << unsigned(headerCheckSum) << std::endl;
      std::cout << "calculated checksum: " << std::hex << unsigned(headerCheckSumCalculated) << std::endl;
      
      headerCheckSumValid = (headerCheckSum == headerCheckSumCalculated);
      for(size_t j = 0; j < SFcurrent - 5; j++)
	dataOut[j] = nibbles[5 + j];
    }
  } /* namespace LibreLoRa */
} /* namespace gr */

