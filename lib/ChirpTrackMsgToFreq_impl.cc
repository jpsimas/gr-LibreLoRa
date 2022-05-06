/* -*- c++ -*- */
/*
 * Copyright 2022 Joao Pedro de O. Simas.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/io_signature.h>
#include "ChirpTrackMsgToFreq_impl.h"

namespace gr {
  namespace LibreLoRa {

    ChirpTrackMsgToFreq::sptr
    ChirpTrackMsgToFreq::make()
    {
      return gnuradio::make_block_sptr<ChirpTrackMsgToFreq_impl>(
        );
    }


    /*
     * The private constructor
     */
    ChirpTrackMsgToFreq_impl::ChirpTrackMsgToFreq_impl()
      : gr::sync_block("ChirpTrackMsgToFreq",
              gr::io_signature::make(0 /* min inputs */, 0 /* max inputs */, 0),
              gr::io_signature::make(0 /* min outputs */, 0 /*max outputs */, 0)) {

      message_port_register_in(pmt::mp("detectionMessage"));
      set_msg_handler(pmt::mp("detectionMessage"), [this](pmt::pmt_t msg) {readMessage(msg);});

      freqPort = pmt::string_to_symbol("frequency");
      message_port_register_out(freqPort);

    }

    /*
     * Our virtual destructor.
     */
    ChirpTrackMsgToFreq_impl::~ChirpTrackMsgToFreq_impl()
    {
    }

    int
    ChirpTrackMsgToFreq_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      // Take detection message from ChirpTrack and output frequency
      // to be used with frequency source.

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

    void
    ChirpTrackMsgToFreq_impl::readMessage(pmt::pmt_t msg) {
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
      
      if(!pmt::tuple_ref(header, 1)->is_number())
	return;
      float BWRead = pmt::to_float(pmt::tuple_ref(header, 1));

      if(!pmt::tuple_ref(header, 2)->is_number())
	return;
      float sampRateRead = pmt::to_float(pmt::tuple_ref(header, 2));

      auto offsetMsg = pmt::tuple_ref(msg, 1);
      if(!offsetMsg->is_number())
	return;
      float offsetRead = pmt::to_float(offsetMsg);

      float freq = offsetRead*sampRateRead;
      
      message_port_pub(freqPort, pmt::from_float(freq));
    }
    
  } /* namespace LibreLoRa */
} /* namespace gr */

