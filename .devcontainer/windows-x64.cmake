# Cross-compile the SDL emulator to Windows x86_64 (amd64) with mingw-w64, from
# the hdzero-dev-win image. Run/test the resulting .exe on Windows — or on a
# Windows-on-ARM VM via the built-in x64 emulation.
#
# The fake FPV video feed works here too: it decodes a looping MPEG-1 clip with
# pl_mpeg (single-header, portable), so there is no ffmpeg to link on any platform.
#
#   cmake -S . -B build_win_x64 -DEMULATOR_BUILD=ON -DHDZ_GOGGLE=ON \
#         -DCMAKE_TOOLCHAIN_FILE=.devcontainer/windows-x64.cmake

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(TOOLCHAIN_PREFIX x86_64-w64-mingw32)
# The -posix variants use the POSIX threading model, which provides winpthread
# (pthread_*). The default (win32 model) lacks pthreads and fails to link.
set(CMAKE_C_COMPILER   ${TOOLCHAIN_PREFIX}-gcc-posix)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}-g++-posix)
set(CMAKE_RC_COMPILER  ${TOOLCHAIN_PREFIX}-windres)

# SDL2 mingw dev tree, staged into the image at /opt/sdl2-mingw.
set(SDL2_DIR /opt/sdl2-mingw/${TOOLCHAIN_PREFIX}/lib/cmake/SDL2)

set(CMAKE_FIND_ROOT_PATH /usr/${TOOLCHAIN_PREFIX} /opt/sdl2-mingw/${TOOLCHAIN_PREFIX})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
