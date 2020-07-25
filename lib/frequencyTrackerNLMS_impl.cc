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
#include "frequencyTrackerNLMS_impl.h"

#include <volk/volk.h>

namespace gr {
  namespace LibreLoRa {

    template<size_t N>
    typename frequencyTrackerNLMS<N>::sptr
    frequencyTrackerNLMS<N>::make(float mu, const std::vector<gr_complex>& window)
    {
      return gnuradio::get_initial_sptr
        (new frequencyTrackerNLMS_impl<N>(mu, window));
    }


    /*
     * The private constructor
     */
    template<size_t N>
    frequencyTrackerNLMS_impl<N>::frequencyTrackerNLMS_impl(float mu, const std::vector<gr_complex>& window)
      : gr::sync_block("frequencyTrackerNLMS",
		       gr::io_signature::make(1, 1, sizeof(gr_complex)),
		       gr::io_signature::make(1, 1, sizeof(float))),
		       // gr::io_signature::make(1, 1, N*sizeof(float))),
	w{},
	mu(mu),
	window(window)
    {
      this->set_history(N + 1);
#ifdef NDEBUG
      std::cout << "frequencyTrackerAPA: constructed." << std::endl;
#endif
    }

    /*
     * Our virtual destructor.
     */
    template<size_t N> 
    frequencyTrackerNLMS_impl<N>::~frequencyTrackerNLMS_impl()
    {
    }

    template<size_t N>
    int
    frequencyTrackerNLMS_impl<N>::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      float *out = (float *) output_items[0];
     
      
      for(int i = 0; i < noutput_items; i++) {
	gr_complex d = in[i + N];
	const gr_complex* u = in + i;

	gr_complex y;
	volk_32fc_x2_conjugate_dot_prod_32fc(&y, u, w.data(), N);

	gr_complex normU;
	volk_32fc_x2_conjugate_dot_prod_32fc(&normU, u, u, N);
	
	for(auto j = 0; j < N; j++)
	  w[j] += mu*std::conj(d - y)*u[j]/(normU + 1e-6f);

	// out[i] = -((N == 2) ?
	// 	   std::arg(w[1] + std::sqrt(w[1]*w[1] + 4.0f*w[0]))/(2*M_PI) :
	// 	   std::arg(w[0])/(2*M_PI));
	if(N == 2) {
	  auto r1 = (w[1] + std::sqrt(w[1]*w[1] + 4.0f*w[0]))/2.0f;
	  auto r2 = (w[1] - std::sqrt(w[1]*w[1] + 4.0f*w[0]))/2.0f;
	  if(std::abs(std::abs(r1) - 1.0f) < std::abs(std::abs(r2) - 1.0f))
	    out[i] = std::arg(r1)/(2*M_PI);
	  else
	    out[i] = std::arg(r2)/(2*M_PI);
	} else
	  out[i] = std::arg(w[0])/(2*M_PI);
      }

      this->consume_each (noutput_items);
      // Tell runtime system how many output items we produced.
      return noutput_items;
    }
    
    template class frequencyTrackerNLMS<1>;
    template class frequencyTrackerNLMS<2>;
  } /* namespace LibreLoRa */
} /* namespace gr */

