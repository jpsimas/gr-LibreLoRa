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
#include "readHeader_impl.h"

#include <array>
#include <LibreLoRa/calculateHeaderChecksum.h>

namespace gr {
  namespace LibreLoRa {

    readHeader::sptr
    readHeader::make(size_t SF)
    {
      return gnuradio::make_block_sptr<readHeader_impl>(
        SF);
    }


    /*
     * The private constructor
     */
    readHeader_impl::readHeader_impl(size_t SF)
      : gr::sync_decimator("readHeader",
		  gr::io_signature::make(1, 1, sizeof(uint8_t)),
			   gr::io_signature::makev(5, 5, {sizeof(uint8_t), sizeof(uint8_t), sizeof(uint8_t), sizeof(uint8_t), 7*sizeof(uint8_t)}), 5) {
    }

    /*
     * Our virtual destructor.
     */
    readHeader_impl::~readHeader_impl()
    {
    }
    
    int
    readHeader_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const uint8_t *in = (const uint8_t *) input_items[0];
      uint8_t *payloadLength = (uint8_t *) output_items[0];
      uint8_t *payloadCRCPresent = (uint8_t *) output_items[1];
      uint8_t *CR = (uint8_t *) output_items[2];
      uint8_t *headerCheckSumValid = (uint8_t *) output_items[3];
      uint8_t *dataOut = (uint8_t *) output_items[4];

      // Do <+signal processing+>
      for(size_t i = 0; i < noutput_items; i++) {
	auto inI = in + 5*i;
	payloadLength[i] = (inI[0] << 4)&(inI[1]);
	payloadCRCPresent[i] = inI[2] & 0x01;
	CR[i] = inI[2] >> 1;
	uint8_t headerCheckSum = (inI[3] << 4)&inI[4];
	uint8_t headerCheckSumCalculated = calculateHeaderChecksum(*((uint16_t*) inI));
	headerCheckSumValid[i] = (headerCheckSum == headerCheckSumCalculated);
	for(size_t j = 0; j < 7; j++)
	  dataOut[7*i + j] = inI[5 + j];
      }

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace LibreLoRa */
} /* namespace gr */

