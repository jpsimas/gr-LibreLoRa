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

namespace gr {
  namespace LibreLoRa {
    constexpr std::array<uint8_t, 4> getPairityMatrix(size_t CR);
    uint8_t calcPairityBit(const uint8_t nibble, const uint8_t pairityMask);
    uint8_t calcPairity(const uint8_t nibble, const std::array<uint8_t, 4> pairityMatrix);
  } /* namespace LibreLoRa */
} /* namespace gr */

#endif
