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
#include "BytesToNibbles_impl.h"

namespace gr {
  namespace LibreLoRa {

    BytesToNibbles::sptr
    BytesToNibbles::make()
    {
      return gnuradio::make_block_sptr<BytesToNibbles_impl>(
        );
    }


    /*
     * The private constructor
     */
    BytesToNibbles_impl::BytesToNibbles_impl()
      : gr::sync_interpolator("BytesToNibbles",
              gr::io_signature::make(1, 1, sizeof(uint8_t)),
              gr::io_signature::make(1, 1, sizeof(uint8_t)), 2)
    {}

    /*
     * Our virtual destructor.
     */
    BytesToNibbles_impl::~BytesToNibbles_impl()
    {
    }

    int
    BytesToNibbles_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const uint8_t *in = (const uint8_t *) input_items[0];
      uint8_t *out = (uint8_t *) output_items[0];

      for(size_t i = 0; i < noutput_items/2; i++){
	out[2*i] = in[i]&0x0f;
	out[2*i + 1] = (in[i] >> 4)&0x0f;

#ifndef NDEBUG
	std::cout << "BytesToNibbles: in = " << unsigned(in[i]) << ", out = " << unsigned(out[2*i]) << ", " << unsigned(out[2*i + 1]) <<  std::endl;
#endif
      }

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace LibreLoRa */
} /* namespace gr */

