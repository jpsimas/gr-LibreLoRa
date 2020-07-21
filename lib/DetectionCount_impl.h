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

#ifndef INCLUDED_LIBRELORA_DETECTIONCOUNT_IMPL_H
#define INCLUDED_LIBRELORA_DETECTIONCOUNT_IMPL_H

#include <LibreLoRa/DetectionCount.h>

namespace gr {
  namespace LibreLoRa {

    class DetectionCount_impl : public DetectionCount
    {
     private:
      float BW;
      size_t nChannels;
      void readMessage(pmt::pmt_t msg);
      static constexpr size_t SFMin = 7;
      static constexpr size_t SFMax = 12;
      const float channelWidth;
      std::vector<unsigned> count;

      pmt::pmt_t countPort;
     public:
      DetectionCount_impl(float BW, size_t nChannels);
      ~DetectionCount_impl();

      // Where all the action really happens
      int work(
              int noutput_items,
              gr_vector_const_void_star &input_items,
              gr_vector_void_star &output_items
      );
    };

  } // namespace LibreLoRa
} // namespace gr

#endif /* INCLUDED_LIBRELORA_DETECTIONCOUNT_IMPL_H */

