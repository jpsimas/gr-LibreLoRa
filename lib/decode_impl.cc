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
#include "decode_impl.h"

#include <LibreLoRa/getPairityMatrix.h>
#include <iostream>

namespace gr {
  namespace LibreLoRa {

    decode::sptr
    decode::make(size_t CR)
    {
      return gnuradio::get_initial_sptr
        (new decode_impl(CR));
    }


    /*
     * The private constructor
     */
    decode_impl::decode_impl(size_t CR)
      : gr::sync_block("decode",
		       gr::io_signature::make(1, 1, sizeof(uint8_t)),
		       gr::io_signature::make(1, 1, sizeof(uint8_t))) {
      setCR(CR);
      std::cout << "BENILOSCOPE v2 ACTIVATED" << std::endl;
      
      message_port_register_in(pmt::mp("setCR"));
      set_msg_handler(pmt::mp("setCR"), [this](pmt::pmt_t msg) {setCR(size_t(pmt::to_long(msg)));});
    }

    /*
     * Our virtual destructor.
     */
    decode_impl::~decode_impl()
    {
    }

    void
    decode_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
       /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
      //ninput_items_required[0] = noutput_items;
      sync_block::forecast(noutput_items, ninput_items_required);
      // std::cout << "decode: forecast called: nouput_items_required = " << ninput_items_required[0] << std::endl;
    }

    int
    decode_impl::work (int noutput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const uint8_t *in = (const uint8_t *) input_items[0];
      uint8_t *out = (uint8_t *) output_items[0];

      std::cout << "decode: work called: noutput_items = " << noutput_items << std::endl;
      
      // Do <+signal processing+>
      for(size_t i = 0; i < noutput_items; i++) {
	uint8_t syndrome = calculatePairity(in[i], pairityMatrix) ^ in[i];
	out[i] = (in[i] ^ cosetLeader[syndrome]) & 0x0f;
	std::cout << "decoded nibble: " <<  std::hex << unsigned(out[i]) << std::endl;
      }

      // Tell runtime system how many input items we consumed on
      // each input stream.
      // consume_each (noutput_items);

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

    void decode_impl::calculateCosetLeaders(){
      cosetLeader.clear();

      std::array<uint8_t, 4> checkMatrix;
      for(int i = 0; i < 4; i++)
	checkMatrix[i] = calculatePairity(1 << i, pairityMatrix);
      
      for(size_t i = 0; i + 1 < (CR + 4); i++)
	for(size_t j = 0; j < (CR + 4); j++) {
	  uint8_t syndrome = checkMatrix[i]^checkMatrix[j];
	  if(cosetLeader.find(syndrome) != cosetLeader.end())
	    cosetLeader[syndrome] = (1 << i)&(1 << j);
	}
    }

    void decode_impl::setCR(size_t CRnew) {
      CR = CRnew;
      pairityMatrix = getPairityMatrix(CR);
      
      calculateCosetLeaders();
    }
  } /* namespace LibreLoRa */
} /* namespace gr */

