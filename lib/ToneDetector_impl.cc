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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "ToneDetector_impl.h"

namespace gr {
  namespace LibreLoRa {

    const pmt::pmt_t ToneDetector_impl::tagKey = pmt::intern("freq_offset");
    
    ToneDetector::sptr
    ToneDetector::make(size_t DFTSize)
    {
      return gnuradio::get_initial_sptr
        (new ToneDetector_impl(DFTSize));
    }


    /*
     * The private constructor
     */
    ToneDetector_impl::ToneDetector_impl(size_t DFTSize)
      : gr::sync_block("ToneDetector",
		       gr::io_signature::make(1, 1, DFTSize*sizeof(float)),
		       gr::io_signature::make(1, 1, sizeof(float))),
	DFTSize(DFTSize)
    {}

    /*
     * Our virtual destructor.
     */
    ToneDetector_impl::~ToneDetector_impl()
    {
    }
    
    int
    ToneDetector_impl::work (int noutput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const float *dftIn = (const float *) input_items[0];
      gr_complex *out = (gr_complex *) output_items[0];
      
      for(auto i = 0; i < noutput_items; i++) {
	const float* vect = dftIn + i*DFTSize;
	float meanPower;
	volk_32f_accumulator_s32f(&meanPower, vect, DFTSize);
	uint32_t maxInd;
	volk_32f_index_max_32u(&maxInd, vect, DFTSize);
	
	add_item_tag(0, nitems_written(0) + i, tagKey, pmt::from_float(fmod(float(maxInd)/DFTSize + 0.5f, 1.0f) - 0.5f));
	
	out[i] = vect[maxInd]/meanPower;
      }

      return noutput_items;
    }
    
  } /* namespace LibreLoRa */
} /* namespace gr */

