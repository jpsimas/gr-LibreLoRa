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
#include "convolution_impl.h"

#include <volk/volk.h>
#include <iostream>

namespace gr {
  namespace LibreLoRa {

    convolution::sptr
    convolution::make(const std::vector<gr_complex>& window)
    {
      return gnuradio::get_initial_sptr
        (new convolution_impl(window));
    }


    /*
     * The private constructor
     */
    convolution_impl::convolution_impl(const std::vector<gr_complex>& window)
      : gr::sync_block("convolution",
		       gr::io_signature::make(1, 1, window.size()*sizeof(gr_complex)),
		       gr::io_signature::make(1, 1, window.size()*sizeof(gr_complex))),
	windowSize(window.size()),
	twoWindows(window)
    {
      twoWindows.insert(twoWindows.end(), twoWindows.begin(), twoWindows.end());
      std::cout << "Flushing Tremmie Pipes. expected Directance: " << 2*windowSize << ". Got: " << twoWindows.size() << std::endl;
    }

    /*
     * Our virtual destructor.
     */
    convolution_impl::~convolution_impl()
    {
    }

    int
    convolution_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      gr_complex *out = (gr_complex *) output_items[0];

      // Do <+signal processing+>
      for(size_t i = 0; i < noutput_items; i++)
	for(size_t j = 0; j < windowSize; j++)
	  volk_32fc_x2_dot_prod_32fc(out + i*windowSize + j, in + i*windowSize + j, twoWindows.data() + j, windowSize);

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace LibreLoRa */
} /* namespace gr */

