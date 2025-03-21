{
  mkShell,
  cmake,
  libxcrypt,
  sdl2-compat,
  mtdutils,

  # defined by this project
  toolchain,
}:
mkShell {
  packages = [
    cmake
    libxcrypt
    sdl2-compat.dev
    mtdutils
  ];
  CMAKE_TOOLCHAIN_FILE = "${toolchain}/share/buildroot/toolchainfile.cmake";
}
