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
#include "slidingDFTMax_impl.h"

#include <volk/volk.h>
#include <stdexcept>

namespace gr {
  namespace LibreLoRa {

    slidingDFTMax::sptr
    slidingDFTMax::make(size_t DFTLength, size_t windowSize)
    {
      return gnuradio::get_initial_sptr
        (new slidingDFTMax_impl(DFTLength, windowSize));
    }


    /*
     * The private constructor
     */
    slidingDFTMax_impl::slidingDFTMax_impl(size_t DFTLength, size_t windowSize)
      : gr::sync_block("slidingDFTMax",
		       gr::io_signature::make(1, 1, sizeof(gr_complex)),
		       gr::io_signature::make2(1, 2, sizeof(float), sizeof(gr_complex))),
	length(DFTLength),
	windowSize(windowSize),//,
	exponents(generateExponents()),
	exponentsN(generateExponentsN())
	// beta(float(1 << SF)/(symbolSize*symbolSize)),
	// offset(beta)//,
	// step(1.0),
	// stepStep(std::polar<float>(1.0, -2*M_PI*beta))
    {

      if(windowSize > length)
	throw std::logic_error("slidingDFT: Error. Invalid window length.");
      
      set_history(windowSize);

      const int alignment_multiple = volk_get_alignment()/sizeof(gr_complex);
      set_alignment(std::max(1,alignment_multiple));

      DFT = (gr_complex *)volk_malloc(length*sizeof(gr_complex), volk_get_alignment());

      deltas = (gr_complex *)volk_malloc(length*sizeof(gr_complex), volk_get_alignment());
      
      // float alphai = 1.0;
      for (size_t i = 0; i < length; i++)
	DFT[i] = 1;

      // e0 = 1.0;
      // eN = std::polar<float>(1.0, -2*M_PI*beta*length*(length - 1)/2.0);

      // std::cout << "step: " << step << std::endl;
    }

    /*
     * Our virtual destructor.
     */
    slidingDFTMax_impl::~slidingDFTMax_impl()
    {
      free(const_cast<gr_complex*>(exponents));
      free(const_cast<gr_complex*>(exponentsN));
      free(DFT);
    }

    // void
    // slidingDFTMax_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    // {
    //   /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
    //   ninput_items_required[0] = noutput_items + length;
    // }
    
    int
    slidingDFTMax_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      float *freqOut = (float *) output_items[0];
      gr_complex *maxOut;
      if(output_items.size() > 1)
	maxOut = (gr_complex *) output_items[1];

      // Do <+signal processing+>
      
      for(size_t i = 0; i < noutput_items; i++) {
	
	volk_32fc_x2_multiply_32fc(DFT, exponents, DFT, length);
	
	// for(auto j = 0; j < length; j++){
	//   auto delta = - in[i]*exponentsN[j] + in[i + windowSize];
	//   DFT[j] += delta;
	// }

	volk_32fc_s32fc_multiply_32fc(deltas, exponentsN, -in[i], length);

	for(auto j = 0; j < length; j++)
	  deltas[j] += in[i + windowSize];
	
	volk_32fc_x2_add_32fc(DFT, DFT, deltas, length);

	uint32_t indMax;
	volk_32fc_index_max_32u(&indMax, DFT, length);
	
	freqOut[i] = std::remainder(indMax/float(length), 1.0);
	
	if(output_items.size() > 1)
	  maxOut[i] = DFT[indMax];
      }

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

    const gr_complex* slidingDFTMax_impl::generateExponents() {

#ifndef NDEBUG
      std::cout << "SlidingDFT: initializing exponents. length: " << length << ", alpha: " << alpha << std::endl;
#endif
      
      gr_complex* exponents = (gr_complex *)volk_malloc(length*sizeof(gr_complex), volk_get_alignment());

      for (size_t i = 0; i < length; i++)
	exponents[i] = alpha*std::polar<float>(1, (2.0*M_PI/length)*i);
      
      return exponents;
    }

    const gr_complex* slidingDFTMax_impl::generateExponentsN() {

#ifndef NDEBUG
      std::cout << "SlidingDFT: initializing exponentsN. length: " << length << ", windowSize: " << windowSize << ", alphaN: " << alphaN << std::endl;
#endif
      
      gr_complex* exponentsN = (gr_complex *)volk_malloc(length*sizeof(gr_complex), volk_get_alignment());
      
      for (size_t i = 0; i < length; i++)
	exponentsN[i] = alphaN*std::polar<float>(1, (2.0*M_PI/length)*i*windowSize);
	
      return exponentsN;
    }
    
  } /* namespace LibreLoRa */
} /* namespace gr */

