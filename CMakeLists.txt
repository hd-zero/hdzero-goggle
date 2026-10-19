cmake_minimum_required(VERSION 3.10)

project(HDZGOGGLE VERSION 1.1)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(COMMON_COMPILER_FLAGS "-Wno-unused-function -Wno-unused-variable -Wno-deprecated-declarations -ffunction-sections -fdata-sections -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64")
# Dead-strip unused sections: macOS ld64 uses -dead_strip, GNU ld uses -gc-sections.
if(APPLE)
	# Also disable _FORTIFY_SOURCE: macOS clang's fortified __*_chk wrappers trap
	# (SIGTRAP) on some firmware snprintf calls that are benign on Linux.
	set(COMMON_COMPILER_FLAGS "${COMMON_COMPILER_FLAGS} -Wl,-dead_strip -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0")
else()
	set(COMMON_COMPILER_FLAGS "${COMMON_COMPILER_FLAGS} -Wl,-gc-sections")
endif()

option(EMULATOR_BUILD "emulate using SDL on host" OFF)
if(NOT EMULATOR_BUILD)
	set(COMMON_COMPILER_FLAGS "${COMMON_COMPILER_FLAGS} -mfpu=neon -mfloat-abi=hard")
endif()

option(HDZ_GOGGLE "Target Goggles" OFF)
if (HDZ_GOGGLE)
	set(COMMON_COMPILER_FLAGS "${COMMON_COMPILER_FLAGS} -DHDZGOGGLE=1")
	file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/mkapp/app/platform "HDZGOGGLE")
endif()

option(HDZ_BOXPRO "Target BoxPRO" OFF)
if (HDZ_BOXPRO)
	set(COMMON_COMPILER_FLAGS "${COMMON_COMPILER_FLAGS} -DHDZBOXPRO=1")
	file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/mkapp/app/platform "HDZBOXPRO")
endif()

option(HDZ_GOGGLE2 "Target Goggles2" OFF)
if (HDZ_GOGGLE2)
	set(COMMON_COMPILER_FLAGS "${COMMON_COMPILER_FLAGS} -DHDZGOGGLE2=1")
	file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/mkapp/app/platform "HDZGOGGLE2")
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_COMPILER_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_COMPILER_FLAGS}")

set(CMAKE_C_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_DEBUG "-g")

set(CMAKE_C_FLAGS_RELEASE "-Werror -O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-Werror -O3 -DNDEBUG")

set(OS_APP_PATH ${CMAKE_CURRENT_BINARY_DIR}/mkapp/app/app)
set(RECORD_APP_PATH ${CMAKE_CURRENT_BINARY_DIR}/mkapp/app/app/record)

# common build settings
if(APPLE)
	# On macOS, crypt/dl/rt live in libSystem; C++ runtime is libc++.
	set(STANDARD_LIBRARIES
		c
		m
		pthread
		c++
	)
elseif(WIN32)
	# mingw (Windows): crypt/dl/rt don't exist; pthread is winpthread (posix model).
	set(STANDARD_LIBRARIES
		m
		pthread
		stdc++
	)
else()
	set(STANDARD_LIBRARIES
		c
		crypt
		dl
		m
		pthread
		rt
		stdc++
	)
endif()

add_definitions(
	-DAWCHIP=AW_V5 
	-D_GNU_SOURCE 
	-DUSE_LOG_LIB_GLOG
)

if(EMULATOR_BUILD)
	add_definitions(
		-DEMULATOR_BUILD
	)
endif()

# dependencies
add_subdirectory(lib/lvgl)
add_subdirectory(lib/minIni)
add_subdirectory(lib/log)
add_subdirectory(lib/esp-loader)
add_subdirectory(lib/softwinner)

if(EMULATOR_BUILD)
	find_package(SDL2 REQUIRED)
else()
	add_subdirectory(lib/ffmpeg)
	add_subdirectory(lib/live)
endif()

