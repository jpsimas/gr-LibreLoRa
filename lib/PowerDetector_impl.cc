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
    PowerDetector::make(float threshold, float timeoutSeconds, size_t decimation, pmt::pmt_t message)
    {
      return gnuradio::get_initial_sptr
        (new PowerDetector_impl(threshold, timeoutSeconds, decimation, message));
    }


    /*
     * The private constructor
     */
    PowerDetector_impl::PowerDetector_impl(float threshold, float timeoutSeconds, size_t decimation, pmt::pmt_t message)
      : gr::block("PowerDetector",
		  gr::io_signature::make2(2, 2, sizeof(gr_complex), sizeof(float)),
		  gr::io_signature::make(1, 1, sizeof(gr_complex))),
	threshold(threshold), 
	state(detection),
	timeout(timeoutSeconds*CLOCKS_PER_SEC),
	decimation(decimation),
	message(message)
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

      detectOutPort = pmt::string_to_symbol("detectOut");
      message_port_register_out(detectOutPort);
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
      ninput_items_required[0] = ((noutput_items + decimation - 1)/decimation)*decimation;
      ninput_items_required[1] = (noutput_items + decimation - 1)/decimation;
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
	for(i = 0; i < (noutput_items + decimation - 1)/decimation; i++) {
	  if(powerIn[i] > threshold) {
	    state = started;
	    time = clock();

	    std::vector<gr::tag_t> tags;
	    auto nr =  nitems_read(1);
	    get_tags_in_range(tags, 1, nr + i, nr + i + 1);
	    if(tags.size() != 0)
	      message_port_pub(detectOutPort, pmt::make_tuple(message, tags[0].value));
	    else
	      message_port_pub(detectOutPort, message);
#ifndef NDEBUG
	    std::cout << "PowerDetector: started" << std::endl;
#endif
	    break;
	  }
	}

	consume(0, i*decimation);
	consume(1, i);
	
	return 0;
	
      case started:
	for(auto i = 0; i < ((noutput_items + decimation - 1)/decimation)*decimation; i++) {
	  out[i] = in[i];
	  if(clock() > time + timeout) {
	    state = waiting;
	  }
	}
	
	consume(0, ((noutput_items + decimation - 1)/decimation)*decimation);
	consume(1, (noutput_items + decimation - 1)/decimation);
	return ((noutput_items + decimation - 1)/decimation)*decimation;
      case waiting:
	for(i = 0; i < ((noutput_items + decimation - 1)/decimation)*decimation; i++) {
	  if(powerIn[i] < threshold) {
	    state = detection;
#ifndef NDEBUG
	    std::cout << "PowerDetector: stopped (timeout)" << std::endl;
#endif
	    break;
	  }
	}
	
	consume(0, i*decimation);
	consume(1, i);
	return 0;
      }
      return 0;
    }

    void
    PowerDetector_impl::reset() {
      state = waiting;
    }
    
  } /* namespace LibreLoRa */
} /* namespace gr */

