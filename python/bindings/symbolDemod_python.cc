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
/* BINDTOOL_HEADER_FILE(symbolDemod.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(df07a2622013153249e6c8392405db88)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <LibreLoRa/symbolDemod.h>
// pydoc.h is automatically generated in the build directory
#include <symbolDemod_pydoc.h>

template<typename T>
void bind_symbolDemod_template(py::module& m, const char* classname)
{

    using symbolDemod    = gr::LibreLoRa::symbolDemod<T>;


    py::class_<symbolDemod,
               gr::block,
               gr::basic_block,
	       std::shared_ptr<symbolDemod>>(m, classname, D(symbolDemod))

        .def(py::init(&symbolDemod::make),
           py::arg("SF"),
           py::arg("symbolSize"),
           py::arg("implicit"),
           py::arg("symbolDemod"),
           D(symbolDemod,make)
        )
        



        ;




}

void bind_symbolDemod(py::module& m)
{
    bind_symbolDemod_template<float>(m, "symbolDemod_ff");
    bind_symbolDemod_template<gr_complex>(m, "symbolDemod_cc");
}






