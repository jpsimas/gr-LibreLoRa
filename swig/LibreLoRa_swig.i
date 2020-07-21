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
#include "LibreLoRa/streamToHistoryVector.h"
#include "LibreLoRa/getSymbol.h"
#include "LibreLoRa/readHeader.h"
#include "LibreLoRa/Correlation.h"
#include "LibreLoRa/convolution.h"
#include "LibreLoRa/correlationSyncDemod.h"
#include "LibreLoRa/receiverController.h"
#include "LibreLoRa/calculateHeaderChecksum.h"
#include "LibreLoRa/blockFixedProduce.h"
#include "LibreLoRa/randomize.h"
#include "LibreLoRa/NibblesToBytes.h"
#include "LibreLoRa/CRC16.h"
#include "LibreLoRa/slidingDFTMax.h"
#include "LibreLoRa/frequencyTrackerN.h"
#include "LibreLoRa/symbolDemodNew.h"
#include "LibreLoRa/PowerDetector.h"
#include "LibreLoRa/frequencyTackerAPA.h"
#include "LibreLoRa/frequencyTrackerNMode.h"
#include "LibreLoRa/ToneDetector.h"
#include "LibreLoRa/DetectionCount.h"
%}
  
%include "LibreLoRa/frequencyTracker.h"
GR_SWIG_BLOCK_MAGIC2_TMPL(LibreLoRa, frequencyTracker_ff, frequencyTracker<float>);
GR_SWIG_BLOCK_MAGIC2_TMPL(LibreLoRa, frequencyTracker_cc, frequencyTracker<gr_complex>);
%include "LibreLoRa/symbolCorrelator.h"
GR_SWIG_BLOCK_MAGIC2(LibreLoRa, symbolCorrelator);
%include "LibreLoRa/hier_test2.h"
GR_SWIG_BLOCK_MAGIC2(LibreLoRa, hier_test2);
%include "LibreLoRa/preambleDetector.h"
GR_SWIG_BLOCK_MAGIC2(LibreLoRa, preambleDetector);
%include "LibreLoRa/normalizeCorrelation.h"
GR_SWIG_BLOCK_MAGIC2(LibreLoRa, normalizeCorrelation);
%include "LibreLoRa/correlationSync.h"
GR_SWIG_BLOCK_MAGIC2_TMPL(LibreLoRa, correlationSync_ff, correlationSync<float>);
GR_SWIG_BLOCK_MAGIC2_TMPL(LibreLoRa, correlationSync_cc, correlationSync<gr_complex>);
%include "LibreLoRa/symbolDemod.h"
GR_SWIG_BLOCK_MAGIC2_TMPL(LibreLoRa, symbolDemod_ff, symbolDemod<float>);
GR_SWIG_BLOCK_MAGIC2_TMPL(LibreLoRa, symbolDemod_cc, symbolDemod<gr_complex>);

%include "LibreLoRa/grayEncode.h"
GR_SWIG_BLOCK_MAGIC2(LibreLoRa, grayEncode);
%include "LibreLoRa/deinterleave.h"
GR_SWIG_BLOCK_MAGIC2(LibreLoRa, deinterleave);
%include "LibreLoRa/decode.h"
GR_SWIG_BLOCK_MAGIC2(LibreLoRa, decode);
%include "LibreLoRa/randomize.h"
GR_SWIG_BLOCK_MAGIC2(LibreLoRa, randomize);
%include "LibreLoRa/receiverController.h"
GR_SWIG_BLOCK_MAGIC2(LibreLoRa, receiverController);

%include "LibreLoRa/chirpSelect.h"
GR_SWIG_BLOCK_MAGIC2(LibreLoRa, chirpSelect);
%include "LibreLoRa/chirpSource.h"
GR_SWIG_BLOCK_MAGIC2(LibreLoRa, chirpSource);
//%include "LibreLoRa/maxFrequency.h"
//GR_SWIG_BLOCK_MAGIC2(LibreLoRa, maxFrequency);

%include "LibreLoRa/streamToHistoryVector.h"
 //%template(streamToHistoryVector_ff) gr::LibreLoRa::streamToHistoryVector<float>;
