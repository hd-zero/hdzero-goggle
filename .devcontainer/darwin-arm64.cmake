# CMake cross-compile toolchain: Linux host -> arm64 macOS (Mach-O), using LLVM.
# clang retargets via triple; ld64.lld links Mach-O; SDK provides headers/.tbd stubs.
set(CMAKE_SYSTEM_NAME Darwin)
set(CMAKE_SYSTEM_PROCESSOR arm64)

# macOS SDK sysroot (extracted into the image; overridable via env)
if(DEFINED ENV{MACOS_SDK})
    set(SDK "$ENV{MACOS_SDK}")
else()
    set(SDK "/opt/sdk/MacOSX.sdk")
endif()

# Minimum macOS deployment target (keep conservative for wide compatibility)
set(MACOS_MIN "12.0")
set(TARGET_TRIPLE "arm64-apple-macos${MACOS_MIN}")

set(CMAKE_C_COMPILER   clang)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_ASM_COMPILER clang)
set(CMAKE_OBJC_COMPILER   clang)
set(CMAKE_OBJCXX_COMPILER clang++)
set(CMAKE_C_COMPILER_TARGET      "${TARGET_TRIPLE}")
set(CMAKE_CXX_COMPILER_TARGET    "${TARGET_TRIPLE}")
set(CMAKE_ASM_COMPILER_TARGET    "${TARGET_TRIPLE}")
set(CMAKE_OBJC_COMPILER_TARGET   "${TARGET_TRIPLE}")
set(CMAKE_OBJCXX_COMPILER_TARGET "${TARGET_TRIPLE}")

# Mach-O tooling from LLVM
set(CMAKE_AR      llvm-ar)
set(CMAKE_RANLIB  llvm-ranlib)
set(CMAKE_NM      llvm-nm)
set(CMAKE_STRIP   llvm-strip)

set(CMAKE_OSX_SYSROOT "${SDK}")
set(CMAKE_OSX_ARCHITECTURES "arm64")
set(CMAKE_OSX_DEPLOYMENT_TARGET "${MACOS_MIN}")

# Use LLVM's Mach-O linker (clang selects ld64.lld for Darwin targets with -fuse-ld=lld)
# clang derives -platform_version from the -macos<ver> triple automatically.
add_compile_options("-isysroot" "${SDK}")
add_link_options("-fuse-ld=lld" "-isysroot" "${SDK}")

set(CMAKE_FIND_ROOT_PATH "${SDK}")
# Cross-built macOS deps (e.g. SDL2) live outside the SDK sysroot; add them so
# find_package/find_library (rooted by FIND_ROOT_PATH) can locate them.
if(DEFINED ENV{SDL2_MACOS})
    list(APPEND CMAKE_FIND_ROOT_PATH "$ENV{SDL2_MACOS}")
endif()
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
