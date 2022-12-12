set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(TOOLS ${PROJECT_SOURCE_DIR}/toolchain/arm-linux-musleabihf-cross)

set(CMAKE_FIND_ROOT_PATH ${TOOLS})
set(CMAKE_SYSROOT ${TOOLS})

set(CMAKE_AR                        ${TOOLS}/bin/arm-linux-musleabihf-ar)
set(CMAKE_ASM_COMPILER              ${TOOLS}/bin/arm-linux-musleabihf-gcc)
set(CMAKE_C_COMPILER                ${TOOLS}/bin/arm-linux-musleabihf-gcc)
set(CMAKE_CXX_COMPILER              ${TOOLS}/bin/arm-linux-musleabihf-g++)
set(CMAKE_LINKER                    ${TOOLS}/bin/arm-linux-musleabihf-ld)
set(CMAKE_OBJCOPY                   ${TOOLS}/bin/arm-linux-musleabihf-objcopy CACHE INTERNAL "")
set(CMAKE_RANLIB                    ${TOOLS}/bin/arm-linux-musleabihf-ranlib CACHE INTERNAL "")
set(CMAKE_SIZE                      ${TOOLS}/bin/arm-linux-musleabihf-size CACHE INTERNAL "")
set(CMAKE_STRIP                     ${TOOLS}/bin/arm-linux-musleabihf-strip CACHE INTERNAL "")
