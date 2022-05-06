/* -*- c++ -*- */
/*
 * Copyright 2022 Joao Pedro de O. Simas.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_LIBRELORA_CHIRPTRACKMSGTOFREQ_H
#define INCLUDED_LIBRELORA_CHIRPTRACKMSGTOFREQ_H

#include <LibreLoRa/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace LibreLoRa {

    /*!
     * \brief <+description of block+>
     * \ingroup LibreLoRa
     *
     */
    class LIBRELORA_API ChirpTrackMsgToFreq : virtual public gr::sync_block
    {
     public:
      typedef std::shared_ptr<ChirpTrackMsgToFreq> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of LibreLoRa::ChirpTrackMsgToFreq.
       *
       * To avoid accidental use of raw pointers, LibreLoRa::ChirpTrackMsgToFreq's
       * constructor is in a private implementation
       * class. LibreLoRa::ChirpTrackMsgToFreq::make is the public interface for
       * creating new instances.
       */
      static sptr make();
    };

  } // namespace LibreLoRa
} // namespace gr

#endif /* INCLUDED_LIBRELORA_CHIRPTRACKMSGTOFREQ_H */

