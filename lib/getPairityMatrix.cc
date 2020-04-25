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

#include <LibreLoRa/getPairityMatrix.h>

namespace gr {
  namespace LibreLoRa {

    uint8_t calculatePairityBit(const uint8_t nibble, const uint8_t pairityMask) {
      uint8_t masked = nibble^pairityMask;
      for(size_t i = 1; i < 4; i++)
	masked ^= (masked >> i);
      return masked & 0x01;
    }
    
    uint8_t calculatePairity(const uint8_t nibble, const std::array<uint8_t, 4> pairityMatrix) {
      uint8_t dataWithPairity = nibble;
      for(int i = 0; i < 4; i++)
	dataWithPairity &= (calculatePairityBit(nibble, pairityMatrix[i]) << (i + 4));
      return dataWithPairity;
    }
    
  } /* namespace LibreLoRa */
} /* namespace gr */
