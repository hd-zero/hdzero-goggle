set(SOFTWINNER_INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}/include/system/public/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/system/public/include/vo
    ${CMAKE_CURRENT_SOURCE_DIR}/include/system/public/include/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/include/middleware/sample/configfileparser
    ${CMAKE_CURRENT_SOURCE_DIR}/include/middleware/media/LIBRARY/libcedarx/libcore/common/iniparser
    ${CMAKE_CURRENT_SOURCE_DIR}/include/middleware/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/middleware/include/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/include/middleware/include/media
    ${CMAKE_CURRENT_SOURCE_DIR}/include/middleware/media/include/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/include/middleware/media/include/component
    ${CMAKE_CURRENT_SOURCE_DIR}/include/middleware/media/LIBRARY/libcedarc/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/middleware/media/LIBRARY/libisp/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/middleware/media/LIBRARY/libisp/isp_tuning/
)

set(PREBUILT_LIBRARIES
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/lib_ise_mo.so
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libadecoder.so
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libawmjpegplus.so
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libcdc_base.so
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libcdx_base.so
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libcdx_common.so
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libcdx_parser.so
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libcdx_stream.so
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libcedarx_aencoder.so
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libcedarx_tencoder.so
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libcedarxrender.so
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libcedarxstream.so
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libcutils.so
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libhwdisplay.so
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libion.so
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libisp_ini.so
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libISP.so
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libmedia_mpp.so
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libmedia_utils.so
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libMemAdapter.so
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libmpp_component.so
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libmpp_ise.so
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libmpp_isp.so
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libmpp_vi.so
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libmpp_vo.so
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libsample_confparser.so
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libvdecoder.so
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libVE.so
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libvenc_base.so
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libvenc_codec.so
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libvencoder.so
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libvideoengine.so
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libasound.so
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libasound.so.2
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libasound.so.2.0.0
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libz.so
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libz.so.1
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libz.so.1.2.8
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libglog.so
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libglog.so.0
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libglog.so.0.0.0

    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libaw_aacdec.so
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libawh264.so
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libawh265.so
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libawmd.so
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libawmjpegplus.so
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libaw_mp3dec.so
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libaw_wavdec.so
)

if(NOT EMULATOR_BUILD)
    add_library(softwinner SHARED IMPORTED GLOBAL)
    set_target_properties(softwinner PROPERTIES
        IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/libz.so # dummy
        INTERFACE_INCLUDE_DIRECTORIES "${SOFTWINNER_INCLUDE_DIRECTORIES}"
    )
    target_link_libraries(softwinner INTERFACE
        log

        ${PREBUILT_LIBRARIES}
    )
else()
    add_library(softwinner INTERFACE)
    target_include_directories(softwinner INTERFACE ${SOFTWINNER_INCLUDE_DIRECTORIES})
endif()
install(FILES ${PREBUILT_LIBRARIES} TYPE LIB)