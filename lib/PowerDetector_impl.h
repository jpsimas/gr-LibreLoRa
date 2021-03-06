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

#ifndef INCLUDED_LIBRELORA_POWERDETECTOR_IMPL_H
#define INCLUDED_LIBRELORA_POWERDETECTOR_IMPL_H

#include <LibreLoRa/PowerDetector.h>
// #include <time.h>

namespace gr {
  namespace LibreLoRa {

    class PowerDetector_impl : public PowerDetector
    {
     private:
      const float threshold;
      const size_t decimation;
      enum state_t {detection, started, waiting};
      state_t state;
      //clock_t timeout;
      //clock_t time;

      const size_t maxSamplesToRead;
      size_t samplesToRead;
      
      pmt::pmt_t detectOutPort;
      pmt::pmt_t message;
     public:
      PowerDetector_impl(float sampRate, float threshold, float timeoutSeconds, size_t decimation, pmt::pmt_t message);
      ~PowerDetector_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
           gr_vector_int &ninput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);

      void reset();

    };

  } // namespace LibreLoRa
} // namespace gr

#endif /* INCLUDED_LIBRELORA_POWERDETECTOR_IMPL_H */

