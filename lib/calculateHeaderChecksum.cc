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

namespace gr {
  namespace LibreLoRa {

    constexpr uint8_t nibblePairity(const uint8_t nibble) {
      return (nibble ^ (nibble >> 1) ^ (nibble >> 2) ^ (nibble >> 3)) & 0x1;
    }
    
    uint8_t calculateHeaderChecksum(const uint16_t headerData) {
      uint8_t checkSum = 0x00;
      constexpr std::array<uint16_t, 5> masks = {0xF21, 0x752, 0xA94, 0x1E8, 0x00F};
      for(size_t i = 0; i < masks.size(); i++)
	checkSum |= nibblePairity(masks[i]&headerData) << i;
      return checkSum;
    }

  } /* namespace LibreLoRa */
} /* namespace gr */

