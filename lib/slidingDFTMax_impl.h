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

#ifndef INCLUDED_LIBRELORA_SLIDINGDFTMAX_IMPL_H
#define INCLUDED_LIBRELORA_SLIDINGDFTMAX_IMPL_H

#include <LibreLoRa/slidingDFTMax.h>

#include <vector>

namespace gr {
  namespace LibreLoRa {

    class slidingDFTMax_impl : public slidingDFTMax
    {
    private:
      // Nothing to declare in this block.
      const size_t length;
      const size_t windowSize;

      static constexpr float alpha = 1 - 1e-6;//for stability
      const float alphaN = std::pow(alpha, windowSize);
      
      const gr_complex* exponents;
      const gr_complex* exponentsN;
      gr_complex* DFT;
      gr_complex* deltas;

      const gr_complex* generateExponents();
      const gr_complex* generateExponentsN();
      
      // const float beta;
      
      // gr_complex step;
      // const gr_complex stepStep;
      // gr_complex e0;
      // gr_complex eN;

      // float offset;
    public:
      slidingDFTMax_impl(size_t DFTLength, size_t windowSize);
      ~slidingDFTMax_impl();

      // Where all the action really happens
      // void forecast (int noutput_items, gr_vector_int &ninput_items_required);
      int work(
	       int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items
	       );
    };

  } // namespace LibreLoRa
} // namespace gr

#endif /* INCLUDED_LIBRELORA_SLIDINGDFTMAX_IMPL_H */

