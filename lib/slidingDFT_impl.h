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

#ifndef INCLUDED_LIBRELORA_SLIDINGDFT_IMPL_H
#define INCLUDED_LIBRELORA_SLIDINGDFT_IMPL_H

#include <LibreLoRa/slidingDFT.h>
#include <vector>

namespace gr {
  namespace LibreLoRa {

    class slidingDFT_impl : public slidingDFT
    {
     private:
      // Nothing to declare in this block.
      size_t length;
      // std::vector<gr_complex> exponents;
      // std::vector<gr_complex> DFT;
      gr_complex* exponents;
      gr_complex* DFT;

      const float alpha;//for stability
      const float alphaN;

      const float beta;
      
      gr_complex step;
      gr_complex stepN;
      size_t index;

      gr_complex a;
     public:
      slidingDFT_impl(size_t DFTLength, float beta);
      ~slidingDFT_impl();

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

#endif /* INCLUDED_LIBRELORA_SLIDINGDFT_IMPL_H */

