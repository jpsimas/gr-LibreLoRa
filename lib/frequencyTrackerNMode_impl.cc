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
    frequencyTrackerNMode<T>::make(float mu, size_t SF, size_t OSF, const std::vector<gr_complex>& window)
    {
      return gnuradio::get_initial_sptr
        (new frequencyTrackerNMode_impl<T>(mu, SF, OSF, window));
    }


    /*
     * The private constructor
     */
    template<typename T>
    frequencyTrackerNMode_impl<T>::frequencyTrackerNMode_impl(float mu, size_t SF, size_t OSF, const std::vector<gr_complex>& window)
      : gr::sync_block("frequencyTrackerNMode",
		  gr::io_signature::make(1, 1, sizeof(gr_complex)),
		  gr::io_signature::make(1, 1, sizeof(T))),
	window(window),
	windowedSig(window.size()),
	mu(mu),
	OSF(OSF),
	SF(SF),
	wStep(std::polar<float>(1, 2*M_PI*1.0/((1 << SF)*OSF*OSF))),
	w(1.0),
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

	for(auto& x : count)
	  x = 0;
	for(auto i = 0; i < windowedSig.size() - 1; i++) {
	  int16_t estimate = int16_t(std::round(((1 << SF)*OSF)*(std::arg(std::conj(windowedSig[i])*windowedSig[i + 1])/(2*M_PI) + 1.0f)))%int16_t((1 << SF)*OSF);
	  count[estimate]++;
	}

	
	size_t maxK = 0;
	size_t maxCount = 0;
	for(auto k = 0; k < (1 << SF)*OSF; k++)
	  if(count[k] > maxCount){
	    maxK = k;
	    maxCount = count[k];
	  }
	  
	w = (1 - mu)*w + mu*std::polar<float>(1.0f, 2*M_PI*float(maxK)/((1 << SF)*OSF));
	
	out[i] = calcFreq(w);
      }

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

    template class frequencyTrackerNMode<float>;
    template class frequencyTrackerNMode<gr_complex>;
  } /* namespace LibreLoRa */
} /* namespace gr */

