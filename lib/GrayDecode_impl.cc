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
#include "GrayDecode_impl.h"

namespace gr {
  namespace LibreLoRa {

    GrayDecode::sptr
    GrayDecode::make(size_t nBits)
    {
      return gnuradio::get_initial_sptr
        (new GrayDecode_impl(nBits));
    }


    /*
     * The private constructor
     */
    GrayDecode_impl::GrayDecode_impl(size_t nBits)
      : gr::sync_block("GrayDecode",
		       gr::io_signature::make(1, 1, sizeof(uint16_t)),
		       gr::io_signature::make(1, 1, sizeof(uint16_t))),
	nBits(nBits)
    {}

    /*
     * Our virtual destructor.
     */
    GrayDecode_impl::~GrayDecode_impl()
    {
    }

    int
    GrayDecode_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const uint16_t *in = (const uint16_t *) input_items[0];
      uint16_t *out = (uint16_t *) output_items[0];

      // Do <+signal processing+>
      for(size_t i = 0; i < noutput_items; i++)
	// out[i] = grayDecode(in[i]);;

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace LibreLoRa */
} /* namespace gr */

