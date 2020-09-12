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
      return ((N == 1)?
	      (x & 0xf) :
	      (x & 0xf) | (nibbles2bytes<T2, T1>(x >> 8, N - 1)) << 4);
    }
    
    template <typename T2, typename T1>
    constexpr T2 bytes2nibbles(T1 x, size_t N = sizeof(T1)) {
      return ((N == 1)?
	      ((x & 0xf) | ((x >> 4) & 0x0f) << 8) :
	      ((x & 0xf) | ((x >> 4) & 0x0f) << 8) | (bytes2nibbles<T2, T1>(x >> 8, N - 1)) << 16);
    }
    
    template <typename T2, typename T1, size_t N2 = (sizeof(T2) << 3) + 1>
    constexpr T2 polDivRem(T1 data, T2 polynomial, size_t N = (sizeof(T1) << 3)) {
      return ((N < N2)?
	      data :
	      ((data >> (N - 1))?
	       polDivRem<T2, T1>(data^((T1(polynomial) << (N - N2)) | (T1(1) << (N - 1))), polynomial, N - 1) :
	       polDivRem<T2, T1>(data, polynomial, N - 1)
	       )
	      );
    }
      
      template <typename T>
      constexpr T invertEndianness(T val, size_t N = sizeof(T)) {
	return (N == 1)?
	  val&0xff :
	  ((val&0xff) << ((N - 1) << 3))|invertEndianness<T>(val >> 8, N - 1);
      }
    
  } // namespace LibreLoRa
} // namespace gr

#endif /* INCLUDED_LIBRELORA_UTILITIES_H */

