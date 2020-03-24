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
		  gr::io_signature::make2(2, 2, sizeof(int), sizeof(float))),
	corrMin(corrMin),
	corrStop(corrStop),
	maxDelay(maxDelay),
	foundFirstPt(false),
	delay(0),
	delayCounter(0) {
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
      ninput_items_required[0] = noutput_items + maxDelay;
      ninput_items_required[1] = noutput_items + maxDelay;
    }

    int
    correlationSync_impl::general_work (int noutput_items,
					gr_vector_int &ninput_items,
					gr_vector_const_void_star &input_items,
					gr_vector_void_star &output_items)
    {
      const float *data_in = (const float *) input_items[0];
      const float *corr = (const float *) input_items[1];
      int *synched = (int *) output_items[0];
      float *data_out = (float *) output_items[1];

      // Do <+signal processing+>
      for(size_t i = 0; i < noutput_items; i++) {
	synched[i] = 0;
	
	if(foundFirstPt) {
	  if(corr[i] <= corrStop) {
	    foundFirstPt = false;
	    synched[i] = 1;
	    foundFirstPt = 0;
	    delay = delayCounter;
	  } else if(delayCounter > maxDelay) {
	      foundFirstPt = false;
	      delayCounter = 0;
	  } else 
	    delayCounter++;
	} else if(corr[i] >= corrMin)
	  foundFirstPt = 1;

	data_out[i] = data_in[maxDelay - delay + i];
      }

      
      // Tell runtime system how many input items we consumed on
      // each input stream.
      consume_each (noutput_items);

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace LibreLoRa */
} /* namespace gr */

