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

#ifdef DEBUG
#include <iostream>
#endif

namespace gr {
  namespace LibreLoRa {

    CRC16::sptr
    CRC16::make(uint16_t polynomial, uint16_t xorOut)
    {
      return gnuradio::get_initial_sptr
        (new CRC16_impl(polynomial, xorOut));
    }


    /*
     * The private constructor
     */
    CRC16_impl::CRC16_impl(uint16_t polynomial, uint16_t xorOut)
      : gr::block("CRC16",
              gr::io_signature::make(1, 1, sizeof(uint8_t)),
		  gr::io_signature::make(1, 1, sizeof(uint16_t))),
	polynomial(polynomial),
	xorOut(xorOut),
	payloadSize(2) {
      message_port_register_in(pmt::mp("setPayloadSize"));
      set_msg_handler(pmt::mp("setPayloadSize"), [this](pmt::pmt_t msg) {setPayloadSize(size_t(pmt::to_long(msg)));});
      
      message_port_register_in(pmt::mp("setXorOut"));
      set_msg_handler(pmt::mp("setXorOut"), [this](pmt::pmt_t msg) {setXorOut(uint16_t(pmt::to_long(msg)));});
#ifdef DEBUG
      std::cout << "gyro-controlled sine-wave director calibration started." << std::endl;
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
      ninput_items_required[0] = noutput_items*payloadSize;
    }

    int
    CRC16_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const uint8_t *in = (const uint8_t *) input_items[0];
      uint16_t *out = (uint16_t *) output_items[0];

#ifdef DEBUG
      std::cout << std::dec << "CRC16: work called, noutput_items = " << noutput_items << ", payloadSize = " << payloadSize << std::endl;
#endif
      
      for(size_t j = 0; j < noutput_items; j++) {
	const uint8_t* inJ = in + j*payloadSize;

	uint32_t crc = 0x0000;
	for(size_t i = 0; i < payloadSize; i++) {
	  crc <<= 8;
	  crc = polDivRem(crc^uint16_t(inJ[i]), polynomial);
	}
	
	out[j] = crc^xorOut;

#ifdef DEBUG
	std::cout << std::hex << "CRC16: calculated CRC: " << unsigned(out[j]) << std::endl;
#endif
      }
      // Tell runtime system how many input items we consumed on
      // each input stream.
      consume_each (noutput_items*payloadSize);

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }
    
    void CRC16_impl::setXorOut(uint16_t newXorOut) {
#ifdef DEBUG
      std::cout << std::hex << "CRC16: set xorOut to:" << newXorOut << std::endl;
#endif
      xorOut = newXorOut;
    }
    
    void CRC16_impl::setPayloadSize(size_t newSize) {
#ifdef DEBUG
      std::cout << "CRC16: set payloadSize to:" << newSize << std::endl;
#endif
      payloadSize = newSize;
    }
  } /* namespace LibreLoRa */
} /* namespace gr */

