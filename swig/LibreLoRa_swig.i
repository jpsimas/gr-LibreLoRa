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
#include "LibreLoRa/chirpSelect.h"
#include "LibreLoRa/chirpSource.h"
#include "LibreLoRa/slidingDFT.h"
#include "LibreLoRa/ArgMaxAbs.h"
//#include "LibreLoRa/maxFrequency.h"
//#include "LibreLoRa/streamToHistoryVector.h"
#include "LibreLoRa/getSymbol.h"
#include "LibreLoRa/readHeader.h"
#include "LibreLoRa/Correlation.h"
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
%include "LibreLoRa/chirpSelect.h"
GR_SWIG_BLOCK_MAGIC2(LibreLoRa, chirpSelect);
%include "LibreLoRa/chirpSource.h"
GR_SWIG_BLOCK_MAGIC2(LibreLoRa, chirpSource);
//%include "LibreLoRa/maxFrequency.h"
//GR_SWIG_BLOCK_MAGIC2(LibreLoRa, maxFrequency);
//%include "LibreLoRa/streamToHistoryVector.h"
//GR_SWIG_BLOCK_MAGIC2_TMPL(LibreLoRa, streamToHistoryVector_ff, streamToHistoryVector<float>);
 ////GR_SWIG_BLOCK_MAGIC2_TMPL(LibreLoRa, streamToHistoryVector_cc, streamToHistoryVector<gr_complex>);
////GR_SWIG_BLOCK_MAGIC2_TMPL(LibreLoRa, streamToHistoryVector_ii, streamToHistoryVector<int>);
%include "LibreLoRa/slidingDFT.h"
GR_SWIG_BLOCK_MAGIC2(LibreLoRa, slidingDFT);

%include "LibreLoRa/ArgMaxAbs.h"
GR_SWIG_BLOCK_MAGIC2(LibreLoRa, ArgMaxAbs);

%include "LibreLoRa/getSymbol.h"
%include "LibreLoRa/readHeader.h"
GR_SWIG_BLOCK_MAGIC2(LibreLoRa, readHeader);
%include "LibreLoRa/Correlation.h"
GR_SWIG_BLOCK_MAGIC2(LibreLoRa, Correlation);
