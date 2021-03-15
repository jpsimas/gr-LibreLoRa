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

(define-public gnuradio
  (package
   (name "gnuradio")
   (version "3.9.0.0")
   (source
    (origin
     (method url-fetch)
     (uri (string-append "https://www.gnuradio.org/releases/gnuradio/"
			 "gnuradio-" version ".tar.xz"))
     (sha256
      (base32 "1jvm9xd0l2pz1fww4zii6hl7ccnvy256nrf70ljb594n7j9j49ha"))))
   (build-system cmake-build-system)
   (native-inputs
    `(("doxygen" ,doxygen)
      ("ghostscript" ,ghostscript)
      ("orc" ,orc)
      ("pkg-config" ,pkg-config)
      ("pybind11" ,pybind11)
      ("python-cheetah" ,python-cheetah)
      ("python-mako" ,python-mako)
      ("python-pyzmq" ,python-pyzmq)
      ("python-scipy" ,python-scipy)
      ("python-sphinx" ,python-sphinx)
      ("texlive" ,(texlive-union (list texlive-amsfonts
                                       texlive-latex-amsmath
                                       ;; TODO: Add newunicodechar.
                                       texlive-latex-graphics)))
      ("xorg-server" ,xorg-server-for-tests)))
   (inputs
    `(("alsa-lib" ,alsa-lib)
      ("boost" ,boost)
      ("cairo" ,cairo)
      ("codec2" ,codec2)
      ("cppzmq" ,cppzmq)
      ("fftwf" ,fftwf)
      ("gmp" ,gmp)
      ("gsl" ,gsl)
      ("gsm" ,gsm)
      ("gtk+" ,gtk+)
      ("jack" ,jack-1)
      ("log4cpp" ,log4cpp)
      ("pango" ,pango)
      ("portaudio" ,portaudio)
      ("python" ,python)
      ("python-click" ,python-click)
      ("python-click-plugins" ,python-click-plugins)
      ("python-lxml" ,python-lxml)
      ("python-numpy" ,python-numpy)
      ("python-pycairo" ,python-pycairo)
      ("python-pygccxml" ,python-pygccxml)
      ("python-pygobject" ,python-pygobject)
      ("python-pyqt" ,python-pyqt)
      ("python-pyyaml" ,python-pyyaml)
      ("qtbase" ,qtbase)
      ("qwt" ,qwt)
      ("sdl" ,sdl)
      ("volk" ,volk)
      ("zeromq" ,zeromq)))
   (arguments
    `(#:modules ((guix build cmake-build-system)
		 ((guix build glib-or-gtk-build-system) #:prefix glib-or-gtk:)
		 ((guix build python-build-system) #:prefix python:)
		 (guix build utils)
		 (ice-9 match))
      #:imported-modules (,@%cmake-build-system-modules
                          (guix build glib-or-gtk-build-system)
                          (guix build python-build-system))
      ;;#:configure-flags
      ;;'("-DENABLE_INTERNAL_VOLK=OFF");;REMOVED IN 3.9
      #:phases
      (modify-phases %standard-phases
		     (add-after 'unpack 'fix-paths
				(lambda* (#:key inputs #:allow-other-keys)
				  (let ((qwt (assoc-ref inputs "qwt")))
				    (substitute* "cmake/Modules/FindQwt.cmake"
						 (("/usr/include")
						  (string-append qwt "/include"))
						 (("/usr/lib")
						  (string-append qwt "/lib"))
						 (("qwt6-\\$\\{QWT_QT_VERSION\\}")
						  "qwt")))
				  (substitute* "cmake/Modules/GrPython.cmake"
					       (("dist-packages")
						"site-packages"))
				  (substitute* '(;;"gr-vocoder/swig/vocoder_swig.i"
						 "gr-vocoder/include/gnuradio/vocoder/codec2.h"
						 "gr-vocoder/include/gnuradio/vocoder/freedv_api.h")
					       (("<codec2/")
						"<"))
				  #t))
		     (add-before 'check 'set-test-environment
				 (lambda* (#:key inputs #:allow-other-keys)
				   (setenv "HOME" "/tmp")
				   (system (string-append (assoc-ref inputs "xorg-server")
							  "/bin/Xvfb :1 &"))
				   (setenv "DISPLAY" ":1")
				   #t))
		     (replace 'check
			      (lambda* (#:key tests? parallel-tests? #:allow-other-keys)
				(invoke "ctest" "-j" (if parallel-tests?
							 (number->string (parallel-job-count))
							 "1")
					"--output-on-failure"
					;;disable broken tests
					"-E" (string-join
					      '(;; https://github.com/gnuradio/gnuradio/issues/3871
						"qa_header_payload_demux"
						;; https://github.com/gnuradio/gnuradio/issues/4348
						"qa_packet_headerparser_b")
					      "|"))))
		     (add-after 'install 'wrap-python
				(assoc-ref python:%standard-phases 'wrap))
		     (add-after 'wrap-python 'wrap-glib-or-gtk
				(assoc-ref glib-or-gtk:%standard-phases 'glib-or-gtk-wrap))
		     (add-after 'wrap-glib-or-gtk 'wrap-with-GI_TYPELIB_PATH
				(lambda* (#:key inputs outputs #:allow-other-keys)
				  (let ((out (assoc-ref outputs "out"))
					(paths (map (match-lambda
						      ((output . directory)
						       (let ((girepodir (string-append
									 directory
									 "/lib/girepository-1.0")))
							 (if (file-exists? girepodir)
							     girepodir
							     #f))))
						    inputs)))
				    (wrap-program (string-append out "/bin/gnuradio-companion")
						  `("GI_TYPELIB_PATH" ":" prefix ,(filter identity paths))))
				  #t)))))
   (native-search-paths
    ;; Variables required to find third-party plugins at runtime.
    (list (search-path-specification
           (variable "GRC_BLOCKS_PATH")
           (files '("share/gnuradio/grc/blocks")))
          (search-path-specification
           (variable "PYTHONPATH")
           (files (list (string-append "lib/python"
                                       (version-major+minor
					(package-version python))
                                       "/site-packages"))))))
   (synopsis "Toolkit for software-defined radios")
   (description
    "GNU Radio is a development toolkit that provides signal processing blocks
to implement software radios.  It can be used with external RF hardware to
create software-defined radios, or without hardware in a simulation-like
environment.")
   (home-page "https://www.gnuradio.org")
   (license license:gpl3+)))

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
   (version "1.1")
   (source
    (origin
     ;; (method git-fetch)
     ;; (uri (git-reference
     ;;   	 (url "https://gitlab.com/jpsimas/librelora.git")
     ;; (commit (string-append "v" version))))
     (method url-fetch)
     (uri (string-append "file:///home/jp/Polito/Thesis/librelora-3.9"))
     
     (sha256
      (base32 "0r0qh20f15bfckzyis847lz4havmfyi032svd0frwwc8anrz1cni"))))
   (build-system cmake-build-system)
   (arguments '(#:configure-flags '("-DCMAKE_BUILD_TYPE=RELEASEDEBUGMSGS")))
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
      ("gnuradio-osmosdr" ,gnuradio-osmosdr)
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
