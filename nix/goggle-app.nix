{
  # nixpkgs inputs
  cmake,
  lib,
  mtdutils,
  stdenv,

  # defined by this project
  hdzero-goggle-src,
  toolchain,
}:
let
  inherit (lib)
    readFile
    removeSuffix
    ;
  version = removeSuffix "\n" (readFile (hdzero-goggle-src + "/VERSION"));
in
stdenv.mkDerivation {
  pname = "hdzero-goggle";
  version = version;
  src = hdzero-goggle-src;
  postPatch = ''
    substituteInPlace ./mkapp/mkapp_ota.sh \
      --replace-fail "APP_VERSION=\$(get_app_version)" "APP_VERSION=${version}"
    patchShebangs ./mkapp
  '';
  dontConfigure = true;
  dontInstall = true;
  dontFixup = true;
  nativeBuildInputs = [
    cmake
    mtdutils
  ];
  buildPhase = ''
    mkdir build
    cmake . -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=${toolchain}/share/buildroot/toolchainfile.cmake -Bbuild
    cd build
    make all -j$NIX_BUILD_CORES
    mv ../out $out
  '';
}
