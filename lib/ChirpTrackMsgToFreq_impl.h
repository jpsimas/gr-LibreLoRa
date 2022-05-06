/* -*- c++ -*- */
/*
 * Copyright 2022 Joao Pedro de O. Simas.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_LIBRELORA_CHIRPTRACKMSGTOFREQ_IMPL_H
#define INCLUDED_LIBRELORA_CHIRPTRACKMSGTOFREQ_IMPL_H

#include <LibreLoRa/ChirpTrackMsgToFreq.h>

namespace gr {
  namespace LibreLoRa {

    class ChirpTrackMsgToFreq_impl : public ChirpTrackMsgToFreq
    {
     private:
      void readMessage(pmt::pmt_t msg);

      pmt::pmt_t freqPort;
     public:
      ChirpTrackMsgToFreq_impl();
      ~ChirpTrackMsgToFreq_impl();

      // Where all the action really happens
      int work(
              int noutput_items,
              gr_vector_const_void_star &input_items,
              gr_vector_void_star &output_items
      );
    };

  } // namespace LibreLoRa
} // namespace gr

#endif /* INCLUDED_LIBRELORA_CHIRPTRACKMSGTOFREQ_IMPL_H */

