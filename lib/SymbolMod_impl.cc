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
#include "SymbolMod_impl.h"
#include <LibreLoRa/getSymbol.h>

namespace gr {
  namespace LibreLoRa {

    SymbolMod::sptr
    SymbolMod::make(size_t SF, size_t symbolSize)
    {
      return gnuradio::get_initial_sptr
        (new SymbolMod_impl(SF, symbolSize));
    }


    /*
     * The private constructor
     */
    SymbolMod_impl::SymbolMod_impl(size_t SF, size_t symbolSize)
      : gr::sync_interpolator("SymbolMod",
			      gr::io_signature::make(1, 1, sizeof(uint16_t)),
			      gr::io_signature::make(1, 1, sizeof(float)), symbolSize),
	SF(SF),
	symbolSize(symbolSize)
    {
#ifndef NDEBUG
      std::cout << "SymbolMod: constructed" << std::endl;
#endif
    }

    /*
     * Our virtual destructor.
     */
    SymbolMod_impl::~SymbolMod_impl()
    {
    }

    int
    SymbolMod_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const uint16_t *in = (const uint16_t *) input_items[0];
      float *out = (float *) output_items[0];

      for(auto i = 0; i < noutput_items/symbolSize; i++){
	const std::vector<float> symi = getSymbol<float>(in[i], SF, symbolSize);
	memcpy(out + i*symbolSize, symi.data(), symbolSize*sizeof(float));
      }

      // Tell runtime system how many output items we produced.
      return (noutput_items/symbolSize)*symbolSize;
    }

  } /* namespace LibreLoRa */
} /* namespace gr */

