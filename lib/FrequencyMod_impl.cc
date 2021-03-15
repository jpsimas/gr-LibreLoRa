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
#include "FrequencyMod_impl.h"

namespace gr {
  namespace LibreLoRa {

    FrequencyMod::sptr
    FrequencyMod::make()
    {
      return gnuradio::make_block_sptr<FrequencyMod_impl>(
        );
    }


    /*
     * The private constructor
     */
    FrequencyMod_impl::FrequencyMod_impl()
      : gr::sync_block("FrequencyMod",
		       gr::io_signature::make(1, 1, sizeof(float)),
		       gr::io_signature::make(1, 1, sizeof(gr_complex))),
	w(1.0f)
    {}

    /*
     * Our virtual destructor.
     */
    FrequencyMod_impl::~FrequencyMod_impl()
    {
    }

    int
    FrequencyMod_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const float *freqIn = (const float *) input_items[0];
      gr_complex *out = (gr_complex *) output_items[0];

      for(auto i = 0; i < noutput_items; i++){
	w *= std::polar<float>(1.0f, 2*M_PI*freqIn[i]);
	out[i] = w;
      }
	

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace LibreLoRa */
} /* namespace gr */

