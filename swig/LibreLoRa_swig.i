/* -*- c++ -*- */

#define LIBRELORA_API

%include "gnuradio.i"           // the common stuff

//load generated python docstrings
%include "LibreLoRa_swig_doc.i"

%{
#include "LibreLoRa/frequencyTracker.h"
#include "LibreLoRa/symbolCorrelator.h"
#include "LibreLoRa/hier_test2.h"
#include "LibreLoRa/preambleDetector.h"
#include "LibreLoRa/normalizeCorrelation.h"
#include "LibreLoRa/correlationSync.h"
%}

%include "LibreLoRa/frequencyTracker.h"
GR_SWIG_BLOCK_MAGIC2(LibreLoRa, frequencyTracker);
%include "LibreLoRa/symbolCorrelator.h"
GR_SWIG_BLOCK_MAGIC2(LibreLoRa, symbolCorrelator);
%include "LibreLoRa/hier_test2.h"
GR_SWIG_BLOCK_MAGIC2(LibreLoRa, hier_test2);
%include "LibreLoRa/preambleDetector.h"
GR_SWIG_BLOCK_MAGIC2(LibreLoRa, preambleDetector);
%include "LibreLoRa/normalizeCorrelation.h"
GR_SWIG_BLOCK_MAGIC2(LibreLoRa, normalizeCorrelation);
%include "LibreLoRa/correlationSync.h"
GR_SWIG_BLOCK_MAGIC2(LibreLoRa, correlationSync);
