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
    correlationSync::make(float corrMin, float corrStop, size_t maxDelay)
    {
      return gnuradio::get_initial_sptr
        (new correlationSync_impl(corrMin, corrStop, maxDelay));
    }


    /*
     * The private constructor
     */
    correlationSync_impl::correlationSync_impl(float corrMin, float corrStop, size_t maxDelay)
      : gr::block("correlationSync",
		  gr::io_signature::make(2, 2, sizeof(float)),
		  gr::io_signature::make2(2, 2, maxDelay*sizeof(float), sizeof(bool))),
	corrMin(corrMin),
	corrStop(corrStop),
	maxDelay(maxDelay),
	foundFirstPt(false),
	delay(0),
	delayCounter(0),
	corrMax(0) {
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
      ninput_items_required[0] = 2*maxDelay;
      ninput_items_required[1] = 2*maxDelay;
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
      for(size_t i = 0; i < maxDelay; i++) {
      	if(foundFirstPt) {
      	  if(corr[i] <= corrStop) {
      	    foundFirstPt = false;
      	    delay = delayCounter;
	    consume_each(i - delay);
	    *syncd_out = true;
      	  } else if(delayCounter > maxDelay)
      	      foundFirstPt = false;
	else {
	    if(corr[i] > corrMax) {
	      corrMax = corr[i];
	      delayCounter = 0;
	    } else
	      delayCounter++;
	  }
      	} else if(corr[i] >= corrMin) {
      	  foundFirstPt = true;
	  corrMax = corr[i];
	  delayCounter = 0;
	}
      }

      for(size_t i = 0; i < maxDelay; i++)
	data_out[i] = data_in[i];
      
      // Tell runtime system how many input items we consumed on
      // each input stream.
      consume_each (maxDelay);

      // Tell runtime system how many output items we produced.
      return 1;
    }

  } /* namespace LibreLoRa */
} /* namespace gr */

