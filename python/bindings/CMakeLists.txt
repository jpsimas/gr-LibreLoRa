# Copyright 2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Check if there is C++ code at all
########################################################################
if(NOT LibreLoRa_sources)
    MESSAGE(STATUS "No C++ sources... skipping python bindings")
    return()
endif(NOT LibreLoRa_sources)

########################################################################
# Check for pygccxml
########################################################################
GR_PYTHON_CHECK_MODULE_RAW(
    "pygccxml"
    "import pygccxml"
    PYGCCXML_FOUND
    )

include(GrPybind)

########################################################################
# Python Bindings
########################################################################

list(APPEND LibreLoRa_python_files
    AppendPrefix_python.cc
    AppendSilence_python.cc
    ArgMaxAbs_python.cc
    BinSearchFFT_python.cc
    BytesToNibbles_python.cc
    chirpSelect_python.cc
    chirpSource_python.cc
    convolution_python.cc
    Correlation_python.cc
    correlationSyncDemod_python.cc
    correlationSync_python.cc
    CRC16_python.cc
    decode_python.cc
    deinterleave_python.cc
    DetectionCount_python.cc
    Encode_python.cc
    FFTMax_python.cc
    FrequencyMod_python.cc
    frequencyTrackerNLMS_python.cc
    frequencyTrackerNMode_python.cc
    frequencyTrackerN_python.cc
    frequencyTracker_python.cc
    GrayDecode_python.cc
    grayEncode_python.cc
    hier_test2_python.cc
    Interleave_python.cc
    LoRaPDUHeaderGenerator_python.cc
    LoRaPDUHeaderReader_python.cc
    NibblesToBytes_python.cc
    normalizeCorrelation_python.cc
    PowerDetector_python.cc
    preambleDetector_python.cc
    randomize_python.cc
    readHeader_python.cc
    receiverController_python.cc
    Remainder_python.cc
    SelectMax_python.cc
    slidingDFTMax_python.cc
    slidingDFT_python.cc
    streamToHistoryVector_python.cc
    symbolCorrelator_python.cc
    symbolDemodNew_python.cc
    symbolDemod_python.cc
    SymbolMod_python.cc
    ToneDetector_python.cc
    TransmitterController_python.cc
    getSymbol_python.cc
    python_bindings.cc)

GR_PYBIND_MAKE_OOT(LibreLoRa 
   ../..
   gr::LibreLoRa
   "${LibreLoRa_python_files}")

install(TARGETS LibreLoRa_python DESTINATION ${GR_PYTHON_DIR}/LibreLoRa COMPONENT pythonapi)
