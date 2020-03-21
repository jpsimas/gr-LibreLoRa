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
#include <gnuradio/blocks/add_blk.h>
#include "hier_test2_impl.h"

namespace gr {
  namespace LibreLoRa {

    hier_test2::sptr
    hier_test2::make(size_t SF, size_t OSF, size_t nUpchirps, uint8_t syncWordNumber, float minCorr, float corrStop)
    {
      return gnuradio::get_initial_sptr
        (new hier_test2_impl(SF, OSF, nUpchirps, syncWordNumber, minCorr, corrStop));
    }


    /*
     * The private constructor
     */
    hier_test2_impl::hier_test2_impl(size_t SF, size_t OSF, size_t nUpchirps, uint8_t syncWordNumber, float minCorr, float corrStop)
      : gr::hier_block2("hier_test2",
			gr::io_signature::make(1, 1, sizeof(float)),
			gr::io_signature::make(2, 2, sizeof(float)))
    {
      

    }

    /*
     * Our virtual destructor.
     */
    hier_test2_impl::~hier_test2_impl()
    {
    }


  } /* namespace LibreLoRa */
} /* namespace gr */

