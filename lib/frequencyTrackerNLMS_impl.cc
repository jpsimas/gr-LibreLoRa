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

    typename frequencyTrackerNLMS::sptr
    frequencyTrackerNLMS::make(float mu, const std::vector<gr_complex>& window)
    {
      return gnuradio::make_block_sptr<frequencyTrackerNLMS_impl>(
        mu, window);
    }


    /*
     * The private constructor
     */
    frequencyTrackerNLMS_impl::frequencyTrackerNLMS_impl(float mu, const std::vector<gr_complex>& window)
      : gr::sync_block("frequencyTrackerNLMS",
		       gr::io_signature::make(1, 1, sizeof(gr_complex)),
		       gr::io_signature::make2(1, 2, sizeof(gr_complex), sizeof(float))),
		       // gr::io_signature::make2(1, 2, (window.size() - 1)*sizeof(gr_complex), sizeof(float))),
	w(window.size() - 1),
	mu(mu),
	window(window),
	windowedSig(window.size()),
	xEst(M_PI/2.0f)
    {
      this->set_history(window.size());

      wEst = std::polar(1.0f, std::real(xEst));
      
      for(auto i = 0; i < w.size(); i++)
	w[i] = 0.0f;
	
#ifdef NDEBUG
      std::cout << "frequencyTrackerAPA: constructed." << std::endl;
#endif
    }

    /*
     * Our virtual destructor.
     */
    frequencyTrackerNLMS_impl::~frequencyTrackerNLMS_impl()
    {
    }

    int
    frequencyTrackerNLMS_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      gr_complex *out = (gr_complex *) output_items[0];
      float *out_err;
      if(output_items.size() > 1)
	out_err = (float *) output_items[1];
     
      
      for(int i = 0; i < noutput_items; i++) {
	// gr_complex d = in[i + N];
	// const gr_complex* u = in + i;
	 volk_32fc_x2_multiply_32fc(windowedSig.data(), in + i, window.data(), window.size());
	
	const gr_complex* u = windowedSig.data();
	const gr_complex d = windowedSig[window.size() - 1];
	  
	gr_complex y;
	volk_32fc_x2_conjugate_dot_prod_32fc(&y, u, w.data(), w.size());

	gr_complex normU;
	volk_32fc_x2_conjugate_dot_prod_32fc(&normU, u, u, w.size());

	const auto err = std::conj(d - y);
	
	for(auto j = 0; j < w.size(); j++)
	  w[j] += mu*err*windowedSig[j]/(normU + 1e-6f);
	
	// if(N == 2) {
	//   auto r1 = (w[1] + std::sqrt(w[1]*w[1] + 4.0f*w[0]))/2.0f;
	//   auto r2 = (w[1] - std::sqrt(w[1]*w[1] + 4.0f*w[0]))/2.0f;
	//   if(std::abs(std::abs(r1) - 1.0f) < std::abs(std::abs(r2) - 1.0f))
	//     out[i] = -std::arg(r1)/(2*M_PI);
	//   else
	//     out[i] = -std::arg(r2)/(2*M_PI);
	// } else
	//   out[i] = -std::arg(w[0])/(2*M_PI);

	// wEst = windowedSig[1]/(windowedSig[0] + 1e-12f);
	// xEst = std::arg(wEst);
	
	// const float muRoot = 1e-15;
	
	// constexpr size_t nIter = 0;//3;
	// for(auto i = 0; i < nIter; i++) {
	//   gr_complex p, pPrime, zi;
	//   p = 0;
	//   pPrime = 0;
	//   zi = 1;
	//   // p = -windowedSig[window.size() - 1];
	//   for(auto j = 0; j < w.size() - 1; j++) {
	//     p += w[j]*zi;
	//     pPrime += float(j + 1)*w[j + 1]*zi;
	//     zi *= wEst;
	//   }
	  
	//   p += w[w.size() - 1]*zi;
	//   pPrime += -float(w.size())*zi;

	//   zi *= wEst;
	 
	//   p += -zi;
	  
	//   xEst -= muRoot*std::real(std::conj(p)*pPrime*gr_complex(0, 1));
	//   wEst = std::polar(1.0f, std::real(xEst));
	// }
	
	// out[i] = wEst;
	out[i] = y;
	 //memcpy(out + i*w.size(), w.data(), sizeof(gr_complex)*w.size());

	if(output_items.size() > 1)
	  out_err[i] = std::norm(err);
      }

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace LibreLoRa */
} /* namespace gr */

