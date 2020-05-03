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

#ifndef INCLUDED_LIBRELORA_UTILITIES_H
#define INCLUDED_LIBRELORA_UTILITIES_H

#include <LibreLoRa/api.h>

namespace gr {
  namespace LibreLoRa {

    /*!
     * \brief <+description+>
     *
     */
    template <typename T>
    constexpr T pairity(T x, size_t N = (sizeof(T) << 3)) {
      return ((N == 1)? x : (x^pairity<T>(x >> 1, N - 1))) & 0x01;
    }
    
    template <typename T2, typename T1>
    constexpr T2 nibbles2bytes(T1 x, size_t N = sizeof(T1)) {
      return ((N == 1)? (x & 0xf): (x & 0xf) | (nibbles2bytes<T2, T1>(x >> 8, N - 1)) << 4);
    }    
    
  } // namespace LibreLoRa
} // namespace gr

#endif /* INCLUDED_LIBRELORA_UTILITIES_H */

