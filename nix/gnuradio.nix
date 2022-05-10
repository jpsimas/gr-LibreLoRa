with import <nixpkgs>{};

let
  soapyExtraPackages =  [
      soapyrtlsdr
      soapyhackrf
      soapyremote
      soapyaudio
      soapyuhd
  ];
  soapysdrWithPlugins = soapysdr.override {
    extraPackages = soapyExtraPackages;};
  soapyModulesVersion = with lib; versions.major soapysdrWithPlugins.version + "." + versions.minor soapysdrWithPlugins.version;
  soapyModulesPath = "lib/SoapySDR/modules" + soapyModulesVersion;
  soapyPackagesPath = lib.makeSearchPath soapyModulesPath soapyExtraPackages;
in
gnuradio.override {
  extraPythonPackages = [
    # add python libs to use inside GNU Radio here
  ];
  extraPackages = [
    (callPackage ./librelora.nix {})
    soapysdrWithPlugins
    # add other GNU Radio block libraries here
  ];
  extraMakeWrapperArgs = [
    "--prefix" "SOAPY_SDR_PLUGIN_PATH" ":" soapyPackagesPath
  ];
}

