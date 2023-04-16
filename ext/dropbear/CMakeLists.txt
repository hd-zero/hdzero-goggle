cmake_minimum_required(VERSION 3.14)
project (dropbear C)

if(DEFINED CMAKE_TOOLCHAIN_FILE)
	set(CMAKE_C_FLAGS "-mfpu=neon -mfloat-abi=hard")
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-function -Wno-unused-variable -ffunction-sections -fdata-sections -Wl,-gc-sections -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64")
set(CMAKE_C_FLAGS_DEBUG "-g")
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")

if(DEFINED CMAKE_TOOLCHAIN_FILE)
	get_filename_component(COMPILER_NAME ${CMAKE_C_COMPILER} NAME)
	get_filename_component(COMPILER_PATH ${CMAKE_C_COMPILER} PATH)
	string(REGEX REPLACE "(.*)-gcc" "\\1" TARGET_PLATFORM "${COMPILER_NAME}")
	set(TARGET_OPTIONS
		"CFLAGS=${CMAKE_C_FLAGS}"
		"CC=${COMPILER_PATH}/${TARGET_PLATFORM}-gcc"
		"AR=${COMPILER_PATH}/${TARGET_PLATFORM}-ar"
		"LD=${COMPILER_PATH}/${TARGET_PLATFORM}-ld"
		"RANLIB=${COMPILER_PATH}/${TARGET_PLATFORM}-ranlib"
		"NM=${COMPILER_PATH}/${TARGET_PLATFORM}-nm"
		"AS=${COMPILER_PATH}/${TARGET_PLATFORM}-as"
		"OBJDUMP=${COMPILER_PATH}/${TARGET_PLATFORM}-objdump"
		"OBJCOPY=${COMPILER_PATH}/${TARGET_PLATFORM}-objcopy"
		"STRIP=${COMPILER_PATH}/${TARGET_PLATFORM}-strip"
		"STRINGS=${COMPILER_PATH}/${TARGET_PLATFORM}-strings"
		"SIZE=${COMPILER_PATH}/${TARGET_PLATFORM}-size"
	)
	set(ENV ${TARGET_OPTIONS})
endif()

include(FetchContent)

FetchContent_Declare(
	dropbear
	GIT_REPOSITORY "https://github.com/mkj/dropbear.git"
	GIT_TAG        "DROPBEAR_2022.83"
	SOURCE_DIR     "src"
	BINARY_DIR     "out"
)
FetchContent_MakeAvailable(dropbear)

execute_process(
	# GCC 4.9 bug, both stddef.h stdlib.h define NULL without inclusion guards.
	COMMAND sed -i "s@#include <stddef.h>@//#include <stddef.h>@g" ${dropbear_SOURCE_DIR}/libtommath/tommath.h
	COMMAND ./configure --host=${TARGET_PLATFORM} --disable-zlib --enable-static
	WORKING_DIRECTORY ${dropbear_SOURCE_DIR}
)

add_custom_command(
	OUTPUT dropbear_build
	COMMAND cd ${dropbear_SOURCE_DIR} && make PROGRAMS='dropbear dbclient dropbearkey dropbearconvert scp' MULTI=1 SCPPROGRESS=1
	COMMAND cp ${dropbear_SOURCE_DIR}/dropbearmulti ${dropbear_BINARY_DIR}
)

add_custom_command(
	OUTPUT dropbear_clean
	COMMAND find . -name '*.o' -delete
	COMMAND find . -name '*.a' -delete
	COMMAND rm -rf ${dropbear_BINARY_DIR}/*
)

add_custom_target(dropbearmulti ALL DEPENDS dropbear_build)
add_custom_target(cleanall dropbearmulti DEPENDS dropbear_clean)
