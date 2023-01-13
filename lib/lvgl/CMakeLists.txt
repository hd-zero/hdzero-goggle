file(GLOB_RECURSE SRC_FILES_LVGL "lvgl/*.c")

add_library(lvgl OBJECT ${SRC_FILES_LVGL})
target_include_directories(lvgl PUBLIC .)
target_include_directories(lvgl PRIVATE src)
