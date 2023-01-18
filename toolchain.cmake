cmake_minimum_required(VERSION 3.13.0)

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(TOOLS ${PROJECT_SOURCE_DIR}/toolchain)
set(CMAKE_C_COMPILER "${TOOLS}/bin/arm-openwrt-linux-muslgnueabi-gcc")
set(CMAKE_CXX_COMPILER "${TOOLS}/bin/arm-openwrt-linux-muslgnueabi-g++")
set(CMAKE_AR "${TOOLS}/bin/arm-openwrt-linux-muslgnueabi-ar")
set(CMAKE_LD "${TOOLS}/bin/arm-openwrt-linux-muslgnueabi-ld")

set(CMAKE_C_FLAGS "-Wall -O3 -mfpu=neon -mfloat-abi=hard -Wno-unused-function -Wno-unused-variable -ffunction-sections -fdata-sections -Wl,-gc-sections")

