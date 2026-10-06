cmake_minimum_required(VERSION 3.10)
project(motor_audio_poc C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_EXTENSIONS ON)
set(ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../..)

find_package(SDL2 REQUIRED)

add_executable(motor_audio_poc
	${ROOT}/src/core/motor_audio.c
	${ROOT}/src/core/motor_rpm_proto.c
	${ROOT}/src/core/msp_displayport.c
	${ROOT}/src/util/time.c
	${ROOT}/src/emulator/motor_audio_sdl.c
	${ROOT}/src/emulator/motor_audio_dev.c
	host_main.c
	host_stubs.c
)

target_include_directories(motor_audio_poc PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
	${ROOT}/src
	${ROOT}/src/core
	${ROOT}/src/emulator
	${ROOT}/src/util
	${SDL2_INCLUDE_DIRS}
)

# Prefer local host_log.h over lib/log via #include <log/log.h>
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/log)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/host_log.h ${CMAKE_CURRENT_BINARY_DIR}/log/log.h COPYONLY)
target_include_directories(motor_audio_poc PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

target_compile_definitions(motor_audio_poc PRIVATE
	EMULATOR_BUILD
	HDZGOGGLE=1
	MOTOR_AUDIO_POC_HOST=1
	_GNU_SOURCE
)

target_compile_options(motor_audio_poc PRIVATE -std=gnu99)

target_link_libraries(motor_audio_poc PRIVATE
	m
	${SDL2_LIBRARIES}
)

if(APPLE)
	# Prefer Homebrew SDL2 include path when CMake package lacks it
	execute_process(COMMAND sdl2-config --cflags OUTPUT_VARIABLE SDL2_CFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
	execute_process(COMMAND sdl2-config --libs OUTPUT_VARIABLE SDL2_LIBS OUTPUT_STRIP_TRAILING_WHITESPACE)
	separate_arguments(SDL2_CFLAGS_LIST UNIX_COMMAND "${SDL2_CFLAGS}")
	separate_arguments(SDL2_LIBS_LIST UNIX_COMMAND "${SDL2_LIBS}")
	target_compile_options(motor_audio_poc PRIVATE ${SDL2_CFLAGS_LIST})
	target_link_libraries(motor_audio_poc PRIVATE ${SDL2_LIBS_LIST})
endif()
