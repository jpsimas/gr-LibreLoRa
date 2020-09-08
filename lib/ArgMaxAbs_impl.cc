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
#include "ArgMaxAbs_impl.h"
#include <volk/volk.h>

namespace gr {
  namespace LibreLoRa {

    ArgMaxAbs::sptr
    ArgMaxAbs::make(size_t vecLength)
    {
      return gnuradio::get_initial_sptr
        (new ArgMaxAbs_impl(vecLength));
    }


    /*
     * The private constructor
     */
    ArgMaxAbs_impl::ArgMaxAbs_impl(size_t vecLength)
      : gr::sync_block("ArgMaxAbs",
		       gr::io_signature::make(1, 1, vecLength*sizeof(gr_complex)),
		       gr::io_signature::make2(1, 2, sizeof(uint32_t), sizeof(gr_complex))),
	length(vecLength)
    {}

    /*
     * Our virtual destructor.
     */
    ArgMaxAbs_impl::~ArgMaxAbs_impl()
    {
    }

    int
    ArgMaxAbs_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      uint32_t *out = (uint32_t *) output_items[0];
      gr_complex *max_out;
      if(output_items.size() > 1)
	max_out = (gr_complex *) output_items[1];

      // Do <+signal processing+>
      for(size_t i = 0; i < noutput_items; i++){
	volk_32fc_index_max_32u(out + i, const_cast<gr_complex*>(in + i*length), length);
	if(output_items.size() > 1)
	  max_out[i] = in[i*length + out[i]];
      }

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace LibreLoRa */
} /* namespace gr */

