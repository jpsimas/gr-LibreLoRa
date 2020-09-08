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
#include "Interleave_impl.h"

namespace gr {
  namespace LibreLoRa {

    Interleave::sptr
    Interleave::make(size_t SF, size_t CR)
    {
      return gnuradio::get_initial_sptr
        (new Interleave_impl(SF, CR));
    }


    /*
     * The private constructor
     */
    Interleave_impl::Interleave_impl(size_t SF, size_t CR)
      : codeLength(CR + 4),
	SF(SF),
	gr::block("Interleave",
		  gr::io_signature::make(1, 1, sizeof(uint8_t)),
		  gr::io_signature::make(1, 1, sizeof(uint16_t)))
    {
#ifndef NDEBUG
      std::cout << "Interleave: constructed" << std::endl;
#endif
      
      message_port_register_in(pmt::mp("setSF"));
      set_msg_handler(pmt::mp("setSF"), [this](pmt::pmt_t msg) {setSF(size_t(pmt::to_long(msg)));});

      message_port_register_in(pmt::mp("setCR"));
      set_msg_handler(pmt::mp("setCR"), [this](pmt::pmt_t msg) {setCR(size_t(pmt::to_long(msg)));});
    }

    /*
     * Our virtual destructor.
     */
    Interleave_impl::~Interleave_impl()
    {
    }

    void
    Interleave_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      ninput_items_required[0] = SF*((noutput_items + codeLength - 1)/codeLength);
    }

    int
    Interleave_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const uint8_t *in = (const uint8_t *) input_items[0];
      uint16_t *out = (uint16_t *) output_items[0];

      const size_t blocksToProduce = (noutput_items + codeLength - 1)/codeLength;

      #ifndef NDEBUG
      if(blocksToProduce != 0) {
	std::cout << "Interleave: producing: " << blocksToProduce << " blocks (" << "nouput_items = " << noutput_items << ", SF = " << SF << ")" << std::endl;
      }
#endif
      
      for(size_t k = 0; k < codeLength; k++)
	out[k] = 0;

      for(size_t i = 0; i < blocksToProduce; i++) {

#ifndef NDEBUG
	std::cout << "Interleave: interleaving symbols: ";
	for(size_t j = 0; j < SF; j++)
	  std::cout << std::hex << in[i*SF + j] << " ";
	std::cout << std::endl;
#endif
      
	for(size_t j = 0; j < SF; j++)
	  for(size_t k = 0; k < codeLength; k++)
	    out[i*codeLength + k] |= (in[i*SF + (j + k)%SF] >> k & 0x01) << j;
      }
    

      // Tell runtime system how many input items we consumed on
      // each input stream.
      // consume_each (noutput_items);
      consume_each(SF*blocksToProduce);

      // Tell runtime system how many output items we produced.
      // return noutput_items;
      return codeLength*blocksToProduce;
    }

    void Interleave_impl::setSF(size_t SFNew) {
      SF = SFNew;
    }
    
    void Interleave_impl::setCR(size_t CR) {
      codeLength = CR + 4;
    }
    
  } /* namespace LibreLoRa */
} /* namespace gr */

