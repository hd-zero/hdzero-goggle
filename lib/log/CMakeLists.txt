file(GLOB SRC_FILES "src/*.c")

add_library(log SHARED ${SRC_FILES})
target_include_directories(log PUBLIC include)
install(TARGETS log)