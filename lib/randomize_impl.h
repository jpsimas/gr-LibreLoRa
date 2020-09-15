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

#ifndef INCLUDED_LIBRELORA_RANDOMIZE_IMPL_H
#define INCLUDED_LIBRELORA_RANDOMIZE_IMPL_H

#include <LibreLoRa/randomize.h>

namespace gr {
  namespace LibreLoRa {

    class randomize_impl : public randomize
    {
     private:
      const uint8_t lfsrInitialState;
      uint8_t lfsrState;
      size_t payloadSize;

      size_t byteCount;
      
     public:
      randomize_impl(const uint8_t lfsrInitialState, const size_t payloadSize);
      ~randomize_impl();

      // Where all the action really happens
      int work(
              int noutput_items,
              gr_vector_const_void_star &input_items,
              gr_vector_void_star &output_items
      );

      void reset();
      void setLfsrState(uint8_t state);
      void setPayloadSize(size_t payloadSizeNew);
    };

  } // namespace LibreLoRa
} // namespace gr

#endif /* INCLUDED_LIBRELORA_RANDOMIZE_IMPL_H */

