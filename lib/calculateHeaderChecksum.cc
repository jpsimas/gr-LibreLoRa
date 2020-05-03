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
#include <LibreLoRa/calculateHeaderChecksum.h>
#include <LibreLoRa/utilities.h>

namespace gr {
  namespace LibreLoRa {
    
    uint8_t calculateHeaderChecksumBytes(const uint16_t headerData) {
      uint8_t checkSum = 0x00;
      constexpr std::array<uint16_t, 5> masks = {0xF21, 0x752, 0xA94, 0x1E8, 0x00F};
      for(size_t i = 0; i < masks.size(); i++)
	checkSum |= pairity(masks[i]&headerData) << i;
      return checkSum;
    }

    uint8_t calculateHeaderChecksum(const uint32_t headerNibbles) {
      return calculateHeaderChecksumBytes(nibbles2bytes<uint16_t>(headerNibbles, 3));
    }
    
  } /* namespace LibreLoRa */
} /* namespace gr */

