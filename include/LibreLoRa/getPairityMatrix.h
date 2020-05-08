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

#ifndef GETPAIRITYMATRIX_H
#define GETPAIRITYMATRIX_H

#include <array>
#include <cstdint>
#include <unordered_map>
#include <LibreLoRa/utilities.h>

namespace gr {
  namespace LibreLoRa {
    constexpr std::array<uint8_t, 4> getPairityMatrix(size_t CR) {
      switch(CR){
      case 4:
	return {0x07, 0x0e, 0x0b, 0x0d};
      case 3:
	return {0x07, 0x0e, 0x0b, 0x00};
      case 2:
	return {0x07, 0x0e, 0x00, 0x00};
      case 1:
	return {0x0f, 0x00, 0x00, 0x00};
      default:
	return {0x00, 0x00, 0x00, 0x00};
      }
    }
    
    constexpr uint8_t calculatePairityBit(const uint8_t nibble, const uint8_t pairityMask) {
      return pairity<uint8_t>(nibble&pairityMask, 4);
    }

    constexpr uint8_t calculatePairity(const uint8_t nibble, const std::array<uint8_t, 4> pairityMatrix, size_t N = 0) {
      return (N == 4)?
	nibble :
	calculatePairity(nibble | (calculatePairityBit(nibble, pairityMatrix[N]) << (N + 4)), pairityMatrix, N + 1);
    }

    std::array<uint8_t, 4> getCheckMatrix(size_t CR);
    std::unordered_map<uint8_t, uint8_t> getCosetLeaders(size_t CR);
    
  } /* namespace LibreLoRa */
} /* namespace gr */

#endif
