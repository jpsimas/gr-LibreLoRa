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
      return gnuradio::make_block_sptr<Encode_impl>(
        CR);
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

      message_port_register_in(pmt::mp("setCR"));
      set_msg_handler(pmt::mp("setCR"), [this](pmt::pmt_t msg) {setCR(size_t(pmt::to_long(msg)));});
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

#ifndef NDEBUG
      std::cout << "Encode: encoding: " << std::dec << noutput_items << " items" << ", CR = " << CR << std::endl;
#endif

      static const pmt::pmt_t tagKey = pmt::intern("loraParams");
      
      for(size_t i = 0; i < noutput_items; i++) {

	std::vector<gr::tag_t> tags;
	auto nr =  nitems_read(0);
	get_tags_in_range(tags, 0, nr + i, nr + i + 1, tagKey);
	if(tags.size() != 0) {
	  
	  pmt::pmt_t message = tags[0].value;
	  size_t CRnew = pmt::to_long(pmt::tuple_ref(message, 1));
	  setCR(CRnew);
#ifndef NDEBUG
	  std::cout << "Encode: " << "changed CR to: " << CR << " (tag)." << std::endl;
#endif
	}
	
	out[i] = calculatePairity(in[i], pairityMatrix);

#ifndef NDEBUG
	std::cout << std::hex << "Encode: in: " << unsigned(in[i]) << ", out: "<<  std::hex << unsigned(out[i]) << ", CR = " << CR << std::endl;
#endif
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

