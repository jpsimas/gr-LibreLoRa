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
#include "CRC16_impl.h"

#include <LibreLoRa/utilities.h>

#ifndef NDEBUG
#include <iostream>
#endif

namespace gr {
  namespace LibreLoRa {

    CRC16::sptr
    CRC16::make(uint16_t polynomial, uint16_t xorOut, size_t payloadSize)
    {
      return gnuradio::make_block_sptr<CRC16_impl>(
        polynomial, xorOut, payloadSize);
    }


    /*
     * The private constructor
     */
    CRC16_impl::CRC16_impl(uint16_t polynomial, uint16_t xorOut, size_t payloadSize)
      : gr::block("CRC16",
		  gr::io_signature::make(1, 1, sizeof(uint8_t)),
		  // gr::io_signature::make(0, 1, sizeof(uint16_t))),
		  gr::io_signature::make(0, 0, 0)),
	polynomial(polynomial),
	xorOut(xorOut),
	payloadSize(payloadSize),
	crc(0),
	byteCount(0) {
      message_port_register_in(pmt::mp("setPayloadSize"));
      set_msg_handler(pmt::mp("setPayloadSize"), [this](pmt::pmt_t msg) {setPayloadSize(size_t(pmt::to_long(msg)));});
      
      message_port_register_in(pmt::mp("setXorOut"));
      set_msg_handler(pmt::mp("setXorOut"), [this](pmt::pmt_t msg) {setXorOut(uint16_t(pmt::to_long(msg)));});

      crcOutPort = pmt::string_to_symbol("crcOut");
      message_port_register_out(crcOutPort);
#ifndef NDEBUG
      std::cout << "CRC16: constructed." << std::endl;
#endif
    }

    /*
     * Our virtual destructor.
     */
    CRC16_impl::~CRC16_impl()
    {
    }

    void
    CRC16_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
      // ninput_items_required[0] = (payloadSize == 0)? noutput_items : noutput_items*payloadSize;
      // ninput_items_required[0] = noutput_items*payloadSize;
      ninput_items_required[0] = noutput_items;
    }

    int
    CRC16_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const uint8_t *in = (const uint8_t *) input_items[0];
      // uint16_t *out;
      // if(output_items.size() > 0)
      // 	out = (uint16_t *) output_items[0];

// #ifndef NDEBUG
//       std::cout << std::dec << "CRC16: work called, noutput_items = " << noutput_items << ", payloadSize = " << payloadSize << std::endl;
// #endif

      //check for payloadSize tags
      std::vector<gr::tag_t> tags;
      get_tags_in_range(tags, 0, nitems_read(0), nitems_read(0) + 1, pmt::intern("payloadSize"));
      if(tags.size() != 0) {
	size_t payloadSizeNew = pmt::to_long(tags[0].value);
	if(payloadSizeNew <= 255) {
	  setPayloadSize(payloadSizeNew);
	} else {
	  setPayloadSize(1);
#ifndef NDEBUG
	  std::cout << "CRC16: got invalid payloadSize." << payloadSize << std::endl;
#endif
	}
      }
      
      if(payloadSize == 0){
	consume_each(noutput_items);
	return 0;
      }
      
      for(size_t j = 0; j < noutput_items; j++) {
	// const uint8_t* inJ = in + j*payloadSize;

	// uint32_t crc = 0x0000;
	// for(size_t i = 0; i < payloadSize; i++) {
	crc <<= 8;
	  // crc = polDivRem(crc^uint16_t(inJ[i]), polynomial);
	crc = polDivRem(crc^uint16_t(in[j]), polynomial);
	// }
	byteCount++;
	// if(output_items.size() > 0)
	//   out[j] = crc^xorOut;

	if(byteCount == payloadSize) {
	  message_port_pub(crcOutPort, pmt::from_long(crc^xorOut));

#ifndef NDEBUG
	  std::cout << std::hex << "CRC16: calculated CRC: " << /*unsigned(out[j])*/unsigned(crc^xorOut) << std::endl;
#endif
	  
	  byteCount = 0;
	  crc = 0x0000;
	}
      }
      // Tell runtime system how many input items we consumed on
      // each input stream.
      // consume_each (payloadSize*noutput_items);
      consume_each(noutput_items);

      // payloadSize = 0;
      // Tell runtime system how many output items we produced.
      return noutput_items;
    }
    
    void CRC16_impl::setXorOut(uint16_t newXorOut) {
#ifndef NDEBUG
      std::cout << std::hex << "CRC16: set xorOut to:" << newXorOut << std::endl;
#endif
      xorOut = newXorOut;
    }
    
    void CRC16_impl::setPayloadSize(size_t newSize) {
#ifndef NDEBUG
      std::cout << "CRC16: set payloadSize to:" << newSize << std::endl;
#endif
      payloadSize = newSize;
    }
  } /* namespace LibreLoRa */
} /* namespace gr */

