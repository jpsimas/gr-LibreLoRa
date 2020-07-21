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

#ifndef INCLUDED_LIBRELORA_CORRELATIONSYNC_IMPL_H
#define INCLUDED_LIBRELORA_CORRELATIONSYNC_IMPL_H

#include <LibreLoRa/correlationSync.h>

namespace gr {
  namespace LibreLoRa {

    template<typename T>
    class correlationSync_impl : public correlationSync<T>
    {
     private:
      const float corrMin;
      const float corrStop;
      const size_t symbolSize;
      const size_t preambleSize;
      const size_t SF;

      const uint8_t syncWordNum1;
      const uint8_t syncWordNum2;
      
      T corrMax;

      T offset;
      inline void estimateOffset(const T *preamble);
      
      bool syncd;
      // enum syncState {initial, foundFirstPt, syncd};
      // syncState currState;
      // float corrMax;
      
      //constexpr static int fixedModeDisabled = -1;
      bool fixedMode;
      
      int nOutputItemsToProduce;
      bool deSyncAfterDone;

      bool preambleConsumed;
      size_t preambleSamplesToConsume;
      
      pmt::pmt_t syncPort;

      inline float norm(T x);

      //for estimateOffset
      std::vector<T> syncWordExpected;
      std::vector<size_t> detectionCount;
     public:
      correlationSync_impl(float corrMin, float corrStop, size_t symbolSize, size_t preambleSize, size_t SF, uint16_t syncWordNumber);
      ~correlationSync_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);

      void reset();
      void setNOutputItemsToProduce(int noutput_items) {nOutputItemsToProduce = noutput_items;};

      constexpr bool fixedModeEnabled() {
	return fixedMode;
	//return nOutputItemsToProduce != fixedModeDisabled;};
      };


      // int fixed_rate_ninput_to_noutput(int ninput_items)
      // {
      // 	const size_t n = ((fixedModeEnabled() && (nOutputItemsToProduce < ninput_items))? nOutputItemsToProduce : ninput_items);
      // 	return syncd? (preambleConsumed? n : 0) : 0;
      // }

      // int fixed_rate_noutput_to_ninput(int noutput_items)
      // {
      // 	const size_t n = ((fixedModeEnabled() && (nOutputItemsToProduce < noutput_items))? nOutputItemsToProduce : noutput_items);
      // 	return (syncd? (preambleConsumed? (symbolSize*n) : std::min((size_t(1 << 14) - 1), preambleSamplesToConsume)) : 2*symbolSize);
      // }
    };
  } // namespace LibreLoRa
} // namespace gr

#endif /* INCLUDED_LIBRELORA_CORRELATIONSYNC_IMPL_H */

