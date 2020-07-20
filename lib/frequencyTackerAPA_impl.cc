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
#include "frequencyTackerAPA_impl.h"

namespace gr {
  namespace LibreLoRa {

    template<size_t N>
    typename frequencyTackerAPA<N>::sptr
    frequencyTackerAPA<N>::make(float mu, const std::vector<gr_complex>& window)
    {
      return gnuradio::get_initial_sptr
        (new frequencyTackerAPA_impl<N>(mu, window));
    }


    /*
     * The private constructor
     */
    template<size_t N>
    frequencyTackerAPA_impl<N>::frequencyTackerAPA_impl(float mu, const std::vector<gr_complex>& window)
      : gr::sync_block("frequencyTackerAPA",
		       gr::io_signature::make(1, 1, sizeof(gr_complex)),
		       gr::io_signature::make(1, 1, sizeof(float))),
		       // gr::io_signature::make(1, 1, N*sizeof(float))),
	K(window.size()),
	//w(Vect::Zero()),
	mu(mu),
	window(window),
	U(N, K)
    {
      Pmatr = decltype(Pmatr)::Zero();
      Pmatr.template block<N - 1, N - 1>(0, 1) = Eigen::Matrix<gr_complex, N - 1, N - 1>::Identity();

#ifdef NDEBUG
      std::cout << "frequencyTrackerAPA: constructed." << std::endl;
#endif
    }

    /*
     * Our virtual destructor.
     */
    template<size_t N> 
    frequencyTackerAPA_impl<N>::~frequencyTackerAPA_impl()
    {
    }

    template<size_t N>
    void
    frequencyTackerAPA_impl<N>::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      ninput_items_required[0] = noutput_items + 1 + K;
    }

    template<size_t N>
    int
    frequencyTackerAPA_impl<N>::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      float *out = (float *) output_items[0];
      
      auto w = Pmatr.col(0);
      
      for(int i = 0; i < noutput_items; i++) {
	VectD d = Eigen::Map<VectD>(const_cast<gr_complex*>(in + i), K);
	for(size_t j = 0; j < d.rows(); j++)
	  U.col(j) = Eigen::Map<Vect>(const_cast<gr_complex*>(in + i + 1 + j));

	gr_complex y = w.dot(U.col(U.cols()-1));
	gr_complex err = d(d.rows()-1) - y;

	VectD wu = U.colPivHouseholderQr().solve(w);
	VectD bu = (U.adjoint()*U).colPivHouseholderQr().solve(d);
  
	w += mu*U*(bu - wu);

	out[i] = std::arg(w(0))/(2*M_PI);
	
	// Vect roots = Pmatr.eigenvalues();

	// size_t minK = 0;
	// float minDist = std::numeric_limits<float>::infinity();

	// for(auto k = 0; k < N; k++) {
	//   float dist = std::abs(std::norm(roots(k)) - 1.0f);
	//   if(dist < minDist){
	//     minK = k;
	//     minDist = dist;
	//   }
	// }

	// out[i] = std::arg(roots(minK))/(2*M_PI);
	
	// for(auto k = 0; k < N; k++)
	//   out[N*i + k] = -std::arg(roots(k))/(2*M_PI);
      }

      this->consume_each (noutput_items);
      // Tell runtime system how many output items we produced.
      return noutput_items;
    }
    
    template class frequencyTackerAPA<1>;
    template class frequencyTackerAPA<2>;
  } /* namespace LibreLoRa */
} /* namespace gr */

