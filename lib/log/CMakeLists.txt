file(GLOB SRC_FILES "src/*.c")

add_library(log OBJECT ${SRC_FILES})
target_compile_options(log PRIVATE ${COMMON_COMPILE_OPTIONS})
target_include_directories(log PUBLIC include)
