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
#include "correlationSync_impl.h"

#include <gnuradio/block_detail.h>
#include <gnuradio/runtime_types.h>

#ifdef DEBUG
#include <iostream>
#endif

namespace gr {
  namespace LibreLoRa {

    correlationSync::sptr
    correlationSync::make(float corrMin, float corrStop, size_t symbolSize)
    {
      return gnuradio::get_initial_sptr
        (new correlationSync_impl(corrMin, corrStop, symbolSize));
    }


    /*
     * The private constructor
     */
    correlationSync_impl::correlationSync_impl(float corrMin, float corrStop, size_t symbolSize)
      : gr::block("correlationSync",
		  gr::io_signature::make(2, 2, sizeof(float)),
		  // gr::io_signature::make2(2, 2, symbolSize*sizeof(float), sizeof(bool))
		  gr::io_signature::make(1, 1, symbolSize*sizeof(float))
		  ),
	corrMin(corrMin),
	corrStop(corrStop),
	symbolSize(symbolSize),
	syncd(false),
	// currState(initial),
	fixedMode(true),
	nOutputItemsToProduce(0),
	deSyncAfterDone(false) {

      syncPort = pmt::string_to_symbol("sync");
      message_port_register_out(syncPort);
      
      message_port_register_in(pmt::mp("setNOutputItemsToProduce"));
      set_msg_handler(pmt::mp("setNOutputItemsToProduce"),
		      [this](pmt::pmt_t msg) {
#ifdef DEBUG
			std::cout << "correlationSync: setting n to " << int(pmt::to_long(msg)) << std::endl;
#endif
			setNOutputItemsToProduce(int(pmt::to_long(msg)));
		      });
      message_port_register_in(pmt::mp("reset"));
      set_msg_handler(pmt::mp("reset"),
		      [this](pmt::pmt_t msg) {
#ifdef DEBUG
			std::cout << "correlationSync: reset" << std::endl;
#endif
			reset();
		      });
    }

    /*
     * Our virtual destructor.
     */
    correlationSync_impl::~correlationSync_impl()
    {
    }

    void
    correlationSync_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      const size_t n = ((fixedModeEnabled() && (nOutputItemsToProduce < noutput_items))? nOutputItemsToProduce : noutput_items);
      ninput_items_required[0] = (syncd? symbolSize*n : /*(noutput_items + 1)*symbolSize*/2*symbolSize);
      
      ninput_items_required[1] = ninput_items_required[0];
    }

    int
    correlationSync_impl::general_work (int noutput_items,
    					gr_vector_int &ninput_items,
    					gr_vector_const_void_star &input_items,
    					gr_vector_void_star &output_items)
    {
      const float *data_in = (const float *) input_items[0];
      const float *corr = (const float *) input_items[1];
      float* data_out = (float*) output_items[0];
      // bool* syncd_out = (bool*) output_items[1];

#ifdef DEBUG
      std::cout << "correlationSync: work called: noutput_items = " << noutput_items << std::endl;
#endif
      
      // *syncd_out = false;
      // Do <+signal processing+>

      if(!syncd) {
    	bool foundFirstPt = false;
    	float corrMax = 0.0;
    	size_t maxPos = 0;

    	for(size_t i = 0; i < 2*symbolSize; i++) {
    	// for(size_t i = 0; i < ninput_items[0]/*(noutput_items + 1)*symbolSize*/; i++) {
    	  if(foundFirstPt) {
    	    if(corr[i] <= corrStop) {
    	      foundFirstPt = false;
    	      consume_each(maxPos);
    	      syncd = true;
#ifdef DEBUG
    	      std::cout << "correlationSync: sync'd" << std::endl;
#endif
    	      // *syncd_out = true;
    	      // produce(1, 1);
	      message_port_pub(syncPort, pmt::PMT_NIL);
#ifdef DEBUG
    	      std::cout << "correlationSync: produced syncd signal" << std::endl;
#endif
	      return WORK_CALLED_PRODUCE;
    	    } else if(corr[i] > corrMax) {
	      if(i < symbolSize) {
    	      // if(i < ninput_items[0] - symbolSize) {
    		corrMax = corr[i];
    		maxPos = i;
    	      } else
    		break;
	    }
	  } else if(corr[i] >= corrMin) {
	    if(i < symbolSize) {
	    // if(i < ninput_items[0] - symbolSize) {
	      foundFirstPt = true;
	      corrMax = corr[i];
	    } else
	      break;
	  }
	}//for

    	consume_each(symbolSize);
	// consume_each(ninput_items[0] - symbolSize);
	
    	return 0;
      } else {

    	size_t n = ((fixedModeEnabled() && (nOutputItemsToProduce < noutput_items))? nOutputItemsToProduce : noutput_items);
	
    	for(size_t j = 0; j < n; j++)
    	  for(size_t i = 0; i < symbolSize; i++)
    	    data_out[i + symbolSize*j] = data_in[i + symbolSize*j];

    	consume_each (symbolSize*n);
	
    	if(fixedModeEnabled())
    	  nOutputItemsToProduce -= n;
#ifdef DEBUG
    	std::cout << "produced " << n << " synced symbols" << std::endl;
#endif

    	if(nOutputItemsToProduce == 0 && deSyncAfterDone) {
    	  deSyncAfterDone = false;
    	  syncd = false;
    	}

	produce(0, n);
    	return WORK_CALLED_PRODUCE;
      }
    }
    
