/* -*- c++ -*- */
/*
 * Copyright 2020 Joao Pedro de O. Simas.
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
#include "correlationSyncDemod_impl.h"

#include <gnuradio/block_detail.h>
#include <gnuradio/runtime_types.h>

#ifdef DEBUG
#include <iostream>
#endif

namespace gr {
  namespace LibreLoRa {

    correlationSyncDemod::sptr
    correlationSyncDemod::make(float corrMin, float corrStop, size_t symbolSize, size_t preambleSize, size_t SF, uint16_t syncWordNumber)
    {
      return gnuradio::get_initial_sptr
        (new correlationSyncDemod_impl(corrMin, corrStop, symbolSize, preambleSize, SF, syncWordNumber));
    }


    /*
     * The private constructor
     */
    correlationSyncDemod_impl::correlationSyncDemod_impl(float corrMin, float corrStop, size_t symbolSize, size_t preambleSize, size_t SF, uint16_t syncWordNumber)
      : gr::block("correlationSyncDemod",
		  gr::io_signature::make(2, 2, sizeof(float)),
		  // gr::io_signature::make2(2, 2, symbolSize*sizeof(float), sizeof(bool))
		  gr::io_signature::make(1, 1, sizeof(uint16_t))
		  ),
	corrMin(corrMin),
	corrStop(corrStop),
	symbolSize(symbolSize),
	preambleSize(preambleSize),
	SF(SF - 2),
	OSF(symbolSize >> SF),
	syncWordNum1((syncWordNumber >> 4) << (SF - 4)),
	syncWordNum2((syncWordNumber & 0xf) << (SF - 4)),
	syncd(false),
	// currState(initial),
	fixedMode(true),
	nOutputItemsToProduce(0),
	deSyncAfterDone(false),
	preambleConsumed(false) {

#ifdef DEBUG
      std::cout << "correlationSyncDemod: Constructed" << std::endl;
#endif
      
      syncPort = pmt::string_to_symbol("sync");
      message_port_register_out(syncPort);
      
      message_port_register_in(pmt::mp("setNOutputItemsToProduce"));
      set_msg_handler(pmt::mp("setNOutputItemsToProduce"),
		      [this](pmt::pmt_t msg) {
#ifdef DEBUG
			std::cout << "correlationSyncDemod: setting n to " << int(pmt::to_long(msg)) << std::endl;
#endif
			setNOutputItemsToProduce(int(pmt::to_long(msg)));
		      });
      message_port_register_in(pmt::mp("reset"));
      set_msg_handler(pmt::mp("reset"),
		      [this](pmt::pmt_t msg) {
#ifdef DEBUG
			std::cout << "correlationSyncDemod: reset" << std::endl;
#endif
			reset();
		      });

      message_port_register_in(pmt::mp("setSF"));
      set_msg_handler(pmt::mp("setSF"), [this](pmt::pmt_t msg) {setSF(size_t(pmt::to_long(msg)));});
    }

    /*
     * Our virtual destructor.
     */
    correlationSyncDemod_impl::~correlationSyncDemod_impl()
    {
    }

    void
    correlationSyncDemod_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      const size_t n = ((fixedModeEnabled() && (nOutputItemsToProduce < noutput_items))? nOutputItemsToProduce : noutput_items);
      ninput_items_required[0] = (syncd? (preambleConsumed? (symbolSize*n) : preambleSize) : 2*symbolSize);
      
      ninput_items_required[1] = ninput_items_required[0];
    }

    int
    correlationSyncDemod_impl::general_work (int noutput_items,
    					gr_vector_int &ninput_items,
    					gr_vector_const_void_star &input_items,
    					gr_vector_void_star &output_items)
    {
      const float *data_in = (const float *) input_items[0];
      const float *corr = (const float *) input_items[1];
      uint16_t* data_out = (uint16_t*) output_items[0];

#ifdef DEBUG
#endif
      
      // Do <+signal processing+>

      if(!syncd) {
    	bool foundFirstPt = false;
    	float corrMax = 0.0;
    	size_t maxPos = 0;

    	for(size_t i = 0; i < 2*symbolSize; i++) {
    	  if(foundFirstPt) {
    	    if(corr[i] <= corrStop) {
    	      foundFirstPt = false;
    	      consume_each(maxPos);
    	      syncd = true;
	      preambleConsumed = false;
#ifdef DEBUG
    	      std::cout << "correlationSyncDemod: sync'd" << std::endl;
#endif
	      
	      message_port_pub(syncPort, pmt::PMT_NIL);
#ifdef DEBUG
    	      std::cout << "correlationSyncDemod: produced syncd signal" << std::endl;
#endif
	      return 0;
    	    } else if(corr[i] > corrMax) {
	      if(i < symbolSize) {
    		corrMax = corr[i];
    		maxPos = i;
    	      } else
    		break;
	    }
	  } else if(corr[i] >= corrMin) {
	    if(i < symbolSize) {
	      foundFirstPt = true;
	      corrMax = corr[i];
	    } else
	      break;
	  }
	}

    	consume_each(symbolSize);
	
    	return 0;
      } else {
	if(!preambleConsumed) {
	  size_t minPreSize = 4*symbolSize + symbolSize/4 - (symbolSize >> (SF + 2));


	  consume_each(preambleSize - minPreSize);

#ifdef DEBUG
	  std::cout << "correlationSyncDemod: preambleSize - minPreSize = " << std::dec << preambleSize - minPreSize << std::endl;
	  std::cout << "correlationSyncDemod: minPreSize = " << std::dec << minPreSize << std::endl;
#endif
	    
	  
	  float deltaF1 = (data_in[symbolSize/2] - syncWordNum1/float(symbolSize));
	  float deltaF2 = (data_in[symbolSize/2 + symbolSize] - syncWordNum2/float(symbolSize));
	  
	  deltaF = (deltaF1 + deltaF2)/2.0;

#ifdef DEBUG
	  std::cout << "correlationSyncDemod: calculated frequency offset 1: " << deltaF1 << std::endl;
	  std::cout << "correlationSyncDemod: calculated frequency offset 2: " << deltaF2 << std::endl;
	  std::cout << "correlationSyncDemod: calculated mean frequency offset: " << deltaF << std::endl;
#endif
	  
	  consume_each(minPreSize);
	  preambleConsumed = true;
	  return 0;
	}
	
	size_t n = ((fixedModeEnabled() && (nOutputItemsToProduce < noutput_items))? nOutputItemsToProduce : noutput_items);
	
	for(size_t j = 0; j < n; j++) {
	  data_out[j] = (uint16_t(std::round((data_in[symbolSize*j + symbolSize/2]- deltaF)*(OSF << SF))))%uint16_t(1 << SF);
	  
#ifdef DEBUG
	  std::cout << "correlationSyncDemod: demodulated symbol, SF = " << std::dec << SF << ": " << data_out[j] << std::endl;
#endif
	}
	  
	consume_each (symbolSize*n);
	
	if(fixedModeEnabled())
	  nOutputItemsToProduce -= n;
#ifdef DEBUG
	if(n != 0)
	  std::cout << "correlationSyncDemod: produced " << n << " synced symbols" << std::endl;
#endif

	if(nOutputItemsToProduce == 0 && deSyncAfterDone) {
	  deSyncAfterDone = false;
	  syncd = false;
	  preambleConsumed = false;
	  SF = SFAfterDone;
	}
	return n;
      }
    }
    
    void correlationSyncDemod_impl::reset() {
      if(nOutputItemsToProduce == 0)      
	syncd = false;
      else {
	deSyncAfterDone = true;
      }
    }

    void correlationSyncDemod_impl::setSF(size_t SFNew) {
      if(nOutputItemsToProduce == 0 && !deSyncAfterDone)
	SF = SFNew;
      else
	SFAfterDone = SFNew;
    }
  } /* namespace LibreLoRa */
} /* namespace gr */

