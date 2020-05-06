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

#ifndef INCLUDED_LIBRELORA_RECEIVERCONTROLLER_IMPL_H
#define INCLUDED_LIBRELORA_RECEIVERCONTROLLER_IMPL_H

#include <LibreLoRa/receiverController.h>

namespace gr {
  namespace LibreLoRa {

    class receiverController_impl : public receiverController
    {
     private:
      correlationSync::sptr synchronizer;
      // symbolDemod::sptr demodulator;
      // grayEncode::sptr grayEncoder;
      // deinterleave::sptr deinterleaver;
      // decode::sptr decoder;
      // randomize::sptr randomizer;

      enum controllerState {waitingForSync,
			    readingHeader,
			    sendingPayload};

      controllerState currentState;
      // size_t symbolSize;
      const size_t SF;
      size_t SFcurrent;

      uint8_t payloadLength;
      uint8_t CR;
      uint8_t payloadCRCPresent;
      bool headerCheckSumValid;

      constexpr static size_t payloadCRCSize = 2;
      
      size_t payloadNibblesToRead;
      size_t extraNibblesToConsume;

      pmt::pmt_t lfsrStatePort;
      pmt::pmt_t setSFPort;
      pmt::pmt_t setCRPort;
      pmt::pmt_t synchronizerSetNPort;
      pmt::pmt_t synchronizerResetPort;
      
      void startRx();
      void stopRx();
      void setSFcurrent(size_t SF);
      void setCR(size_t CR);

      void readHeader(const uint8_t* nibbles, uint8_t* dataOut);
     public:
      receiverController_impl(size_t SF, correlationSync::sptr synchronizer/*, symbolDemod::sptr demodulator, grayEncode::sptr grayEncoder, deinterleave::sptr deinterleaver, decode::sptr decoder, randomize::sptr randomizer*/);
      ~receiverController_impl();

      void forecast (int noutput_items, gr_vector_int &ninput_items_required);
      // Where all the action really happens
      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items
      );
    };

  } // namespace LibreLoRa
} // namespace gr

#endif /* INCLUDED_LIBRELORA_RECEIVERCONTROLLER_IMPL_H */

