cmake_minimum_required(VERSION 3.10)

project(HDZGOGGLE VERSION 1.1)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(OS_APP_PATH ${PROJECT_SOURCE_DIR}/mkapp/app/app)

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
set(COMMON_COMPILE_OPTIONS
	-O3
	-mfpu=neon
	-mfloat-abi=hard
	-Wno-unused-function
	-Wno-unused-variable
	-ffunction-sections
	-fdata-sections 
	-Wl,-gc-sections
)
add_definitions(
	-DAWCHIP=AW_V5 
	-D_GNU_SOURCE 
	-DUSE_LOG_LIB_GLOG
)

# dependencies
add_subdirectory(lib/lvgl)
add_subdirectory(lib/minIni)
add_subdirectory(lib/log)
add_subdirectory(lib/esp-loader)
add_subdirectory(lib/softwinner)

# application
file(GLOB SRC_FILES_CORE   "src/core/*.c" "src/core/*.h")
file(GLOB SRC_FILES_DRIVER "src/driver/*.c" "src/driver/*.h")
file(GLOB SRC_FILES_IMAGE  "src/image/*.c" "src/image/*.h")
file(GLOB SRC_FILES_UI   "src/ui/*.c" "src/ui/*.h")
file(GLOB SRC_FILES_BMI   "src/bmi270/*.c" "src/bmi270/*.h")
file(GLOB SRC_FILES_WINDOW "src/window/*.c" "src/window/*.h")
file(GLOB SRC_FILES_PLAYER "src/player/*.c" "src/player/*.h")

add_executable(${PROJECT_NAME}
	${SRC_FILES_CORE}
	${SRC_FILES_DRIVER}
	${SRC_FILES_IMAGE}
	${SRC_FILES_UI}
	${SRC_FILES_BMI}
	${SRC_FILES_WINDOW}
	${SRC_FILES_PLAYER}
)
target_compile_options(${PROJECT_NAME} PRIVATE ${COMMON_COMPILE_OPTIONS})
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

# image build
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD 
	COMMAND size -A ${PROJECT_NAME}
	COMMAND mkdir -p ${PROJECT_SOURCE_DIR}/out/
	COMMAND cp ${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/out/
	COMMAND cp ${PROJECT_NAME} ${OS_APP_PATH}
	COMMAND cd ${PROJECT_SOURCE_DIR}/mkapp && rm -f ./ota_app/hdzgoggle_app_ota*.tar && ./mkapp_ota.sh && ./make_fw.sh
	COMMAND mv ${PROJECT_SOURCE_DIR}/mkapp/HDZERO_GOGGLE-*.bin ${PROJECT_SOURCE_DIR}/out/
)

