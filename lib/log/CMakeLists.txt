file(GLOB SRC_FILES "src/*.c")

add_library(log-shared SHARED ${SRC_FILES})
target_include_directories(log-shared PUBLIC include)
set_target_properties(log-shared PROPERTIES OUTPUT_NAME "log")
install(TARGETS log-shared)

add_library(log OBJECT ${SRC_FILES})
target_include_directories(log PUBLIC include)
install(TARGETS log)