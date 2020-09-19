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

#ifndef INCLUDED_LIBRELORA_SYMBOLMOD_IMPL_H
#define INCLUDED_LIBRELORA_SYMBOLMOD_IMPL_H

#include <LibreLoRa/SymbolMod.h>

namespace gr {
  namespace LibreLoRa {

    class SymbolMod_impl : public SymbolMod
    {
    private:
      size_t SF;
      size_t SFCurrent;
      size_t symbolSize;

      void setSF(size_t SFNew);
      void setSFCurrent(size_t SFNew);
      void setSymbolSize(size_t symbolSizeNew);
     public:
      SymbolMod_impl(size_t SF, size_t symbolSize);
      ~SymbolMod_impl();

      // Where all the action really happens
      int work(
              int noutput_items,
              gr_vector_const_void_star &input_items,
              gr_vector_void_star &output_items
      );
    };

  } // namespace LibreLoRa
} // namespace gr

#endif /* INCLUDED_LIBRELORA_SYMBOLMOD_IMPL_H */

