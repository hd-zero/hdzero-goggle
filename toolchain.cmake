set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)
 
set(TOOLS ${PROJECT_SOURCE_DIR}/toolchain)
set(CMAKE_C_COMPILER "${TOOLS}/bin/arm-openwrt-linux-muslgnueabi-gcc")
set(CMAKE_CXX_COMPILER "${TOOLS}/bin/arm-openwrt-linux-muslgnueabi-g++")
set(CMAKE_AR "${TOOLS}/bin/arm-openwrt-linux-muslgnueabi-ar")
set(CMAKE_LD "${TOOLS}/bin/arm-openwrt-linux-muslgnueabi-ld")

cmake_minimum_required(VERSION 3.13.0)
 
add_compile_options(-Wall)
