/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <pybind11/pybind11.h>

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <numpy/arrayobject.h>

namespace py = pybind11;

// Headers for binding functions
/**************************************/
/* The following comment block is used for
/* gr_modtool to insert function prototypes
/* Please do not delete
/**************************************/
// BINDING_FUNCTION_PROTOTYPES(
void bind_frequencyTracker(py::module&);
void bind_symbolCorrelator(py::module&);
void bind_hier_test2(py::module&);
void bind_preambleDetector(py::module&);
void bind_normalizeCorrelation(py::module&);
void bind_correlationSync(py::module&);
void bind_symbolDemod(py::module&);
void bind_grayEncode(py::module&);
void bind_deinterleave(py::module&);
void bind_decode(py::module&);
void bind_chirpSelect(py::module&);
void bind_chirpSource(py::module&);
void bind_streamToHistoryVector(py::module&);
void bind_slidingDFT(py::module&);
void bind_ArgMaxAbs(py::module&);
void bind_readHeader(py::module&);
void bind_Correlation(py::module&);
void bind_convolution(py::module&);
void bind_correlationSyncDemod(py::module&);
void bind_receiverController(py::module&);
void bind_randomize(py::module&);
void bind_NibblesToBytes(py::module&);
void bind_CRC16(py::module&);
void bind_slidingDFTMax(py::module&);
void bind_frequencyTrackerN(py::module&);
void bind_symbolDemodNew(py::module&);
void bind_PowerDetector(py::module&);
void bind_frequencyTrackerNLMS(py::module&);
void bind_frequencyTrackerNMode(py::module&);
void bind_ToneDetector(py::module&);
void bind_DetectionCount(py::module&);
void bind_BinSearchFFT(py::module&);
void bind_FFTMax(py::module&);
void bind_SelectMax(py::module&);
void bind_Remainder(py::module&);
void bind_FrequencyMod(py::module&);
void bind_SymbolMod(py::module&);
void bind_GrayDecode(py::module&);
void bind_Encode(py::module&);
void bind_Interleave(py::module&);
void bind_BytesToNibbles(py::module&);
void bind_TransmitterController(py::module&);
void bind_AppendPrefix(py::module&);
void bind_AppendSilence(py::module&);
void bind_LoRaPDUHeaderReader(py::module&);
void bind_LoRaPDUHeaderGenerator(py::module&);
void bind_getSymbol(py::module&);
// ) END BINDING_FUNCTION_PROTOTYPES


// We need this hack because import_array() returns NULL
// for newer Python versions.
// This function is also necessary because it ensures access to the C API
// and removes a warning.
void* init_numpy()
{
    import_array();
    return NULL;
}

PYBIND11_MODULE(LibreLoRa_python, m)
{
    // Initialize the numpy C API
    // (otherwise we will see segmentation faults)
    init_numpy();

    // Allow access to base block methods
    py::module::import("gnuradio.gr");

    /**************************************/
    /* The following comment block is used for
    /* gr_modtool to insert binding function calls
    /* Please do not delete
    /**************************************/
    // BINDING_FUNCTION_CALLS(
    bind_frequencyTracker(m);
    bind_symbolCorrelator(m);
    bind_hier_test2(m);
    bind_preambleDetector(m);
    bind_normalizeCorrelation(m);
    bind_correlationSync(m);
    bind_symbolDemod(m);
    bind_grayEncode(m);
    bind_deinterleave(m);
    bind_decode(m);
    bind_chirpSelect(m);
    bind_chirpSource(m);
    bind_streamToHistoryVector(m);
    bind_slidingDFT(m);
    bind_ArgMaxAbs(m);
    bind_readHeader(m);
    bind_Correlation(m);
    bind_convolution(m);
    bind_correlationSyncDemod(m);
    bind_receiverController(m);
    bind_randomize(m);
    bind_NibblesToBytes(m);
    bind_CRC16(m);
    bind_slidingDFTMax(m);
    bind_frequencyTrackerN(m);
    bind_symbolDemodNew(m);
    bind_PowerDetector(m);
    bind_frequencyTrackerNLMS(m);
    bind_frequencyTrackerNMode(m);
    bind_ToneDetector(m);
    bind_DetectionCount(m);
    bind_BinSearchFFT(m);
    bind_FFTMax(m);
    bind_SelectMax(m);
    bind_Remainder(m);
    bind_FrequencyMod(m);
    bind_SymbolMod(m);
    bind_GrayDecode(m);
    bind_Encode(m);
    bind_Interleave(m);
    bind_BytesToNibbles(m);
    bind_TransmitterController(m);
    bind_AppendPrefix(m);
    bind_AppendSilence(m);
    bind_LoRaPDUHeaderReader(m);
    bind_LoRaPDUHeaderGenerator(m);
    bind_getSymbol(m);
    // ) END BINDING_FUNCTION_CALLS
}
