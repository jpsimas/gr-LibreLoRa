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
#include "NibblesToBytes_impl.h"

namespace gr {
  namespace LibreLoRa {

    NibblesToBytes::sptr
    NibblesToBytes::make( )
    {
      return gnuradio::make_block_sptr<NibblesToBytes_impl>(
        );
    }


    /*
     * The private constructor
     */
    NibblesToBytes_impl::NibblesToBytes_impl( )
      : gr::sync_decimator("NibblesToBytes",
              gr::io_signature::make(1, 1, sizeof(uint8_t)),
              gr::io_signature::make(1, 1, sizeof(uint8_t)), 2)
    {}

    /*
     * Our virtual destructor.
     */
    NibblesToBytes_impl::~NibblesToBytes_impl()
    {
    }

    int
    NibblesToBytes_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const uint8_t *in = (const uint8_t *) input_items[0];
      uint8_t *out = (uint8_t *) output_items[0];

      // Do <+signal processing+>
      for(size_t i = 0; i < noutput_items; i++)
	out[i] = (in[2*i + 1] << 4)|in[2*i];
      
      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace LibreLoRa */
} /* namespace gr */

