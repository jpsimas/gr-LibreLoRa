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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "frequencyTrackerNMode_impl.h"
#include <cmath>
#include <complex>
#include <volk/volk.h>

#ifndef NDEBUG
#include <iostream>
#endif

namespace gr {
  namespace LibreLoRa {

    template<typename T>
    typename frequencyTrackerNMode<T>::sptr
    frequencyTrackerNMode<T>::make(float mu, size_t SF, size_t OSF, const std::vector<gr_complex>& window, size_t nFreqs)
    {
      return gnuradio::make_block_sptr<frequencyTrackerNMode_impl<T>>(
        mu, SF, OSF, window, nFreqs);
    }

    template<typename T>
    constexpr const std::vector<gr_complex> frequencyTrackerNMode_impl<T>::generateExponents(size_t nFreqs, size_t N) {
      std::vector<gr_complex> exponents(N*nFreqs);
      for(auto j = 0; j < nFreqs; j++)
	for(auto i = 0; i < N; i++)
	  exponents[i + j*N] = std::polar<float>(1.0f, -(2*M_PI/nFreqs)*i*j);
      return exponents;
    }
    
    /*
     * The private constructor
     */
    template<typename T>
    frequencyTrackerNMode_impl<T>::frequencyTrackerNMode_impl(float mu, size_t SF, size_t OSF, const std::vector<gr_complex>& window, size_t nFreqs)
      : gr::sync_block("frequencyTrackerNMode",
		  gr::io_signature::make(1, 1, sizeof(gr_complex)),
		  gr::io_signature::make(1, 1, sizeof(T))),
	window(window),
	nFreqs(nFreqs),
	windowedSig(window.size()),
	projections(nFreqs),
	exponents(generateExponents(nFreqs, window.size())),
	mu(mu),
	OSF(OSF),
	SF(SF),
	wStep(std::polar<float>(1, 2*M_PI*1.0/((1 << SF)*OSF*OSF))),
	w(1.0),
	freq(0.0),
	count((1 << SF)*OSF)
    {
      this->set_history(window.size());
#ifndef NDEBUG
      std::cout << "frequencyTrackerNMode: constructed. window size:" << window.size() << std::endl;
      
#endif
    }

    /*
     * Our virtual destructor.
     */
    template<typename T>
    frequencyTrackerNMode_impl<T>::~frequencyTrackerNMode_impl()
    {
    }

    template <>
    float frequencyTrackerNMode_impl<float>::calcFreq(gr_complex w) {
      //return std::arg(w)/(2*M_PI*OSF);
      return std::arg(w)/(2*M_PI);
    }

    template <>
    gr_complex frequencyTrackerNMode_impl<gr_complex>::calcFreq(gr_complex w) {
      return std::polar<float>(1.0, std::arg(w)*OSF);
      //return w;
    }
    
    template<typename T>
    int
    frequencyTrackerNMode_impl<T>::work (int noutput_items,
					 gr_vector_const_void_star &input_items,
					 gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      T *out = (T *) output_items[0];

      // Do <+signal processing+>

      for(int i = 0; i < noutput_items; i++) {
	volk_32fc_x2_multiply_32fc(windowedSig.data(), in + i, window.data(), window.size());

	for(auto& x : projections)
	  x = 0;
	
	for(auto m = 0; m < nFreqs; m++){
	  // for(auto k = 0; k < window.size(); k++){
	    // projections[m] += std::polar<float>(1.0f, -(2.0*M_PI/nFreqs)*m*k)*windowedSig[k];
	    // projections[m] += exponents[(m*k)%nFreqs]*windowedSig[k];
	  // }
	  volk_32fc_x2_dot_prod_32fc(projections.data() + m, windowedSig.data(), exponents.data() + m*window.size(), window.size());
	}

	uint32_t ind;
	volk_32fc_index_max_32u(&ind, projections.data(), nFreqs);

	freq = (1 - mu)*freq + mu*(float((ind + nFreqs/2)%nFreqs)/nFreqs - 0.5);
	out[i] = freq;
	
	// w = (1 - mu)*w + mu*std::polar<float>(1.0f, 2*M_PI*float(maxK)/((1 << SF)*OSF));
	
	// out[i] = calcFreq(w);
      }

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

    template class frequencyTrackerNMode<float>;
    template class frequencyTrackerNMode<gr_complex>;
  } /* namespace LibreLoRa */
} /* namespace gr */

