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
#include "slidingDFT_impl.h"

#include <volk/volk.h>

#include <iostream>

namespace gr {
  namespace LibreLoRa {

    slidingDFT::sptr
    slidingDFT::make(size_t DFTLength, size_t windowSize)
    {
      return gnuradio::make_block_sptr<slidingDFT_impl>(
        DFTLength, windowSize);
    }


    /*
     * The private constructor
     */
    slidingDFT_impl::slidingDFT_impl(size_t DFTLength, size_t windowSize)
      : gr::sync_block("slidingDFT",
		       gr::io_signature::make(1, 1, sizeof(gr_complex)),
		       gr::io_signature::make(1, 1, DFTLength*sizeof(gr_complex))),
	length(DFTLength),
	windowSize(windowSize),
	//beta(float(1 << SF)*2*M_PI/(symbolSize*symbolSize)),
	// beta(beta),
	alphaN(std::pow(alpha, windowSize)),
	exponents(generateExponents()),
	exponentsN(generateExponentsN())
	// step(std::polar<float>(1.0, beta)),
	// stepN(1.0),
	// index(0)
    {

      // std::cout << "WIENERSHLIEDEN ENGAGED. SCHLIDENESS = " << beta << std::endl;
      // std::cout << "COMPLEX SCHLIDENESS = " << step << std::endl;

      std::cout << "SlidingDFT: alpha = " << alpha << std::endl;
      std::cout << "SlidingDFT: alphaN = " << alphaN << std::endl;
      set_history(length);

      const int alignment_multiple = volk_get_alignment()/sizeof(gr_complex);
      set_alignment(std::max(1,alignment_multiple));

      DFT = (gr_complex *)volk_malloc(length*sizeof(gr_complex), volk_get_alignment());

      deltas = (gr_complex *)volk_malloc(length*sizeof(gr_complex), volk_get_alignment());
      
      for (size_t i = 0; i < length; i++)
	DFT[i] = 1;
      
      // a = 1.0;
    }

    /*
     * Our virtual destructor.
     */
    slidingDFT_impl::~slidingDFT_impl()
    {
      free(const_cast<gr_complex*>(exponents));
      free(const_cast<gr_complex*>(exponentsN));
      free(DFT);
    }

    // void
    // slidingDFT_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    // {
    //   /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
    //   ninput_items_required[0] = noutput_items + length;
    // }
    
    int
    slidingDFT_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      gr_complex *out = (gr_complex *) output_items[0];

      // Do <+signal processing+>
      
      for(auto i = 0; i < noutput_items; i++) {
	// for(size_t j = 0; j < length; j++)	
	// a *= std::polar<float>(1.0, std::arg(exponents[0]));
	volk_32fc_x2_multiply_32fc(DFT, exponents, DFT, length);

	// const auto delta = in[i + length]*exponents[0];
	// const auto delta = - a*in[i]*alphaN + in[i + length];

	volk_32fc_s32fc_multiply_32fc(deltas, exponentsN, -in[i], length);

	// volk_32fc_s32fc_add_32fc(deltas, deltas, in[i + windowSize], length);

	for(auto j = 0; j < length; j++)
	  deltas[j] += in[i + windowSize];
	
	volk_32fc_x2_add_32fc(DFT, DFT, deltas, length);
	  
	
	// for(auto j = 0; j < length; j++) {
	//    const auto delta = - in[i]*exponentsN[j] + in[i + windowSize];
	//    DFT[j] += delta;
	// }

	 // for(auto j = 0; j < length; j++)
	 //   out[j + length*i] = DFT[j];
	 memcpy(out + length*i, DFT, length*sizeof(gr_complex));
	 
	 // volk_32fc_s32fc_multiply_32fc(exponents, exponents, step, length);
      }

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

    const gr_complex* slidingDFT_impl::generateExponents() {

#ifndef NDEBUG
      std::cout << "SlidingDFT: initializing exponents. length: " << length << ", alpha: " << alpha << std::endl;
#endif
      
      gr_complex* exponents = (gr_complex *)volk_malloc(length*sizeof(gr_complex), volk_get_alignment());

      for (size_t i = 0; i < length; i++)
	exponents[i] = alpha*std::polar<float>(1, (2.0*M_PI/length)*i);
      
      return exponents;
    }

    const gr_complex* slidingDFT_impl::generateExponentsN() {

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

