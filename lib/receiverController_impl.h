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
      symbolDemod::sptr demodulator;
      grayEncode::sptr grayEncoder;
      deinterleave::sptr deinterleaver;
      decode::sptr decoder;
<<<<<<< HEAD
      randomize::sptr randomizer;
=======
>>>>>>> 062e63eb739d92a3a44626a7e493c62fa54e1060

      enum controllerState {waitingForSync,
			    decodingHeader,
			    decodingPayload};

      controllerState currentState;
      // Nothing to declare in this block.
      size_t symbolSize;
      const size_t SF;
      size_t SFcurrent;

      uint8_t payloadLength;
      uint8_t CR;
      uint8_t payloadCRCPresent;
      bool headerCheckSumValid;

      size_t nibblesToRead;
      
      void startRx();
      void stopRx();
      void setSFcurrent(size_t SF);
      void setCR(size_t CR);

      void readHeader(const uint8_t* nibbles, uint8_t* dataOut);
     public:
      receiverController_impl(size_t SF, size_t symbolSize, correlationSync::sptr synchronizer, symbolDemod::sptr demodulator, grayEncode::sptr grayEncoder, deinterleave::sptr deinterleaver, decode::sptr decoder, randomize::sptr randomizer);
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

