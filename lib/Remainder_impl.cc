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
#include "Remainder_impl.h"

namespace gr {
  namespace LibreLoRa {

    Remainder::sptr
    Remainder::make(float divisor)
    {
      return gnuradio::make_block_sptr<Remainder_impl>(
        divisor);
    }


    /*
     * The private constructor
     */
    Remainder_impl::Remainder_impl(float divisor)
      : gr::sync_block("Remainder",
		       gr::io_signature::make(1, 1, sizeof(float)),
		       gr::io_signature::make(1, 1, sizeof(float))),
	divisor(divisor)
    {}

    /*
     * Our virtual destructor.
     */
    Remainder_impl::~Remainder_impl()
    {
    }

    int
    Remainder_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const float *in = (const float *) input_items[0];
      float *out = (float *) output_items[0];

      for(auto i = 0; i < noutput_items; i++)
	out[i] = std::remainder(in[i], divisor);

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace LibreLoRa */
} /* namespace gr */

