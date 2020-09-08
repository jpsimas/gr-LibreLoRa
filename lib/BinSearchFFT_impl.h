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

#ifndef INCLUDED_LIBRELORA_BINSEARCHFFT_IMPL_H
#define INCLUDED_LIBRELORA_BINSEARCHFFT_IMPL_H

#include <LibreLoRa/BinSearchFFT.h>

namespace gr {
  namespace LibreLoRa {

    class BinSearchFFT_impl : public BinSearchFFT
    {
     private:
      const size_t size;
      int initVal;
      std::pair<std::vector<std::complex<float>>, std::vector<std::complex<float>>> results;
      inline std::pair<std::complex<float>, int> findPeak(const std::complex<float>* data, const size_t N, const int sector = 0, const int step = 2) {
	const std::complex<float> c0 = std::polar<float>(1, -(2*M_PI*sector/step));
	const std::complex<float> c1 = std::polar<float>(1, -(2*M_PI*(sector + step/2)/step));

	size_t count = 0;

	// std::pair<std::complex<float>, std::complex<float>> resSum(0.0, 0.0);
	for(auto i = 0; i < N/2; i++) {
	  results.first[i] = data[i] + c0*data[i + N/2];
	  results.second[i] = data[i] + c1*data[i + N/2];
    
	  count = (std::norm(results.first[i/2]) >= std::norm(results.second[i/2]))? (count + 1) : count;
	  // resSum.first += results.first[i];
	  // resSum.second += results.second[i];
	}
	// const bool isFirst = std::norm(resSum.first) >= std::norm(resSum.second);

	const bool isFirst = (count >= N/2);
  
	if(isFirst) {
	  return (N == 2)?
	    std::make_pair(results.first[0], sector) :
	    findPeak(results.first.data(), N/2, sector, 2*step);
	} else {
	  return (N == 2)?
	    std::make_pair(results.second[0], sector + step/2):
	    findPeak(results.second.data(), N/2, sector + step/2, 2*step);
	}
      }
     public:
      BinSearchFFT_impl(size_t DFTSize);
      ~BinSearchFFT_impl();

      // Where all the action really happens
      int work(
              int noutput_items,
              gr_vector_const_void_star &input_items,
              gr_vector_void_star &output_items
      );
    };

  } // namespace LibreLoRa
} // namespace gr

#endif /* INCLUDED_LIBRELORA_BINSEARCHFFT_IMPL_H */

