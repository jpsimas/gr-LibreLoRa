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
    slidingDFT::make(size_t DFTLength, float beta)
    {
      return gnuradio::get_initial_sptr
        (new slidingDFT_impl(DFTLength, beta));
    }


    /*
     * The private constructor
     */
    slidingDFT_impl::slidingDFT_impl(size_t DFTLength, float beta)
      : gr::sync_block("slidingDFT",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
		       gr::io_signature::make(1, 1, DFTLength*sizeof(gr_complex))),
	length(DFTLength),
	alpha(0.8),
	alphaN(pow(alpha, length)),
	//beta(float(1 << SF)*2*M_PI/(symbolSize*symbolSize)),
	beta(beta),
	step(std::polar<float>(1.0, beta)),
	stepN(1.0),
	index(0){

      std::cout << "WIENERSHLIEDEN ENGAGED. SCHLIDENESS = " << beta << std::endl;
      std::cout << "COMPLEX SCHLIDENESS = " << step << std::endl;

      std::cout << "ANTI-SCHLIDENESS = " << alpha << std::endl;
      std::cout << "N-ANTI-SCHLIDENESS = " << alphaN << std::endl;
      set_history(length);

      const int alignment_multiple = volk_get_alignment()/sizeof(gr_complex);
      set_alignment(std::max(1,alignment_multiple));

      exponents = (gr_complex *)volk_malloc((length + 1)*sizeof(gr_complex), volk_get_alignment());

      DFT = (gr_complex *)volk_malloc(length*sizeof(gr_complex), volk_get_alignment());

      for (auto i = 0; i < length; i++) 
	DFT[i] = 0.0;

      for (auto i = 0; i < length + 1; i++) {
	exponents[i] = std::polar<float>(alpha, (2.0*M_PI/length)*i);
      }

      a = 1.0;
    }

    /*
     * Our virtual destructor.
     */
    slidingDFT_impl::~slidingDFT_impl()
    {
      free(exponents);
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
	a *= std::polar<float>(1.0, std::arg(exponents[0]));
	volk_32fc_x2_multiply_32fc(DFT, exponents, DFT, length);

	// const auto delta = in[i + length]*exponents[0];
	const auto delta = /*- a*in[i]*alphaN*/ + in[i + length];
	// const auto delta = - in[i] + in[i + length]*alphaN;

	 for(auto j = 0; j < length; j++)
	   //DFT[j] = DFT[j] + in[i + length];
	   DFT[j] += delta;

	 // for(auto j = 0; j < length; j++)
	 //   out[j + length*i] = DFT[j];
	 memcpy(out + length*i, DFT, length*sizeof(gr_complex));
	 
	 volk_32fc_s32fc_multiply_32fc(exponents, exponents, step, length);
      }

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace LibreLoRa */
} /* namespace gr */