//GR_SWIG_BLOCK_MAGIC2(LibreLoRa, streamToHistoryVector_ff);
//GR_SWIG_BLOCK_MAGIC2(LibreLoRa, streamToHistoryVector_ii);
//GR_SWIG_BLOCK_MAGIC2(LibreLoRa, streamToHistoryVector_cc);
GR_SWIG_BLOCK_MAGIC2_TMPL(LibreLoRa, streamToHistoryVector_ff, streamToHistoryVector<float>);
GR_SWIG_BLOCK_MAGIC2_TMPL(LibreLoRa, streamToHistoryVector_cc, streamToHistoryVector<gr_complex>);

%include "LibreLoRa/slidingDFT.h"
GR_SWIG_BLOCK_MAGIC2(LibreLoRa, slidingDFT);

%include "LibreLoRa/ArgMaxAbs.h"
GR_SWIG_BLOCK_MAGIC2(LibreLoRa, ArgMaxAbs);

%include "LibreLoRa/getSymbol.h"
%template(getSymbol) gr::LibreLoRa::getSymbol<float>;

%include "LibreLoRa/readHeader.h"

GR_SWIG_BLOCK_MAGIC2(LibreLoRa, readHeader);
// %include "LibreLoRa/Correlation.h"
// GR_SWIG_BLOCK_MAGIC2(LibreLoRa, Correlation);

%include "LibreLoRa/Correlation.h"
GR_SWIG_BLOCK_MAGIC2_TMPL(LibreLoRa, Correlation_ff, Correlation<float>);
GR_SWIG_BLOCK_MAGIC2_TMPL(LibreLoRa, Correlation_cc, Correlation<gr_complex>);

%include "LibreLoRa/convolution.h"
GR_SWIG_BLOCK_MAGIC2(LibreLoRa, convolution);
%include "LibreLoRa/correlationSyncDemod.h"
GR_SWIG_BLOCK_MAGIC2(LibreLoRa, correlationSyncDemod);

%include "LibreLoRa/calculateHeaderChecksum.h"

%include "LibreLoRa/NibblesToBytes.h"
GR_SWIG_BLOCK_MAGIC2(LibreLoRa, NibblesToBytes);
%include "LibreLoRa/CRC16.h"
GR_SWIG_BLOCK_MAGIC2(LibreLoRa, CRC16);
%include "LibreLoRa/slidingDFTMax.h"
GR_SWIG_BLOCK_MAGIC2(LibreLoRa, slidingDFTMax);
%include "LibreLoRa/frequencyTrackerN.h"
GR_SWIG_BLOCK_MAGIC2_TMPL(LibreLoRa, frequencyTrackerN_ff, frequencyTrackerN<float>);
GR_SWIG_BLOCK_MAGIC2_TMPL(LibreLoRa, frequencyTrackerN_cc, frequencyTrackerN<gr_complex>);
%include "LibreLoRa/symbolDemodNew.h"
GR_SWIG_BLOCK_MAGIC2(LibreLoRa, symbolDemodNew);

%include "LibreLoRa/PowerDetector.h"
GR_SWIG_BLOCK_MAGIC2(LibreLoRa, PowerDetector);

%include "LibreLoRa/frequencyTackerAPA.h"
GR_SWIG_BLOCK_MAGIC2_TMPL(LibreLoRa, frequencyTackerAPA_single, frequencyTackerAPA<1>);
GR_SWIG_BLOCK_MAGIC2_TMPL(LibreLoRa, frequencyTackerAPA_double, frequencyTackerAPA<2>);

%include "LibreLoRa/frequencyTrackerNMode.h"
GR_SWIG_BLOCK_MAGIC2_TMPL(LibreLoRa, frequencyTrackerNMode_ff, frequencyTrackerNMode<float>);
GR_SWIG_BLOCK_MAGIC2_TMPL(LibreLoRa, frequencyTrackerNMode_cc, frequencyTrackerNMode<gr_complex>);
%include "LibreLoRa/ToneDetector.h"
GR_SWIG_BLOCK_MAGIC2(LibreLoRa, ToneDetector);



%include "LibreLoRa/DetectionCount.h"
GR_SWIG_BLOCK_MAGIC2(LibreLoRa, DetectionCount);


