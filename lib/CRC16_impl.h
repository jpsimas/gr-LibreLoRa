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

#ifndef INCLUDED_LIBRELORA_CRC16_IMPL_H
#define INCLUDED_LIBRELORA_CRC16_IMPL_H

#include <LibreLoRa/CRC16.h>

namespace gr {
  namespace LibreLoRa {

    class CRC16_impl : public CRC16
    {
     private:
      // Nothing to declare in this block.
      uint16_t polynomial;
      uint16_t xorOut;

      size_t payloadSize;//inBytes

      pmt::pmt_t crcOutPort;
     public:
      CRC16_impl(uint16_t polynomial, uint16_t initial, size_t payloadSize);
      ~CRC16_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
           gr_vector_int &ninput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);

      void setXorOut(uint16_t newXorOut);
      void setPayloadSize(size_t newSize);
    };
    
  } // namespace LibreLoRa
} // namespace gr

#endif /* INCLUDED_LIBRELORA_CRC16_IMPL_H */

