# Copyright 2011,2012,2016,2018,2019 Free Software Foundation, Inc.
#
# This file was generated by gr_modtool, a tool from the GNU Radio framework
# This file is a part of gr-LibreLoRa
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

########################################################################
# Setup library
########################################################################
include(GrPlatform) #define LIB_SUFFIX
list(APPEND LibreLoRa_sources
    getSymbol.cc
    frequencyTracker_impl.cc
    symbolCorrelator_impl.cc
    hier_test2_impl.cc
    preambleDetector_impl.cc
    normalizeCorrelation_impl.cc
    correlationSync_impl.cc
    symbolDemod_impl.cc
    grayEncode_impl.cc
    deinterleave_impl.cc
    getPairityMatrix.cc
    decode_impl.cc
    chirpSelect_impl.cc
    chirpSource_impl.cc
    #maxFrequency_impl.cc
    streamToHistoryVector_impl.cc
    slidingDFT_impl.cc
    ArgMaxAbs_impl.cc
    getSymbol.cc
    readHeader_impl.cc
    Correlation_impl.cc
    convolution_impl.cc
    correlationSyncDemod_impl.cc
    receiverController_impl.cc
    calculateHeaderChecksum.cc
    #blockFixedProduce.cc
    randomize_impl.cc
    NibblesToBytes_impl.cc
    CRC16_impl.cc
    slidingDFTMax_impl.cc
    frequencyTrackerN_impl.cc
    symbolDemodNew_impl.cc
    PowerDetector_impl.cc
    frequencyTrackerNLMS_impl.cc
    frequencyTrackerNMode_impl.cc
    ToneDetector_impl.cc
    DetectionCount_impl.cc
    BinSearchFFT_impl.cc
    FFTMax_impl.cc
    SelectMax_impl.cc
    Remainder_impl.cc
    FrequencyMod_impl.cc
    SymbolMod_impl.cc
    GrayDecode_impl.cc
    Encode_impl.cc
    Interleave_impl.cc
    BytesToNibbles_impl.cc
    TransmitterController_impl.cc
    AppendPrefix_impl.cc
    AppendSilence_impl.cc
    LoRaPDUHeaderReader_impl.cc
    LoRaPDUHeaderGenerator_impl.cc
    ChirpTrackMsgToFreq_impl.cc )

set(LibreLoRa_sources "${LibreLoRa_sources}" PARENT_SCOPE)
if(NOT LibreLoRa_sources)
    MESSAGE(STATUS "No C++ sources... skipping lib/")
    return()
endif(NOT LibreLoRa_sources)


#ADD USED GNURADIO LIBRARIES HERE!!!!!!!!!!!!!!!!!!!

add_library(gnuradio-LibreLoRa SHARED ${LibreLoRa_sources})

target_link_libraries(gnuradio-LibreLoRa gnuradio::gnuradio-runtime gnuradio-blocks gnuradio-filter liquid)

target_include_directories(gnuradio-LibreLoRa
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    PUBLIC $<INSTALL_INTERFACE:include>
  )

set_target_properties(gnuradio-LibreLoRa PROPERTIES DEFINE_SYMBOL "gnuradio_LibreLoRa_EXPORTS")

if(APPLE)
    set_target_properties(gnuradio-LibreLoRa PROPERTIES
        INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
    )
endif(APPLE)

########################################################################
# Install built library files
########################################################################
include(GrMiscUtils)
GR_LIBRARY_FOO(gnuradio-LibreLoRa)

########################################################################
# Print summary
########################################################################
message(STATUS "Using install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Building for version: ${VERSION} / ${LIBVER}")

########################################################################
# Build and register unit test
########################################################################
include(GrTest)

# If your unit tests require special include paths, add them here
# List all files that contain Boost.UTF unit tests here
list(APPEND test_LibreLoRa_sources
)
# Anything we need to link to for the unit tests go here
list(APPEND GR_TEST_TARGET_DEPS gnuradio-LibreLoRa)

if(NOT test_LibreLoRa_sources)
    MESSAGE(STATUS "No C++ unit tests... skipping")
    return()
endif(NOT test_LibreLoRa_sources)

foreach(qa_file ${test_LibreLoRa_sources})
    GR_ADD_CPP_TEST("LibreLoRa_${qa_file}"
        ${CMAKE_CURRENT_SOURCE_DIR}/${qa_file}
    )
endforeach(qa_file)
