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
#include "frequencyTracker_impl.h"
#include <cmath>
#include <complex>
#include <volk/volk.h>

#ifndef NDEBUG
#include <iostream>
#endif

namespace gr {
  namespace LibreLoRa {

    frequencyTracker::sptr
    frequencyTracker::make(float mu, size_t SF, size_t OSF)
    {
      return gnuradio::get_initial_sptr
        (new frequencyTracker_impl(mu, SF, OSF));
    }


    /*
     * The private constructor
     */
    frequencyTracker_impl::frequencyTracker_impl(float mu, size_t SF, size_t OSF)
      : gr::block("frequencyTracker",
		  gr::io_signature::make(1, 1, sizeof(gr_complex)),
		  gr::io_signature::make(1, 1, sizeof(float))),
	mu(mu),
	wStep(std::polar<float>(1, -2*M_PI*1.0/((1 << SF)*OSF*OSF))),
	w(1.0) {
#ifndef NDEBUG
      std::cout << ":DDD" << std::endl;
#endif
    }

    /*
     * Our virtual destructor.
     */
    frequencyTracker_impl::~frequencyTracker_impl()
    {
    }

    void
    frequencyTracker_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      ninput_items_required[0] = noutput_items + 1;
    }

    int
    frequencyTracker_impl::general_work (int noutput_items,
					 gr_vector_int &ninput_items,
					 gr_vector_const_void_star &input_items,
					 gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      float *out = (float *) output_items[0];

      // Do <+signal processing+>

      for(int i = 0; i < noutput_items; i++) {
	w *= wStep;
	
	w = (1 - mu)*w + mu*std::conj(in[i + 1]/(in[i] + 1e-6f));
	
	out[i] = -std::arg(w)/(2*M_PI);
      }

      
      // Tell runtime system how many input items we consumed on
      // each input stream.
      consume_each (noutput_items);

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace LibreLoRa */
} /* namespace gr */

