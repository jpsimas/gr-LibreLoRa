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

    std::array<uint8_t, 4> getCheckMatrix(size_t CR) {
      static std::unordered_map<size_t, std::array<uint8_t, 4>> checkMatrices;
      if(checkMatrices.find(CR) != checkMatrices.end())
	return checkMatrices[CR];
      else {
	std::array<uint8_t, 4> checkMatrix;
	auto pairityMatrix = getPairityMatrix(CR);
	for(size_t i = 0; i < 4; i++)
	  checkMatrix[i] = calculatePairity(1 << i, pairityMatrix);
	checkMatrices[CR] = checkMatrix;
	return checkMatrix;
      }	
    }

    std::unordered_map<uint8_t, uint8_t> getCosetLeaders(size_t CR) {
      static std::unordered_map<size_t, std::unordered_map<uint8_t, uint8_t>> cosetLeaders;
      if(cosetLeaders.find(CR) != cosetLeaders.end())
	return cosetLeaders[CR];
      else {
	auto checkMatrix = getCheckMatrix(CR);
	std::unordered_map<uint8_t, uint8_t> cosetLeader;
	for(size_t i = 0; i + 1 < (CR + 4); i++)
	  for(size_t j = 0; j < (CR + 4); j++) {
	    uint8_t syndrome = checkMatrix[i]^checkMatrix[j];
	    if(cosetLeader.find(syndrome) != cosetLeader.end())
	      cosetLeader[syndrome] = (1 << i)&(1 << j);
	  }
	cosetLeaders[CR] = cosetLeader;
	return cosetLeader;
      }
    }
    
  } /* namespace LibreLoRa */
} /* namespace gr */
