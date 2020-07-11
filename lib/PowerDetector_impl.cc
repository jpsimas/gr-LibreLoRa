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
#include "PowerDetector_impl.h"

namespace gr {
  namespace LibreLoRa {

    PowerDetector::sptr
    PowerDetector::make(float threshold)
    {
      return gnuradio::get_initial_sptr
        (new PowerDetector_impl(threshold));
    }


    /*
     * The private constructor
     */
    PowerDetector_impl::PowerDetector_impl(float threshold)
      : gr::block("PowerDetector",
		  gr::io_signature::make2(2, 2, sizeof(gr_complex), sizeof(float)),
		  gr::io_signature::make(1, 1, sizeof(gr_complex))),
	threshold(threshold), 
	state(detection)
    {
      this->message_port_register_in(pmt::mp("reset"));
      this->set_msg_handler(pmt::mp("reset"),
			    [this](pmt::pmt_t msg) {
			      if(state == started) {
#ifndef NDEBUG
				std::cout << "PowerDetector: reset" << std::endl;
#endif
				reset();
			      }
			    });
    }

    /*
     * Our virtual destructor.
     */
    PowerDetector_impl::~PowerDetector_impl()
    {
    }

    void
    PowerDetector_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      ninput_items_required[0] = ninput_items_required[1] = noutput_items;
    }

    int
    PowerDetector_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      const float *powerIn = (const float *) input_items[1];
      gr_complex *out = (gr_complex *) output_items[0];

      size_t i;
      switch(state) {
      case detection:
	for(i = 0; i < noutput_items; i++) {
	  if(powerIn[i] > threshold) {
	    state = started;
#ifndef NDEBUG
	    std::cout << "PowerDetector: started" << std::endl;
#endif
	    break;
	  }
	}

	consume_each(i);
	return 0;
	
      case started:
	for(auto i = 0; i < noutput_items; i++)
	  out[i] = in[i];
	
	consume_each (noutput_items);
	return noutput_items;
      case waiting:
	for(i = 0; i < noutput_items; i++) {
	  if(powerIn[i] < threshold) {
	    state = detection;
	    break;
	  }
	}

	consume_each(i);
	return 0;
      }
    }

    void
    PowerDetector_impl::reset() {
      state = waiting;
    }
    
  } /* namespace LibreLoRa */
} /* namespace gr */

