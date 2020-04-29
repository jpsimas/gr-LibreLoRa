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
#include "correlationSyncDemod_impl.h"

namespace gr {
  namespace LibreLoRa {

    correlationSyncDemod::sptr
    correlationSyncDemod::make(float corrMin, float corrStop, size_t symbolSize)
    {
      return gnuradio::get_initial_sptr
        (new correlationSyncDemod_impl(corrMin, corrStop, symbolSize));
    }


    /*
     * The private constructor
     */
    correlationSyncDemod_impl::correlationSyncDemod_impl(float corrMin, float corrStop, size_t symbolSize)
      : gr::sync_decimator("correlationSync",
			   gr::io_signature::make(2, 2, sizeof(float)),
			   gr::io_signature::make2(2, 2, sizeof(float), sizeof(bool)),
			   symbolSize),
	corrMin(corrMin),
	corrStop(corrStop),
	symbolSize(symbolSize),
	foundFirstPt(false),
	delay(0),
	delayCounter(0) {
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
      /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
      ninput_items_required[0] = noutput_items*symbolSize + symbolSize;
      ninput_items_required[1] = noutput_items*symbolSize + symbolSize;
    }
    
    int
    correlationSyncDemod_impl::work(int noutput_items,
				    gr_vector_const_void_star &input_items,
				    gr_vector_void_star &output_items)
    {
      const float *data_in = (const float *) input_items[0];
      const float *corr = (const float *) input_items[1];
      float* data_out = (float*) output_items[0];
      bool* syncd_out = (bool*) output_items[1];

      for(size_t i = 0; i < noutput_items; i++)
	syncd_out[i] = false;
      // Do <+signal processing+>
      for(size_t i = 0; i < noutput_items; i++) {
	for(size_t j = 0; j < symbolSize; j++) {
	  size_t k = i*symbolSize + j;
	  // syncd_out[i] = false;
	  if(foundFirstPt) {
	    if(corr[k] <= corrStop) {
	      foundFirstPt = false;
	      delay = delayCounter;
	      syncd_out[i] = true;
	    } else if(delayCounter > symbolSize) {
      	      foundFirstPt = false;
      	      delayCounter = 0;
	    } else 
	      delayCounter++;
	  } else if(corr[k] >= corrMin)
	    foundFirstPt = true;

	  //data_out[k] = data_in[symbolSize - delay + k];
	}
	data_out[i] = data_in[symbolSize - delay + (symbolSize/2 - 1) + i*symbolSize];
	if(syncd_out[i])
	  deltaF = ....
      }
      
      // Tell runtime system how many input items we consumed on
      // each input stream.
      consume_each (noutput_items);

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace LibreLoRa */
} /* namespace gr */

