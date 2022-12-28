file(GLOB SRC_FILES "src/*.c")

add_library(esp-loader OBJECT ${SRC_FILES})
target_compile_definitions(esp-loader PUBLIC -DMD5_ENABLED=1)
target_compile_options(esp-loader PRIVATE ${COMMON_COMPILE_OPTIONS})
target_include_directories(esp-loader PUBLIC include)
target_include_directories(esp-loader PRIVATE private_include)