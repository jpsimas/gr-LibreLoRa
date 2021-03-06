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
#include "maxFrequency_impl.h"

namespace gr {
  namespace LibreLoRa {

    maxFrequency::sptr
    maxFrequency::make(size_t symbolSize)
    {
      return gnuradio::make_block_sptr<maxFrequency_impl>(
        symbolSize);
    }


    /*
     * The private constructor
     */
    maxFrequency_impl::maxFrequency_impl(size_t symbolSize)
      : gr::sync_block("maxFrequency",
		  gr::io_signature::make(1, 1, sizeof(gr_complex)),
		       gr::io_signature::make(1, 1, sizeof(float))),
	// fftVect(symbolSize),
	plan(symbolSize),
	// planTemp(plan.temp_size),
	symbolSize(symbolSize)
    {
      set_history(symbolSize);
    }

    /*
     * Our virtual destructor.
     */
    maxFrequency_impl::~maxFrequency_impl()
    {
    }

    void
    maxFrequency_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
      ninput_items_required[0] = noutput_items + symbolSize;
    }
    
    int
    maxFrequency_impl::work (int noutput_items,
				     gr_vector_const_void_star &input_items,
				     gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      float *out = (float *) output_items[0];
      
      // Do <+signal processing+>
      for(size_t i = 0; i < noutput_items; i++) {
	// plan.execute(fftVect, kfr::make_univector(const_cast<std::complex<float>*>(in + i), symbolSize), planTemp, false);
	out[i] = 0;
       
      }
      // Tell runtime system how many input items we consumed on
      // each input stream.
      consume_each (noutput_items);

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace LibreLoRa */
} /* namespace gr */

