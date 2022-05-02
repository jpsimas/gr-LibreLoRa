with import <nixpkgs>{};

gnuradio.override {
  extraPythonPackages = [
    # add python libs to use inside GNU Radio here
  ];
  extraPackages = [
    (callPackage ./librelora.nix {})
    # add other GNU Radio block libraries here
  ];
}

