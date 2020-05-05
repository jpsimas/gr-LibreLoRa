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
		  gr::io_signature::make2(2, 2, symbolSize*sizeof(float), sizeof(bool))),
	corrMin(corrMin),
	corrStop(corrStop),
	symbolSize(symbolSize),
	syncd(false),
	fixedMode(false),
	nOutputItemsToProduce(0) {
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
      /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
      const size_t n = ((fixedMode && (nOutputItemsToProduce < noutput_items))? nOutputItemsToProduce : noutput_items);
      ninput_items_required[0] = (syncd? symbolSize*n : 2*symbolSize);
      
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
      bool* syncd_out = (bool*) output_items[1];

      std::cout << "correlationSync: work called: noutput_items = " << noutput_items << std::endl;
      
      *syncd_out = false;
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
	      std::cout << "correlationSync: sync'd" << std::endl;
	      *syncd_out = true;
	      produce(1, 1);
	      std::cout << "correlationSync: produced syncd signal" << std::endl;
	      return WORK_CALLED_PRODUCE;
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

	size_t n = ((fixedMode && (nOutputItemsToProduce < noutput_items))? nOutputItemsToProduce : noutput_items);
	
	for(size_t j = 0; j < n; j++)
	  for(size_t i = 0; i < symbolSize; i++)
	    data_out[i + symbolSize*j] = data_in[i + symbolSize*j];

	consume_each (symbolSize*n);
	
	if(fixedMode)
	  nOutputItemsToProduce -= n;

	std::cout << "produced " << n << " synced symbols" << std::endl;
	
	return n;
      }
    }
    
    void correlationSync_impl::reset() {
      syncd = false;
    }

  } /* namespace LibreLoRa */
} /* namespace gr */

