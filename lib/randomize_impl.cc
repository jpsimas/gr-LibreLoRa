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
#include "randomize_impl.h"

#include <LibreLoRa/utilities.h>

#include <iostream>

namespace gr {
  namespace LibreLoRa {

    randomize::sptr
    randomize::make( )
    {
      return gnuradio::get_initial_sptr
        (new randomize_impl());
    }


    /*
     * The private constructor
     */
    randomize_impl::randomize_impl( )
      : gr::sync_block("randomize",
              gr::io_signature::make(1, 1, sizeof(uint8_t)),
		       gr::io_signature::make(1, 1, sizeof(uint8_t))),
	lfsrState(0xff) {

      message_port_register_in(pmt::mp("reset"));
      set_msg_handler(pmt::mp("reset"), [this](pmt::pmt_t msg) {reset();});
    }

    /*
     * Our virtual destructor.
     */
    randomize_impl::~randomize_impl()
    {
    }

    int
    randomize_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const uint8_t *in = (const uint8_t *) input_items[0];
      uint8_t *out = (uint8_t *) output_items[0];

      std::vector<tag_t> tags;
      get_tags_in_range(tags, 0, nitems_read(0), nitems_read(0) + noutput_items);
      
      for(size_t i = 0; i < noutput_items; i++) {
	out[i] = in[i] ^ lfsrState;
	std::cout << "randomize: in = " << std::hex << unsigned(in[i]) << ", out = " << unsigned(out[i]) << ", state = " << unsigned(lfsrState) << std::endl;
	lfsrState = (lfsrState << 1) | pairity(lfsrState&0xB8);
      }
      
      return noutput_items;
    }

    void
    randomize_impl::reset() {
      lfsrState = 0xff;
    }
  } /* namespace LibreLoRa */
} /* namespace gr */