    // void
    // correlationSync_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    // { 
    // ninput_items_required[0] = noutput_items*symbolSize;
      
    // ninput_items_required[1] = ninput_items_required[0];
    // }
      
    // int
    // correlationSync_impl::general_work (int noutput_items,
    // 					gr_vector_int &ninput_items,
    // 					gr_vector_const_void_star &input_items,
    // 					gr_vector_void_star &output_items)
    // {
    //   const float *data_in = (const float *) input_items[0];
    //   const float *corr = (const float *) input_items[1];
    //   float* data_out = (float*) output_items[0];
    //   bool* syncd_out = (bool*) output_items[1];

    //   std::cout << "correlationSync: work called: noutput_items = " << noutput_items << std::endl;
      
    //   *syncd_out = false;
    //   // Do <+signal processing+>
    //   size_t maxPos = 0;
    //   size_t consumed = 0;
      
    //   for(size_t i = 0; i < noutput_items*symbolSize - consumed; i++) {
    // 	size_t m = i - consumed;
    // 	switch(currState) {
    // 	case initial:
    // 	  if(corr[m] >= corrMin) {
    // 	    currState = foundFirstPt;
    // 	    corrMax = corr[m];
    // 	  }
    // 	  break;
    // 	case foundFirstPt:
    // 	  if(corr[m] <= corrStop) {
    // 	    consume_each(maxPos);
    // 	    consumed += maxPos;
    // 	    currState = syncd;
    // 	    std::cout << "correlationSync: sync'd" << std::endl;
    // 	    *syncd_out = true;
    // 	    produce(1, 1);
    // 	    std::cout << "correlationSync: produced syncd signal" << std::endl;
    // 	  } else if(corr[m] > corrMax) {
    // 	    corrMax = corr[m];
    // 	    maxPos = i;
    // 	  }
	  
    // 	  break;
    // 	case syncd:
    // 	  size_t n = ((nOutputItemsToProduce < noutput_items)? nOutputItemsToProduce : noutput_items);
	
    // 	  for(size_t j = 0; j < n; j++)
    // 	    for(size_t k = 0; k < symbolSize; k++)
    // 	      data_out[k + symbolSize*j] = data_in[k + symbolSize*j];

    // 	  consume_each (symbolSize*n);
    // 	  consumed += symbolSize*n;
	
    // 	  nOutputItemsToProduce -= n;

    // 	  std::cout << "produced " << n << " synced symbols" << std::endl;
	
    // 	  produce(0, n);
	  
    // 	  if(nOutputItemsToProduce == 0)
    // 	    currState = initial;
    // 	  break;
    // 	}
    //   }

    //   if(currState == foundFirstPt)
    // 	consume_each(maxPos);
	
    //   return WORK_CALLED_PRODUCE;
    // }
    
    void correlationSync_impl::reset() {
      // currState = initial;
      syncd = false;
      // if(syncd) {
      // 	if(nOutputItemsToProduce == 0)
      // 	  syncd = false;
      // 	else
      // 	  deSyncAfterDone = true;
      // }
    }

  } /* namespace LibreLoRa */
} /* namespace gr */

