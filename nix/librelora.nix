{ lib
, stdenv
, fetchFromGitLab
, cmake
, pkg-config
, cppunit
, boost175
, log4cpp
, python
, doxygen
, gnuradio
, mpir
, volk
, gmp
, liquid-dsp
, icu
}:

stdenv.mkDerivation {
  pname = "gr-librelora";
  version = "1.4";
  src = fetchFromGitLab {
    owner = "jpsimas";
    repo = "librelora";
    rev = "v1.4";
    sha256 = "1licykxxnlgz6yxqnc2a6w7schn5907jj3lldg7lrahs9jy4bi6q";
  };
  disabledForGRafter = "3.9";

  nativeBuildInputs = [
    cmake
    pkg-config
    doxygen
    gnuradio.python.pkgs.pybind11
  ];

  buildInputs = [
    cppunit
    log4cpp
    gnuradio.boost
    gnuradio
    mpir
    volk
    gmp
    liquid-dsp
    icu
  ];

  propagatedBuildInputs  = [
    gnuradio.python.pkgs.numpy
    gnuradio.python.pkgs.matplotlib # for MatrixSink
    gnuradio.python.pkgs.pyqt5 # for MatrixSink
  ];

  meta = with lib; {
    description = "GNURadio Based LoRa PHY receiver and transmitter implementations";
    homepage = "https://gitlab.com/jpsimas/librelora";
    license = licenses.gpl3Plus;
    platforms = platforms.linux ++ platforms.darwin;
    maintainers = with maintainers; [];
  };
}
