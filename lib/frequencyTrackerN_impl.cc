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
#include "frequencyTrackerN_impl.h"
#include <cmath>
#include <complex>
#include <volk/volk.h>

#ifndef NDEBUG
#include <iostream>
#endif

namespace gr {
  namespace LibreLoRa {

    frequencyTrackerN::sptr
    frequencyTrackerN::make(float mu, size_t SF, size_t OSF, const std::vector<gr_complex>& window)
    {
      return gnuradio::get_initial_sptr
        (new frequencyTrackerN_impl(mu, SF, OSF, window));
    }


    /*
     * The private constructor
     */
    frequencyTrackerN_impl::frequencyTrackerN_impl(float mu, size_t SF, size_t OSF, const std::vector<gr_complex>& window)
      : gr::sync_block("frequencyTrackerN",
		  gr::io_signature::make(1, 1, sizeof(gr_complex)),
		  gr::io_signature::make(1, 1, sizeof(float))),
	window(window),
	windowedSig(window.size()),
	mu(mu),
	OSF(OSF),
	w(1.0){
      set_history(window.size());
#ifndef NDEBUG
      std::cout << "frequencyTrackerN: constructed. window size:" << window.size() << std::endl;
      
#endif
    }

    /*
     * Our virtual destructor.
     */
    frequencyTrackerN_impl::~frequencyTrackerN_impl()
    {
    }

    int
    frequencyTrackerN_impl::work (int noutput_items,
					 gr_vector_const_void_star &input_items,
					 gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      float *out = (float *) output_items[0];

      // Do <+signal processing+>

      for(int i = 0; i < noutput_items; i++) {

	for(auto j = 0; j < window.size(); j++)
	  windowedSig[j] = in[i + j]*window[j];

	gr_complex prod;
	volk_32fc_x2_conjugate_dot_prod_32fc(&prod, windowedSig.data() + OSF, windowedSig.data(), window.size() - OSF);
	w = (1 - mu)*w + mu*prod/(std::abs(prod) + 1e-6f);
	
	out[i] = std::arg(w)/(2*M_PI*OSF);
      }

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace LibreLoRa */
} /* namespace gr */

