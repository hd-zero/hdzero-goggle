{
  autoPatchelfHook,
  coreutils,
  fetchzip,
  libgcc,
  stdenv,
}:
stdenv.mkDerivation {
  name = "hdzero-google-toolchain";
  src = fetchzip {
    url = "https://toolchains.bootlin.com/downloads/releases/toolchains/armv7-eabihf/tarballs/armv7-eabihf--musl--stable-2018.02-2.tar.bz2";
    sha256 = "sha256-rcUDw4I7hryOzgY7aJwUuiQLpXX5MRKdGK0m1irzvfQ=";
  };
  dontCheckForBrokenSymlinks = true;
  nativeBuildInputs = [
    autoPatchelfHook
  ];
  buildInputs = [
    libgcc.lib
  ];
  buildPhase = ''
    cp -r . $out
  '';
  postFixup = ''
    for file in $(find $out -type f); do
      sed -i "s|/bin/true|${coreutils}/bin/true|g" $file
    done
  '';
}
