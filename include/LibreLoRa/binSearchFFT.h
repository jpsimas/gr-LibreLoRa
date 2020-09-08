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

#ifndef INCLUDED_LIBRELORA_BINSEARCHFFT_FUNC_H
#define INCLUDED_LIBRELORA_BINSEARCHFFT_FUNC_H

#include <LibreLoRa/api.h>
#include <vector>
#include <complex>

namespace gr {
  namespace LibreLoRa {

    /*!
     * \brief <+description+>
     *
     */      
      
    
    template<typename T = float>
    constexpr std::pair<std::complex<T>, int> binSearchFFT(const std::complex<T>* data, const size_t N, const int sector = 0, const int step = 2) {
  
      // size_t count = 0;
      std::pair<std::vector<std::complex<T>>, std::vector<std::complex<T>>> results(N/2, N/2);
  
      const std::complex<T> c0 = std::polar<T>(1, -(2*M_PI*sector/step));
      const std::complex<T> c1 = std::polar<T>(1, -(2*M_PI*(sector + step/2)/step));

      std::pair<std::complex<T>, std::complex<T>> resSum(0.0, 0.0);
      for(auto i = 0; i < N/2; i++) {
	results.first[i] = data[i] + c0*data[i + N/2];
	results.second[i] = data[i] + c1*data[i + N/2];
    
	// count = (std::norm(results.first[i/2]) >= std::norm(results.second[i/2]))? (count + 1) : count;
	resSum.first += results.first[i];
	resSum.second += results.second[i];
      }
      const bool isFirst = std::norm(resSum.first) >= std::norm(resSum.second);

      // const bool isFirst = (count >= N/2);
  
      if(isFirst) {
	return (N == 2)?
	  std::make_pair(results.first[0], sector) :
	  binSearchFFT<T>(results.first.data(), N/2, sector, 2*step);
      } else {
	return (N == 2)?
	  std::make_pair(results.second[0], sector + step/2):
	  binSearchFFT<T>(results.second.data(), N/2, sector + step/2, 2*step);
      }
    }

  } // namespace LibreLoRa
} // namespace gr

#endif /* INCLUDED_LIBRELORA_BINSEARCHFFT_FUNC_H */

