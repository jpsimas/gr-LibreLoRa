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
#include "grayEncode_impl.h"

#ifndef NDEBUG
#include <iostream>
#endif

namespace gr {
  namespace LibreLoRa {

    grayEncode::sptr
    grayEncode::make(size_t SF)
    {
      return gnuradio::make_block_sptr<grayEncode_impl>(
        SF);
    }


    /*
     * The private constructor
     */
    grayEncode_impl::grayEncode_impl(size_t SF)
      : SF(SF),
	gr::sync_block("grayEncode",
		       gr::io_signature::make(1, 1, sizeof(uint16_t)),
		       gr::io_signature::make(1, 1, sizeof(uint16_t)))
    {
      message_port_register_in(pmt::mp("setSF"));
      set_msg_handler(pmt::mp("setSF"), [this](pmt::pmt_t msg) {setSF(size_t(pmt::to_long(msg)));});
    }

    /*
     * Our virtual destructor.
     */
    grayEncode_impl::~grayEncode_impl()
    {
    }

    // void
    // grayEncode_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    // {
    //   ninput_items_required[0] = noutput_items;
    // }

    int
    grayEncode_impl::work (int noutput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const uint16_t *in = (const uint16_t *) input_items[0];
      uint16_t *out = (uint16_t *) output_items[0];

#ifndef NDEBUG
      std::cout << "grayEncode: work called: noutput_items = " << noutput_items << std::endl;
#endif
      
      // Do <+signal processing+>
      for(size_t i = 0; i < noutput_items; i++)
	out[i] = (in[i] ^ (in[i] >> 1)) & ((1 << SF) - 1);

      // Tell runtime system how many input items we consumed on
      // each input stream.
	//consume_each (noutput_items);

      // Tell runtime system how many output items we produced.
	return noutput_items;
    }

    void grayEncode_impl::setSF(size_t SFnew) {
      SF = SFnew;
    }
  } /* namespace LibreLoRa */
} /* namespace gr */

