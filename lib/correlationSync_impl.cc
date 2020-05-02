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
	syncd(false) {
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
      ninput_items_required[0] = 2*symbolSize;
      ninput_items_required[1] = 2*symbolSize;
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
	      *syncd_out = true;
	      syncd = true;
	      std::cout << "correlationSync: sync'd" << std::endl;
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
      }

      if(syncd) {
	for(size_t i = 0; i < symbolSize; i++)
	  data_out[i] = data_in[i];

	consume_each (symbolSize);
	return 1;
      } else {
	consume_each (symbolSize);
	return 0;
      }
    }
    
    void correlationSync_impl::reset() {
      syncd = false;
    }

  } /* namespace LibreLoRa */
} /* namespace gr */

