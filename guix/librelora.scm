(define-module (local librelora)
  #:use-module ((guix licenses) #:prefix license:)
  #:use-module (guix packages)
  #:use-module (guix download)
  #:use-module (guix git-download)
  #:use-module (guix utils)
  #:use-module (gnu packages algebra)
  #:use-module (gnu packages audio)
  #:use-module (gnu packages autotools)
  #:use-module (gnu packages base)
  #:use-module (gnu packages bash)
  #:use-module (gnu packages boost)
  #:use-module (gnu packages check)
  #:use-module (gnu packages curl)
  #:use-module (gnu packages databases)
  #:use-module (gnu packages documentation)
  #:use-module (gnu packages engineering)
  #:use-module (gnu packages fltk)
  #:use-module (gnu packages gcc)
  #:use-module (gnu packages gd)
  #:use-module (gnu packages geo)
  #:use-module (gnu packages gettext)
  #:use-module (gnu packages ghostscript)
  #:use-module (gnu packages glib)
  #:use-module (gnu packages gnome)
  #:use-module (gnu packages gstreamer)
  #:use-module (gnu packages gtk)
  #:use-module (gnu packages image)
  #:use-module (gnu packages libusb)
  #:use-module (gnu packages linux)
  #:use-module (gnu packages llvm)
  #:use-module (gnu packages logging)
  #:use-module (gnu packages lua)
  #:use-module (gnu packages maths)
  #:use-module (gnu packages multiprecision)
  #:use-module (gnu packages ncurses)
  #:use-module (gnu packages networking)
  #:use-module (gnu packages pkg-config)
  #:use-module (gnu packages pulseaudio)
  #:use-module (gnu packages python)
  #:use-module (gnu packages python-science)
  #:use-module (gnu packages python-xyz)
  #:use-module (gnu packages qt)
  #:use-module (gnu packages radio)
  #:use-module (gnu packages readline)
  #:use-module (gnu packages ruby)
  #:use-module (gnu packages sdl)
  #:use-module (gnu packages sphinx)
  #:use-module (gnu packages swig)
  #:use-module (gnu packages tcl)
  #:use-module (gnu packages tex)
  #:use-module (gnu packages texinfo)
  #:use-module (gnu packages video)
  #:use-module (gnu packages xiph)
  #:use-module (gnu packages xml)
  #:use-module (gnu packages xorg)
  #:use-module (guix build-system cmake)
  #:use-module (guix build-system glib-or-gtk)
  #:use-module (guix build-system gnu)
  #:use-module (guix build-system python)
  #:use-module (guix build-system qt))

(define-public castxml
  (package
   (name "castxml")
   (version "0.4.3")
   (source
    (origin
     (method git-fetch)
     (uri (git-reference
	   (url "https://github.com/CastXML/CastXML.git")
	   (commit (string-append "v" version))))
     (sha256
      (base32
       "1vqdrnycw1q65mjzh2sf37m252gxi77q13c0car7q6wric56qya9"))))
   (build-system cmake-build-system)

   (arguments
    ;;(let (())
    `(#:configure-flags
	(list (string-append "-DCLANG_RESOURCE_DIR="
			     (assoc-ref %build-inputs "clang")
			     "/lib/clang/"
			     ,(package-version clang)))))


   
   (inputs
    `(("clang" ,clang)
      ("python-sphinx" ,python-sphinx)))
   
   (home-page "https://github.com/CastXML/CastXML")
   (synopsis "C-family Abstract Syntax Tree XML Output.")
   (description
    "CastXML is a C-family abstract syntax tree XML output tool.")
   (license license:asl2.0)))

(define-public python-pygccxml
  (package
   (name "python-pygccxml")
   (version "2.1.0")
   (source
    (origin
     (method git-fetch)
     (uri (git-reference
       	   (url "https://github.com/CastXML/pygccxml.git")
	   (commit (string-append "v" version))))
     (sha256
      (base32
       "1vsxnfzz6qhiv8ac98qgk6w3s4j1jp661qy48gc9plcg2r952453"))))
   (build-system python-build-system)

   ;;tests seem to be broken (should use tox instead of default command)
   (arguments
    `(#:tests? #f))
   
   (native-inputs
    `(("python-setuptools" ,python-setuptools)))
    (inputs
     `(("castxml" ,castxml)
       ("python" ,python)))
    (synopsis "Specialized XML reader.")
    (description
     "Specialized XML reader that reads the output from CastXML.")
    (home-page "https://github.com/CastXML/pygccxml")
    (license license:boost1.0)))

(define-public gnuradio-librelora
  (package
   (name "gnuradio-librelora")
   (version "1.2")
   (source
    (origin
     (method git-fetch)
     (uri (git-reference
       	 (url "https://gitlab.com/jpsimas/librelora.git")
     (commit (string-append "v" version))))     
     (sha256
      (base32 "1b11hdmi77a0fgvxhvwk4xc7zpdyn51i4my58ir2fd5rzy8ga5hb"))))
   (build-system cmake-build-system)
   (arguments '(#:configure-flags '("-DCMAKE_BUILD_TYPE=RELEASE")))
   (native-inputs
    `(("doxygen" ,doxygen)
      ("pkg-config" ,pkg-config)
      ("pybind11" ,pybind11)
      ("python" ,python)
      ("python-mako" ,python-mako)
      ("python-six" ,python-six)))
   (inputs
    `(("boost" ,boost)
      ("fftwf" ,fftwf)
      ("gmp" ,gmp)
      ("gnuradio" ,gnuradio)
      ("hackrf" ,hackrf)
      ("log4cpp" ,log4cpp)
      ("rtl-sdr" ,rtl-sdr)
      ("volk" ,volk)
      ("gr-osmosdr" ,gr-osmosdr)
      ("liquid-dsp" ,liquid-dsp)
      ("python-matplotlib" ,python-numpy)
      ("python-numpy" ,python-numpy)
      ("python-pygccxml" ,python-pygccxml)
      ("python-pyqt" ,python-numpy)))
   (synopsis "GNU Radio block for receiving and tranmitting using LoRa")
   (description "This is a block for GNU Radio that implements a LoRa PHY receiver and transmitter.")
   (home-page "https://gitlab.com/jpsimas/librelora")
   (license license:gpl3+)))

 gnuradio-librelora
;; castxml
;; gnuradio
