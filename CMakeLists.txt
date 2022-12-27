cmake_minimum_required(VERSION 3.10)

project(HDZGOGGLE VERSION 1.1)

set(OS_APP_PATH ${PROJECT_SOURCE_DIR}/mkapp/app/app)

FILE(GLOB SRC_FILES_CORE   "src/core/*.c" "src/core/*.h")
FILE(GLOB SRC_FILES_PLAYER "src/player/*.c" "src/player/*.h")
FILE(GLOB SRC_FILES_DRIVER "src/driver/*.c" "src/driver/*.h")
FILE(GLOB SRC_FILES_ESP32  "src/esp32/*.c" "src/esp32/*.h")
FILE(GLOB SRC_FILES_IMAGE  "src/image/*.c" "src/image/*.h")
FILE(GLOB SRC_FILES_PAGE   "src/page/*.c" "src/page/*.h")
FILE(GLOB SRC_FILES_BMI   "src/bmi270/*.c" "src/bmi270/*.h")
FILE(GLOB SRC_FILES_WINDOW "src/window/*.c" "src/window/*.h")
FILE(GLOB SRC_FILES_MINIINI   "src/minIni/minIni.c" "src/minIni/*.h")

add_library(media ${SRC_FILES_PLAYER})
add_executable(${PROJECT_NAME}
				${SRC_FILES_CORE}
				${SRC_FILES_DRIVER}
				${SRC_FILES_IMAGE}
				${SRC_FILES_ESP32}
				${SRC_FILES_PAGE}
				${SRC_FILES_BMI}
				${SRC_FILES_WINDOW}
				${SRC_FILES_MINIINI}
				)

target_compile_options(media PRIVATE
    -lsample_confparser
	-lc
   	-lstdc++
   	-lrt
   	-lpthread
   	-lm
   	-lcrypt
	-DUSE_LOG_LIB_GLOG
	-DAWCHIP=AW_V5
	-D_GNU_SOURCE
	-ldl
	-lrt
	-lm
	-lstdc++
	-lpthread
	-lz
	-llog
	-lglog
	-lion
	-lmpp_vi
	-lmpp_isp
	-lmpp_ise
	-lmpp_vo
	-lmpp_component
	-lmedia_mpp
	-lvencoder
	-lvenc_codec
	-lvenc_base
	-lISP
	-lisp_dev
	-lisp_ini
	-lisp_iso
	-lVE
	-lMemAdapter
	-lmedia_utils
	-lcedarxrender
	-lcedarxstream
	-lcedarx_tencoder
	-lcdx_common
	-lcdx_base
	-lcdx_parser
	-lcdx_stream
	-lhwdisplay
	-lasound
	-lcutils
	-liniparser
	-law_mpp
	-lcedarx_aencoder
	-laacenc
	-lmp3enc
	-lawmjpegplus
	-ladecoder
	-lvdecoder
	-lvideoengine
	-lcdc_base
	-l_ise_mo
	-Lsrc/player/media/lib
	-Lsrc/player/media/lib/eyesee-mpp
	)

target_compile_options(${PROJECT_NAME} PRIVATE -O3
  	-march=armv7-a
   	-mtune=cortex-a7
   	-mfpu=neon
 	-mfloat-abi=hard
 	-Wno-unused-function
   	-Wno-unused-variable
	-ffunction-sections -fdata-sections -Wl,-gc-sections
	)

target_include_directories(media PRIVATE media/eyesee-mpp/system/public/include )
target_include_directories(media PRIVATE media/eyesee-mpp/system/public/include/vo )
target_include_directories(media PRIVATE media/eyesee-mpp/system/public/include/utils )
target_include_directories(media PRIVATE media/eyesee-mpp/middleware/sample/configfileparser )
target_include_directories(media PRIVATE media/eyesee-mpp/middleware/media/LIBRARY/libcedarx/libcore/common/iniparser )
target_include_directories(media PRIVATE media/eyesee-mpp/middleware/include )
target_include_directories(media PRIVATE media/eyesee-mpp/middleware/include/utils )
target_include_directories(media PRIVATE media/eyesee-mpp/middleware/include/media )
target_include_directories(media PRIVATE media/eyesee-mpp/middleware/media/include/utils )
target_include_directories(media PRIVATE media/eyesee-mpp/middleware/media/include/component )
target_include_directories(media PRIVATE media/eyesee-mpp/middleware/media/LIBRARY/libcedarc/include )
target_include_directories(media PRIVATE linux-4.9/include )
target_include_directories(media PRIVATE linux-4.9/include/video )

target_include_directories(${PROJECT_NAME} PRIVATE src/)
target_include_directories(${PROJECT_NAME} PRIVATE src/core)
target_include_directories(${PROJECT_NAME} PRIVATE src/driver)
target_include_directories(${PROJECT_NAME} PRIVATE src/page)
target_include_directories(${PROJECT_NAME} PRIVATE src/player)
target_include_directories(${PROJECT_NAME} PRIVATE src/player/media/app)
target_include_directories(${PROJECT_NAME} PRIVATE src/window)

target_link_libraries(${PROJECT_NAME}
	liblvgl.a
	media
	m
	pthread
    sample_confparser
	c
   	stdc++
   	rt
   	pthread
   	m
   	crypt
	dl
	rt
	m
	stdc++
	pthread
	z
	log
	glog
	ion
	mpp_vi
	mpp_isp
	mpp_ise
	mpp_vo
	mpp_component
	media_mpp
	vencoder
	venc_codec
	venc_base
	ISP
	isp_dev
	isp_ini
	isp_iso
	VE
	MemAdapter
	media_utils
	cedarxrender
	cedarxstream
	cedarx_tencoder
	cdx_common
	cdx_base
	cdx_parser
	cdx_stream
	hwdisplay
	asound
	cutils
	iniparser
	aw_mpp
	cedarx_aencoder
	aacenc
	mp3enc
	awmjpegplus
	adecoder
	vdecoder
	videoengine
	cdc_base
	_ise_mo
	)

target_link_directories(${PROJECT_NAME} PRIVATE
	build
	src/lvgl/build/lib
	media/lib
	media/lib/eyesee-mpp
	)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
											COMMAND size -A ${PROJECT_NAME}
											COMMAND mkdir -p ${PROJECT_SOURCE_DIR}/out/
											COMMAND cp ${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/out/
											COMMAND cp ${PROJECT_NAME} ${OS_APP_PATH}
											COMMAND cd ${PROJECT_SOURCE_DIR}/mkapp && rm -f ./ota_app/hdzgoggle_app_ota*.tar && ./mkapp_ota && ./make_fw.sh
											COMMAND mv ${PROJECT_SOURCE_DIR}/mkapp/HDZERO_GOGGLE-*.bin ${PROJECT_SOURCE_DIR}/out/
											)
