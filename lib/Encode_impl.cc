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
#include "Encode_impl.h"

#include <LibreLoRa/getPairityMatrix.h>

namespace gr {
  namespace LibreLoRa {

    Encode::sptr
    Encode::make(size_t CR)
    {
      return gnuradio::get_initial_sptr
        (new Encode_impl(CR));
    }


    /*
     * The private constructor
     */
    Encode_impl::Encode_impl(size_t CR)
      : gr::sync_block("Encode",
		       gr::io_signature::make(1, 1, sizeof(uint8_t)),
		       gr::io_signature::make(1, 1, sizeof(uint8_t)))
    {
      setCR(CR);
    }

    /*
     * Our virtual destructor.
     */
    Encode_impl::~Encode_impl()
    {
    }

    int
    Encode_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const uint8_t *in = (const uint8_t *) input_items[0];
      uint8_t *out = (uint8_t *) output_items[0];

      for(size_t i = 0; i < noutput_items; i++) {
	//out[i] = calculatePairity(in[i], pairityMatrix);
      }


      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

    void Encode_impl::setCR(size_t CRnew) {
      CR = CRnew;
      pairityMatrix = getPairityMatrix(CR);
    }
    
  } /* namespace LibreLoRa */
} /* namespace gr */

