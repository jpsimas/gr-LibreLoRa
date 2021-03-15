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
#include "frequencyTrackerN_impl.h"
#include <cmath>
#include <complex>
#include <volk/volk.h>

#ifndef NDEBUG
#include <iostream>
#endif

namespace gr {
  namespace LibreLoRa {

    template<typename T>
    typename frequencyTrackerN<T>::sptr
    frequencyTrackerN<T>::make(float mu, size_t SF, size_t OSF, const std::vector<gr_complex>& window)
    {
      return gnuradio::make_block_sptr<frequencyTrackerN_impl<T>>(
        mu, SF, OSF, window);
    }


    /*
     * The private constructor
     */
    template<typename T>
    frequencyTrackerN_impl<T>::frequencyTrackerN_impl(float mu, size_t SF, size_t OSF, const std::vector<gr_complex>& window)
      : gr::sync_block("frequencyTrackerN",
		  gr::io_signature::make(1, 1, sizeof(gr_complex)),
		  gr::io_signature::make(1, 1, sizeof(T))),
	window(window),
	windowedSig(window.size()),
	// windowedSigNorm(window.size()),
	exponents(window.size()),
	exponentsTimesI(window.size()),
	mu(mu),
	OSF(OSF),
	wStep(std::polar<float>(1, 2*M_PI*1.0/((1 << SF)*OSF*OSF))),
	w(0.1)
    {
      this->set_history(window.size());
      for(auto i = 0; i < window.size(); i++) {
	exponents[i] = 1.0;
	exponentsTimesI[i] = float(i);
      }
#ifndef NDEBUG
      std::cout << "frequencyTrackerN: constructed. window size:" << window.size() << std::endl;
      
#endif
    }

    /*
     * Our virtual destructor.
     */
    template<typename T>
    frequencyTrackerN_impl<T>::~frequencyTrackerN_impl()
    {
    }

    template <>
    float frequencyTrackerN_impl<float>::calcFreq() {
      //return std::arg(w)/(2*M_PI*OSF);
      return -std::arg(w)/(2*M_PI);
    }

    template <>
    gr_complex frequencyTrackerN_impl<gr_complex>::calcFreq() {
      return std::polar<float>(1.0, std::arg(w)*OSF);
      //return w;
    }
    
    template<typename T>
    int
    frequencyTrackerN_impl<T>::work (int noutput_items,
					 gr_vector_const_void_star &input_items,
					 gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      T *out = (T *) output_items[0];

      // Do <+signal processing+>

      for(int i = 0; i < noutput_items; i++) {
	
	volk_32fc_x2_multiply_32fc(windowedSig.data(), in + i, window.data(), window.size());

	gr_complex prod;
	// volk_32fc_x2_conjugate_dot_prod_32fc(&prod, windowedSig.data(), windowedSig.data() + 1, windowedSig.size() - 1);
	volk_32fc_x2_conjugate_dot_prod_32fc(&prod, windowedSig.data(), exponents.data(), window.size());

	gr_complex prodI;
	volk_32fc_x2_conjugate_dot_prod_32fc(&prodI, windowedSig.data(), exponentsTimesI.data(), window.size());	
	
	// w = (1 - mu)*w + mu*prod/(std::abs(prod) + 1e-6f);

	// out[i] = calcFreq();
	
	w = w - mu*gr_complex(0, 1)*w*std::conj(prod)*prodI;

	w = std::remainder(std::real(w), 2*M_PI);
	
	exponents[0] = std::polar<float>(1.0, -std::real(w));
	for(auto j = 1; j < window.size(); j++)
	  exponents[j] = exponents[j - 1]*exponents[0];
	
	for(auto j = 0; j < window.size(); j++)
	  exponentsTimesI[j] = float(j)*exponents[j];
	
	out[i] = std::real(w)/(2*M_PI);
	
      }
      
      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

    template class frequencyTrackerN<float>;
    template class frequencyTrackerN<gr_complex>;
  } /* namespace LibreLoRa */
} /* namespace gr */

