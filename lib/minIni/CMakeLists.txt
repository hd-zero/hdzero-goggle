file(GLOB SRC_FILES "src/*.c" "src/*.h")

add_library(minIni OBJECT ${SRC_FILES})
target_include_directories(minIni PUBLIC src)
target_compile_options(minIni PRIVATE ${COMMON_COMPILE_OPTIONS})