# main application
file(GLOB SRC_FILES_CORE   "src/core/*.c" "src/core/*.h")
file(GLOB SRC_FILES_DRIVER "src/driver/*.c" "src/driver/*.h")
file(GLOB SRC_FILES_FONTS  "src/fonts/*.c" "src/fonts/*.h")
file(GLOB SRC_FILES_UI   "src/ui/*.c" "src/ui/*.h")
file(GLOB SRC_FILES_BMI   "src/bmi270/*.c" "src/bmi270/*.h")
file(GLOB SRC_FILES_UTIL "src/util/*.c" "src/util/*.h")
file(GLOB SRC_FILES_PLAYER "src/player/*.c" "src/player/*.h")
file(GLOB SRC_FILES_EMULATOR "src/emulator/*.c" "src/emulator/*.h")
file(GLOB SRC_FILES_LANG "src/lang/*.c" "src/lang/*.h")
file(GLOB SRC_FILES_PLATFORM "src/platform/*.c" "src/platform/*.h")

# Per-target driver swaps: the emulator provides its own implementations of certain
# hardware drivers (src/emulator/*), so drop the goggle versions to avoid duplicate
# symbols. Keeps the hardware drivers free of any #ifdef EMULATOR_BUILD.
#   uart.c       -> emulator/uart_emu.c         (socketpair UART)
#   i2c.c        -> emulator/i2c_emu.c          (no-op bus)
#   rtc_hw.c     -> emulator/rtc_hw_emu.c       (in-memory clock; rtc.c shared)
#   fbtools.c    -> (none)                      (SDL renders instead; not referenced)
#   input_evdev.c-> emulator/input_device_emu.c (SDL keyboard; core/input_device.c shared)
if(EMULATOR_BUILD)
	list(REMOVE_ITEM SRC_FILES_DRIVER
		"${CMAKE_CURRENT_SOURCE_DIR}/src/driver/uart.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/driver/i2c.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/driver/rtc_hw.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/driver/fbtools.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/driver/input_evdev.c")
endif()

if(HDZ_GOGGLE)
	file(GLOB SRC_FILES_IMAGE  "src/image/goggle/*.c" "src/image/goggle/*.h")
elseif(HDZ_BOXPRO)
	file(GLOB SRC_FILES_IMAGE  "src/image/boxpro/*.c" "src/image/boxpro/*.h")
elseif(HDZ_GOGGLE2)
	file(GLOB SRC_FILES_IMAGE  "src/image/goggle2/*.c" "src/image/goggle2/*.h")
endif()

set(SRC_FILES
	${SRC_FILES_CORE}
	${SRC_FILES_DRIVER}
	${SRC_FILES_FONTS}
	${SRC_FILES_IMAGE}
	${SRC_FILES_UI}
	${SRC_FILES_BMI}
	${SRC_FILES_UTIL}
	${SRC_FILES_LANG}
	${SRC_FILES_PLATFORM}
)

if(EMULATOR_BUILD)
	list(APPEND SRC_FILES ${SRC_FILES_EMULATOR})
else()
	list(APPEND SRC_FILES ${SRC_FILES_PLAYER})
endif()

add_executable(${PROJECT_NAME} ${SRC_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE 
	src/
	src/core
	src/driver
	src/player
	src/player/media/app
	src/window

	lib/linux/include
	lib/linux/include/video
)
target_link_libraries(${PROJECT_NAME} PRIVATE
	${STANDARD_LIBRARIES}

	log
	lvgl
	minIni
	esp-loader
	softwinner
)

if(EMULATOR_BUILD)
	# SDL2_INCLUDE_DIRS carries both <prefix>/include and <prefix>/include/SDL2, so
	# the code's `#include <SDL2/SDL.h>` resolves off Homebrew/MSYS too (on Linux the
	# system /usr/include makes it implicit; the imported target only exposes the
	# deep dir, hence the explicit dirs here).
	target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})
	target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_LIBRARIES})
endif()

# The emulator's mock video device decodes a small MPEG-1 clip with pl_mpeg
# (single-header, in lib/). No ffmpeg is linked into any app binary on any OS.
if(EMULATOR_BUILD)
	target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib)

	# Build step: turn a test video into the looping MPEG-1 clip the mock reads.
	# Opt-in + graceful -- only runs if EMU_TEST_VIDEO points at a real file and
	# ffmpeg is on the build host; otherwise the emulator just shows a gray FPV.
	set(EMU_TEST_VIDEO "" CACHE FILEPATH "Test video to generate the emulator's loop.mpg from")
	find_program(FFMPEG_BIN ffmpeg)
	if(EMU_TEST_VIDEO AND EXISTS "${EMU_TEST_VIDEO}" AND FFMPEG_BIN)
		add_custom_command(
			OUTPUT ${CMAKE_BINARY_DIR}/loop.mpg
			COMMAND ${FFMPEG_BIN} -y -i "${EMU_TEST_VIDEO}" -t 5 -an
			        -c:v mpeg1video -b:v 6M -s 1928x1088 -r 30
			        ${CMAKE_BINARY_DIR}/loop.mpg
			DEPENDS "${EMU_TEST_VIDEO}"
			COMMENT "Generating emulator FPV loop.mpg from ${EMU_TEST_VIDEO}")
		add_custom_target(emu_video ALL DEPENDS ${CMAKE_BINARY_DIR}/loop.mpg)
	endif()
