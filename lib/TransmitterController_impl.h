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

#ifndef INCLUDED_LIBRELORA_TRANSMITTERCONTROLLER_IMPL_H
#define INCLUDED_LIBRELORA_TRANSMITTERCONTROLLER_IMPL_H

#include <LibreLoRa/TransmitterController.h>

#include <LibreLoRa/calculateHeaderChecksum.h>
#include <LibreLoRa/utilities.h>

namespace gr {
  namespace LibreLoRa {

    class TransmitterController_impl : public TransmitterController
    {
     private:
      size_t SF;
      size_t SFCurrent;
      size_t CR;
      size_t CRCurrent;
      size_t payloadSize;
      bool CRCPresent;
      bool lowDataRate;

      constexpr static size_t CRCSize = 2;
      
      size_t nNibblesTotal;//Payload + CRC + padding
      
      enum controllerState {
	sendingHeader,
	sendingPayload
      };

      controllerState currentState;

      uint8_t headerNibbles[5];

      void calculateHeader() {
	headerNibbles[0] = (payloadSize >> 4) & 0x0f;
	headerNibbles[1] = payloadSize & 0x0f;

	headerNibbles[2] = (CRCPresent? 0x01 : 0x00) | (CR&0x0f << 1);

	auto checksum = calculateHeaderChecksum(*(uint32_t*) &headerNibbles);
	
	headerNibbles[3] = (checksum >> 4) & 0x0f;
	headerNibbles[4] = checksum & 0x0f;
      }

      constexpr const uint16_t calculateCRCFromNibbles(const uint8_t* nibbles) {
	constexpr uint16_t polynomial = 0x1021;
	uint16_t crc = 0x0000;
	for(size_t i = 0; i < 2*payloadSize; i++) {
	  crc <<= 4;
	  crc = polDivRem(crc^uint16_t(nibbles[i]), polynomial);
	}
	return crc;
      }

      pmt::pmt_t setSFPort;
      pmt::pmt_t setCRPort;
      pmt::pmt_t nSamplesPort;

      void setSFCurrent(size_t SFnew);
      void setCRCurrent(size_t CRnew);

      void sendParamsTag(bool isStartOfFrame = false);
      
     public:
      TransmitterController_impl(size_t SF, size_t CR, size_t payloadSize, bool CRCPresent, bool lowDataRate);
      ~TransmitterController_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
           gr_vector_int &ninput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);

    };

  } // namespace LibreLoRa
} // namespace gr

#endif /* INCLUDED_LIBRELORA_TRANSMITTERCONTROLLER_IMPL_H */

