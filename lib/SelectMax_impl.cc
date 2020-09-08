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
#include "SelectMax_impl.h"

#include <volk/volk.h>

namespace gr {
  namespace LibreLoRa {

    SelectMax::sptr
    SelectMax::make(size_t nInputs)
    {
      return gnuradio::get_initial_sptr
        (new SelectMax_impl(nInputs));
    }


    /*
     * The private constructor
     */
    SelectMax_impl::SelectMax_impl(size_t nInputs)
      : gr::sync_block("SelectMax",
		       gr::io_signature::make(2, 2, nInputs*sizeof(float)),
		       gr::io_signature::make(1, 1, sizeof(float))),
	nInputs(nInputs)
    {}

    /*
     * Our virtual destructor.
     */
    SelectMax_impl::~SelectMax_impl()
    {
    }

    int
    SelectMax_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const float *in = (const float *) input_items[0];
      const float *inPwr = (const float *) input_items[1];
      float *out = (float *) output_items[0];
      
      for(auto i = 0; i < noutput_items; i++) {
	uint32_t maxInd;
	volk_32f_index_max_32u(&maxInd, inPwr + i*nInputs, nInputs);
	out[i] = in[i*nInputs + maxInd];
      }

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace LibreLoRa */
} /* namespace gr */