endif()

# Per-platform POSIX compatibility headers for the desktop (SDL) emulator. Each
# target OS gets an include dir of shims for the headers it lacks, so the dormant
# device layer (serial/mount/ioctl/sockets) compiles against inert stubs. Linux
# uses the real system headers, so it needs no compat dir.
if(EMULATOR_BUILD AND WIN32)
	target_include_directories(${PROJECT_NAME} BEFORE PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src/platform/compat/windows)
	target_sources(${PROJECT_NAME} PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src/platform/compat/windows/posix_compat.c)
elseif(EMULATOR_BUILD AND APPLE)
	target_include_directories(${PROJECT_NAME} BEFORE PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src/platform/compat/macos)
endif()

if(NOT EMULATOR_BUILD)
	# record application
	file(GLOB SRC_FILES_RECORD "src/record/*.c" "src/record/*.h")

	add_executable(record
		${SRC_FILES_RECORD}
	)
	target_include_directories(record PRIVATE 
		src/record

		lib/linux/include
		lib/linux/include/video
		lib/linux/include/media
	)
	target_link_libraries(record PRIVATE
		${STANDARD_LIBRARIES}

		log
		ffmpeg
		minIni
		softwinner
	)

	# rtspLive application
	file(GLOB SRC_FILES_RTSPLIVE "src/rtspLive/*.cpp" "src/rtspLive/*.hh")
	file(GLOB SRC_FILES_RTSPLIVE_STREAM "src/rtspLive/stream/*.c" "src/rtspLive/stream/*.h")
	file(GLOB SRC_FILES_RTSPLIVE_SERVER "src/rtspLive/server/*.cpp" "src/rtspLive/server/*.hh")

	set(RTSPLIVE_BUILD_FLAGS "-DNO_OPENSSL=1 -fno-pie -no-pie")

	add_executable(rtspLive
		${SRC_FILES_RTSPLIVE}
		${SRC_FILES_RTSPLIVE_STREAM}
		${SRC_FILES_RTSPLIVE_SERVER}
	)
	target_include_directories(rtspLive PRIVATE 
		src/rtspLive
		src/rtspLive/stream
		src/rtspLive/server
	)
	set_target_properties(rtspLive PROPERTIES
		COMPILE_FLAGS ${RTSPLIVE_BUILD_FLAGS}
		LINK_FLAGS ${RTSPLIVE_BUILD_FLAGS}
	)
	target_link_libraries(rtspLive PRIVATE
		${STANDARD_LIBRARIES}

		log
		live
		minIni
	)

	# image build
	add_custom_target(MKAPP ALL
		COMMAND rsync -r ${PROJECT_SOURCE_DIR}/mkapp ${CMAKE_CURRENT_BINARY_DIR}/
	)
	add_custom_command(
		OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/out/${PROJECT_NAME}
		COMMAND size -A ${PROJECT_NAME}
		COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/out/
		COMMAND cp ${PROJECT_NAME} ${CMAKE_CURRENT_BINARY_DIR}/out/
		COMMAND cp record ${CMAKE_CURRENT_BINARY_DIR}/out/
		COMMAND cp rtspLive ${CMAKE_CURRENT_BINARY_DIR}/out/
		COMMAND cp ${PROJECT_NAME} ${OS_APP_PATH}
		COMMAND cp record ${RECORD_APP_PATH}
		COMMAND cp rtspLive ${RECORD_APP_PATH}
		COMMAND cd ${CMAKE_CURRENT_BINARY_DIR} && ./mkapp/mkapp_ota.sh
		DEPENDS MKAPP ${PROJECT_NAME} record rtspLive
	)
	add_custom_target(${PROJECT_NAME}-OTA ALL
		DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/out/${PROJECT_NAME}
	)
endif()
