set(FFMPEG_INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(PREBUILT_LIBRARIES
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libavcodec.so
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libavformat.so
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libavutil.so
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libavcodec.so.59
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libavformat.so.59
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libavutil.so.57
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libavcodec.so.59.18.100
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libavformat.so.59.16.100
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libavutil.so.57.17.100
)

add_library(ffmpeg SHARED IMPORTED GLOBAL)
set_target_properties(ffmpeg PROPERTIES
    IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/libavcodec.so # dummy
    INTERFACE_INCLUDE_DIRECTORIES "${FFMPEG_INCLUDE_DIRECTORIES}"
)
target_link_libraries(ffmpeg INTERFACE
    ${PREBUILT_LIBRARIES}
)
install(FILES ${PREBUILT_LIBRARIES} TYPE LIB)