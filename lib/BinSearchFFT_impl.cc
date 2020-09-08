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
#include "BinSearchFFT_impl.h"

//#include <LibreLoRa/binSearchFFT.h>

namespace gr {
  namespace LibreLoRa {

    BinSearchFFT::sptr
    BinSearchFFT::make(size_t DFTSize)
    {
      return gnuradio::get_initial_sptr
        (new BinSearchFFT_impl(DFTSize));
    }


    /*
     * The private constructor
     */
    BinSearchFFT_impl::BinSearchFFT_impl(size_t DFTSize)
      : gr::sync_block("BinSearchFFT",
		       gr::io_signature::make(1, 1, DFTSize*sizeof(gr_complex)),
		       gr::io_signature::make2(1, 2, sizeof(uint32_t), sizeof(gr_complex))),
	size(DFTSize),
	initVal(0),
	results(size/2, size/2)
    {}

    /*
     * Our virtual destructor.
     */
    BinSearchFFT_impl::~BinSearchFFT_impl()
    {
    }
    
    int
    BinSearchFFT_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      uint32_t *out_bin = (uint32_t *) output_items[0];
      
      gr_complex *out_ampl;
      if(output_items.size() > 1)
	out_ampl = (gr_complex *) output_items[1];

      // Do <+signal processing+>
      for(size_t i = 0; i < noutput_items; i++){
	auto [ampl, bin] = findPeak(in + i*size, size, initVal);
	initVal = bin%size;
	if(output_items.size() > 1)
	  out_ampl[i] = ampl;
	out_bin[i] = bin;
      }

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace LibreLoRa */
} /* namespace gr */

