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
, gnuradioPackages
}:

stdenv.mkDerivation {
  pname = "gr-librelora";
  version = "1.3";
  src = fetchFromGitLab {
    owner = "jpsimas";
    repo = "librelora";
    rev = "v1.3";
    sha256 = "1g0sgasjngzwd9pgbb3bnswy6c0k29j0rld0k862w2zjsg52rwra";
  };
  disabledForGRafter = "3.9";

  nativeBuildInputs = [
    cmake
    pkg-config
    doxygen
    gnuradio.unwrapped.python.pkgs.pybind11
  ];

  buildInputs = [
    cppunit
    log4cpp
    gnuradio.unwrapped.boost
    gnuradio
    mpir
    volk
    gmp
    liquid-dsp
    icu
  ];

  propagatedBuildInputs  = [
    gnuradio.unwrapped.python.pkgs.numpy
  ];

  meta = with lib; {
    description = "GNURadio Based LoRa PHY receiver and transmitter implementations";
    homepage = "https://gitlab.com/jpsimas/librelora";
    license = licenses.gpl3Plus;
    platforms = platforms.linux ++ platforms.darwin;
    maintainers = with maintainers; [];
  };
}
