(use-modules
 ((guix licenses) #:prefix license:)
 (guix packages)
 (guix download)
 (guix git-download)
 (guix build-system cmake)
 (guix utils)
 (gnu packages documentation)
 (gnu packages python)
 (gnu packages boost)
 (gnu packages algebra)
 (gnu packages multiprecision)
 (gnu packages radio)
 (gnu packages logging)
 (gnu packages engineering)
 (gnu packages pkg-config)
 (gnu packages python-science)
 (gnu packages python-xyz)
 (gnu packages swig)
 (gnu packages version-control))

(package
 (name "gnuradio-librelora")
 (version "1.1")
 (source
  (origin
   (method git-fetch)
   (uri (git-reference
     	 (url "https://gitlab.com/jpsimas/librelora.git")
   (commit (string-append "v" version))))
   ;; (method url-fetch)
   ;; (uri (string-append "file:///home/jp/Polito/Thesis/librelora"))
   
   (sha256
    (base32 "12jfmqbbksa2bn6y59q6npmmr1fwsqq85rr1ii83ig02hhymr0v2"))))
 (build-system cmake-build-system)
 (arguments '(#:configure-flags '("-DCMAKE_BUILD_TYPE=RELEASEDEBUGMSGS")))
 (native-inputs
  `(("doxygen" ,doxygen)
   ("pkg-config" ,pkg-config)
   ("python" ,python)
   ("python-mako" ,python-mako)
   ("python-six" ,python-six)
   ("swig" ,swig)))
 (inputs
  `(("boost" ,boost)
   ("fftwf" ,fftwf)
   ("gmp" ,gmp)
   ("gnuradio" ,gnuradio)
   ("hackrf" ,hackrf)
   ("log4cpp" ,log4cpp)
   ("rtl-sdr" ,rtl-sdr)
   ("volk" ,volk)
   ("gnuradio-osmosdr" ,gnuradio-osmosdr)
   ("liquid-dsp" ,liquid-dsp)
   ("python-numpy" ,python-numpy)
   ("python-matplotlib" ,python-numpy)
   ("python-pyqt" ,python-numpy)))
 (synopsis "GNU Radio block for receiving and tranmitting using LoRa")
 (description "This is a block for GNU Radio that implements a LoRa PHY receiver and transmitter.")
 (home-page "https://gitlab.com/jpsimas/librelora")
 (license license:gpl3+))


