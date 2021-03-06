/*
 * Copyright 2021 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/***********************************************************************************/
/* This file is automatically generated using bindtool and can be manually edited  */
/* The following lines can be configured to regenerate this file during cmake      */
/* If manual edits are made, the following tags should be modified accordingly.    */
/* BINDTOOL_GEN_AUTOMATIC(0)                                                       */
/* BINDTOOL_USE_PYGCCXML(0)                                                        */
/* BINDTOOL_HEADER_FILE(normalizeCorrelation.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(f4e13d25400e3d590aa33eba3d251447)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <LibreLoRa/normalizeCorrelation.h>
// pydoc.h is automatically generated in the build directory
#include <normalizeCorrelation_pydoc.h>

void bind_normalizeCorrelation(py::module& m)
{

    using normalizeCorrelation    = gr::LibreLoRa::normalizeCorrelation;


    py::class_<normalizeCorrelation,
	       gr::sync_block,
               gr::block,
               gr::basic_block,
	       std::shared_ptr<normalizeCorrelation>>(m, "normalizeCorrelation", D(normalizeCorrelation))

        .def(py::init(&normalizeCorrelation::make),
           py::arg("correlationVectorSize"),
           D(normalizeCorrelation,make)
        )
        



        ;




}








