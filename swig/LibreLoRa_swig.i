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
#include "LibreLoRa/symbolDemod.h"
#include "LibreLoRa/grayEncode.h"
#include "LibreLoRa/deinterleave.h"
#include "LibreLoRa/decode.h"
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
%include "LibreLoRa/symbolDemod.h"
GR_SWIG_BLOCK_MAGIC2(LibreLoRa, symbolDemod);
%include "LibreLoRa/grayEncode.h"
GR_SWIG_BLOCK_MAGIC2(LibreLoRa, grayEncode);
%include "LibreLoRa/deinterleave.h"
GR_SWIG_BLOCK_MAGIC2(LibreLoRa, deinterleave);
%include "LibreLoRa/decode.h"
GR_SWIG_BLOCK_MAGIC2(LibreLoRa, decode);
