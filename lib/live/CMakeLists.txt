set(LIVE_INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/BasicUsageEnvironment
    ${CMAKE_CURRENT_SOURCE_DIR}/include/groupsock
    ${CMAKE_CURRENT_SOURCE_DIR}/include/liveMedia
    ${CMAKE_CURRENT_SOURCE_DIR}/include/UsageEnvironment
)

set(PREBUILT_LIBRARIES
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libBasicUsageEnvironment.a
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libgroupsock.a
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libliveMedia.a
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libUsageEnvironment.a
)

add_library(live STATIC IMPORTED GLOBAL)
set_target_properties(live PROPERTIES
    IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/libliveMedia.a # dummy
    INTERFACE_INCLUDE_DIRECTORIES "${LIVE_INCLUDE_DIRECTORIES}"
)
target_link_libraries(live INTERFACE
    ${PREBUILT_LIBRARIES}
)
