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
#include "DetectionCount_impl.h"

namespace gr {
  namespace LibreLoRa {

    DetectionCount::sptr
    DetectionCount::make(float BW, size_t nChannels, float channelWidth)
    {
      return gnuradio::get_initial_sptr
        (new DetectionCount_impl(BW, nChannels, channelWidth));
    }


    /*
     * The private constructor
     */
    DetectionCount_impl::DetectionCount_impl(float BW, size_t nChannels, float channelWidth)
      : gr::sync_block("DetectionCount",
				gr::io_signature::make(0, 0, 0),
		       gr::io_signature::make(0, 0, 0)),
	BW(BW),
	nChannels(nChannels),
	channelWidth(channelWidth),//make this a constructor parameter
	count((SFMax - SFMin + 1)*nChannels){
      message_port_register_in(pmt::mp("detectionMessage"));
      set_msg_handler(pmt::mp("detectionMessage"), [this](pmt::pmt_t msg) {readMessage(msg);});

      countPort = pmt::string_to_symbol("count");
      message_port_register_out(countPort);

      for(auto& x : count)
	x = 0;
    }

    /*
     * Our virtual destructor.
     */
    DetectionCount_impl::~DetectionCount_impl()
    {
    }

    int
    DetectionCount_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      int *out = (int *) output_items[0];

      for(auto i = 0; i < noutput_items; i++)
	for(auto j = 0; j < count.size(); j++)
	  out[j + i*nChannels] = count[j];
      
      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

    void
    DetectionCount_impl::readMessage(pmt::pmt_t msg) {
      if(!msg->is_tuple())
	return;

      if(pmt::length(msg) < 2)
	return;
      
      pmt::pmt_t header = pmt::tuple_ref(msg, 0);
      if(!header->is_tuple())
	return;

      if(pmt::length(header) < 3)
	return;

      if(!pmt::tuple_ref(header, 0)->is_integer())
	return;
      size_t SFRead = pmt::to_long(pmt::tuple_ref(header, 0));
      if(SFRead > SFMax || SFRead < SFMin)
	return;
      
      if(!pmt::tuple_ref(header, 1)->is_number())
	return;
      float BWRead = pmt::to_float(pmt::tuple_ref(header, 1));
      if(BWRead != BW)
	return;

      if(!pmt::tuple_ref(header, 2)->is_number())
	return;
      float sampRateRead = pmt::to_float(pmt::tuple_ref(header, 2));

      auto offsetMsg = pmt::tuple_ref(msg, 1);
      if(!offsetMsg->is_number())
	return;
      float offset = pmt::to_float(offsetMsg);
      int channel = int(std::round(nChannels/2 + offset*sampRateRead/channelWidth));
      if(channel < 0 || channel > (nChannels - 1))
	return;

      count[channel + nChannels*(SFRead - SFMin)]++;

      pmt::pmt_t countPmt = pmt::init_u32vector(count.size(), count);
      
      message_port_pub(countPort, countPmt);
#ifndef NDEBUG
      std::cout << "DetectionCount (BW = " << BW << ") got valid detection. SF = " << SFRead << ", channel = " << channel << std::endl;
#endif
    }
    
  } /* namespace LibreLoRa */
} /* namespace gr */

