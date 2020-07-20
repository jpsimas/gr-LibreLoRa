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

#ifndef INCLUDED_LIBRELORA_FREQUENCYTACKERAPA_IMPL_H
#define INCLUDED_LIBRELORA_FREQUENCYTACKERAPA_IMPL_H

#include <LibreLoRa/frequencyTackerAPA.h>

#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Eigenvalues> 

//#include <eigen3/unsupported/Eigen/Polynomials>

namespace gr {
  namespace LibreLoRa {

    template<size_t N>
    class frequencyTackerAPA_impl : public frequencyTackerAPA<N>
    {
     private:
      // Nothing to declare in this block.
      using Vect = Eigen::Matrix<gr_complex, N, 1>;
      using VectD = Eigen::Matrix<gr_complex, Eigen::Dynamic, 1>;//K
      const size_t K;

      //Eigen::PolynomialSolver<double, Eigen::Matrix<gr_complex, N, 1>> solver;

      Eigen::Matrix<gr_complex, N, N> Pmatr;
      //Vect w;
      float mu;
      
      const std::vector<gr_complex> window;
      
      Eigen::Matrix<gr_complex, N, Eigen::Dynamic> U;
     public:
      frequencyTackerAPA_impl(float mu, const std::vector<gr_complex>& window);
      ~frequencyTackerAPA_impl();

      // Where all the action really happens
      int work(
              int noutput_items,
              gr_vector_const_void_star &input_items,
              gr_vector_void_star &output_items
      );

      void forecast (int noutput_items, gr_vector_int &ninput_items_required);
    };

  } // namespace LibreLoRa
} // namespace gr

#endif /* INCLUDED_LIBRELORA_FREQUENCYTACKERAPA_IMPL_H */

