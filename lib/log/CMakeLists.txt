file(GLOB SRC_FILES "src/*.c")

add_library(log OBJECT ${SRC_FILES})
target_include_directories(log PUBLIC include)
