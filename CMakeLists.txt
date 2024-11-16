cmake_minimum_required(VERSION 3.10)

project(HDZGOGGLE VERSION 1.1)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(COMMON_COMPILER_FLAGS "-Wno-unused-function -Wno-unused-variable -Wno-deprecated-declarations -ffunction-sections -fdata-sections -Wl,-gc-sections -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64")

option(EMULATOR_BUILD "emulate using SDL on host" OFF)
if(NOT EMULATOR_BUILD)
	set(COMMON_COMPILER_FLAGS "${COMMON_COMPILER_FLAGS} -mfpu=neon -mfloat-abi=hard")
endif()


set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_COMPILER_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_COMPILER_FLAGS}")

set(CMAKE_C_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_DEBUG "-g")

set(CMAKE_C_FLAGS_RELEASE "-Werror -O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-Werror -O3 -DNDEBUG")

set(OS_APP_PATH ${PROJECT_SOURCE_DIR}/mkapp/app/app)
set(RECORD_APP_PATH ${PROJECT_SOURCE_DIR}/mkapp/app/app/record)

# common build settings
set(STANDARD_LIBRARIES
	c
	crypt
	dl
	m
	pthread
	rt
	stdc++
)

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
file(GLOB SRC_FILES_IMAGE  "src/image/*.c" "src/image/*.h")
file(GLOB SRC_FILES_UI   "src/ui/*.c" "src/ui/*.h")
file(GLOB SRC_FILES_BMI   "src/bmi270/*.c" "src/bmi270/*.h")
file(GLOB SRC_FILES_UTIL "src/util/*.c" "src/util/*.h")
file(GLOB SRC_FILES_PLAYER "src/player/*.c" "src/player/*.h")
file(GLOB SRC_FILES_EMULATOR "src/emulator/*.c" "src/emulator/*.h")
file(GLOB SRC_FILES_LANG "src/lang/*.c" "src/lang/*.h")

set(SRC_FILES
	${SRC_FILES_CORE}
	${SRC_FILES_DRIVER}
	${SRC_FILES_FONTS}
	${SRC_FILES_IMAGE}
	${SRC_FILES_UI}
	${SRC_FILES_BMI}
	${SRC_FILES_UTIL}
	${SRC_FILES_LANG}
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
	target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_LIBRARIES})
endif()

if(NOT EMULATOR_BUILD)
	# record application
	file(GLOB SRC_FILES_RECORD   "src/record/*.c" "src/record/*.h")

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
	add_custom_command(
		OUTPUT ${PROJECT_SOURCE_DIR}/out/${PROJECT_NAME}
		COMMAND size -A ${PROJECT_NAME}
		COMMAND mkdir -p ${PROJECT_SOURCE_DIR}/out/
		COMMAND cp ${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/out/
		COMMAND cp record ${PROJECT_SOURCE_DIR}/out/
		COMMAND cp rtspLive ${PROJECT_SOURCE_DIR}/out/
		COMMAND cp ${PROJECT_NAME} ${OS_APP_PATH}
		COMMAND cp record ${RECORD_APP_PATH}
		COMMAND cp rtspLive ${RECORD_APP_PATH}
		COMMAND cd ${PROJECT_SOURCE_DIR} && ./mkapp/mkapp_ota.sh
		DEPENDS ${PROJECT_NAME} record rtspLive
	)
	add_custom_target(${PROJECT_NAME}-OTA ALL
		DEPENDS ${PROJECT_SOURCE_DIR}/out/${PROJECT_NAME}
	)
endif()
