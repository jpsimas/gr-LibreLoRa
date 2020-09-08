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
#include "FFTMax_impl.h"

namespace gr {
  namespace LibreLoRa {

    FFTMax::sptr
    FFTMax::make(size_t DFTSize, std::vector<gr_complex> window)
    {
      return gnuradio::get_initial_sptr
        (new FFTMax_impl(DFTSize, window));
    }


    /*
     * The private constructor
     */
    FFTMax_impl::FFTMax_impl(size_t DFTSize, std::vector<gr_complex> window)
      : gr::sync_block("FFTMax",
		       gr::io_signature::make(1, 1, DFTSize*sizeof(gr_complex)),
		       gr::io_signature::make(1, 1, sizeof(uint32_t))),
	size(DFTSize),
	windowSize(window.size()),
	enabled(true)
    {
      auto align =  volk_get_alignment();
      fftIn = (gr_complex*) volk_malloc(sizeof(gr_complex) * size, align);
      fftOut = (gr_complex*) volk_malloc(sizeof(gr_complex) * size, align);

      for(auto i = 0; i < size; i++) {
	fftIn[i] = 0;
	fftOut[i] = 0;
      }
      
      fftPlan = fft_create_plan(size, fftIn, fftOut, LIQUID_FFT_FORWARD, 0);

      fftWindow = (gr_complex*) volk_malloc(sizeof(gr_complex) * size, align);
      memcpy(fftWindow, window.data(), windowSize*sizeof(gr_complex));

      //message ports
      this->message_port_register_in(pmt::mp("enable"));
      this->set_msg_handler(pmt::mp("enable"),
			    [this](pmt::pmt_t msg) {
			      enabled = true;
#ifndef NDEBUG
			      std::cout << "FFTMax: enabled." << std::endl;
#endif
			    });

      this->message_port_register_in(pmt::mp("reset"));
      this->set_msg_handler(pmt::mp("reset"),
			    [this](pmt::pmt_t msg) {
			      enabled = false;
#ifndef NDEBUG
			      std::cout << "FFTMax: reset." << std::endl;
#endif
			    });
    }

    /*
     * Our virtual destructor.
     */
    FFTMax_impl::~FFTMax_impl()
    {
      fft_destroy_plan(fftPlan);
      free(fftIn);
      free(fftOut);
    }

    int
    FFTMax_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      uint32_t *out = (uint32_t *) output_items[0];

      for(auto i = 0; i < noutput_items; i++) {
	if(enabled) {
	  volk_32fc_x2_multiply_32fc(fftIn, in + i*size, fftWindow, windowSize);
	  // memcpy(fftIn, in + i*size, size*sizeof(gr_complex));
	  fft_execute(fftPlan);
	  volk_32fc_index_max_32u(out + i, fftOut, size);
	} else
	  out[i] = 0;
      }
      
      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace LibreLoRa */
} /* namespace gr */

