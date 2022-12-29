set(SOFTWINNER_INCLUDE_DIRECTORIES
    ${CMAKE_SOURCE_DIR}/media/eyesee-mpp/system/public/include
    ${CMAKE_SOURCE_DIR}/media/eyesee-mpp/system/public/include/vo
    ${CMAKE_SOURCE_DIR}/media/eyesee-mpp/system/public/include/utils
    ${CMAKE_SOURCE_DIR}/media/eyesee-mpp/middleware/sample/configfileparser
    ${CMAKE_SOURCE_DIR}/media/eyesee-mpp/middleware/media/LIBRARY/libcedarx/libcore/common/iniparser
    ${CMAKE_SOURCE_DIR}/media/eyesee-mpp/middleware/include
    ${CMAKE_SOURCE_DIR}/media/eyesee-mpp/middleware/include/utils
    ${CMAKE_SOURCE_DIR}/media/eyesee-mpp/middleware/include/media
    ${CMAKE_SOURCE_DIR}/media/eyesee-mpp/middleware/media/include/utils
    ${CMAKE_SOURCE_DIR}/media/eyesee-mpp/middleware/media/include/component
    ${CMAKE_SOURCE_DIR}/media/eyesee-mpp/middleware/media/LIBRARY/libcedarc/include
)

add_library(softwinner SHARED IMPORTED GLOBAL)
set_target_properties(softwinner PROPERTIES
    IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/media/lib/libz.so # dummy
    INTERFACE_INCLUDE_DIRECTORIES "${SOFTWINNER_INCLUDE_DIRECTORIES}"
)
target_link_libraries(softwinner INTERFACE
    log
    ${CMAKE_SOURCE_DIR}/media/lib/eyesee-mpp/lib_ise_mo.so
    ${CMAKE_SOURCE_DIR}/media/lib/eyesee-mpp/libadecoder.so
    ${CMAKE_SOURCE_DIR}/media/lib/eyesee-mpp/libawmjpegplus.so
    ${CMAKE_SOURCE_DIR}/media/lib/eyesee-mpp/libcdc_base.so
    ${CMAKE_SOURCE_DIR}/media/lib/eyesee-mpp/libcdx_base.so
    ${CMAKE_SOURCE_DIR}/media/lib/eyesee-mpp/libcdx_common.so
    ${CMAKE_SOURCE_DIR}/media/lib/eyesee-mpp/libcdx_parser.so
    ${CMAKE_SOURCE_DIR}/media/lib/eyesee-mpp/libcdx_stream.so
    ${CMAKE_SOURCE_DIR}/media/lib/eyesee-mpp/libcedarx_aencoder.so
    ${CMAKE_SOURCE_DIR}/media/lib/eyesee-mpp/libcedarx_tencoder.so
    ${CMAKE_SOURCE_DIR}/media/lib/eyesee-mpp/libcedarxrender.so
    ${CMAKE_SOURCE_DIR}/media/lib/eyesee-mpp/libcedarxstream.so
    ${CMAKE_SOURCE_DIR}/media/lib/eyesee-mpp/libcutils.so
    ${CMAKE_SOURCE_DIR}/media/lib/eyesee-mpp/libhwdisplay.so
    ${CMAKE_SOURCE_DIR}/media/lib/eyesee-mpp/libion.so
    ${CMAKE_SOURCE_DIR}/media/lib/eyesee-mpp/libisp_ini.so
    ${CMAKE_SOURCE_DIR}/media/lib/eyesee-mpp/libISP.so
    ${CMAKE_SOURCE_DIR}/media/lib/eyesee-mpp/libmedia_mpp.so
    ${CMAKE_SOURCE_DIR}/media/lib/eyesee-mpp/libmedia_utils.so
    ${CMAKE_SOURCE_DIR}/media/lib/eyesee-mpp/libMemAdapter.so
    ${CMAKE_SOURCE_DIR}/media/lib/eyesee-mpp/libmpp_component.so
    ${CMAKE_SOURCE_DIR}/media/lib/eyesee-mpp/libmpp_ise.so
    ${CMAKE_SOURCE_DIR}/media/lib/eyesee-mpp/libmpp_isp.so
    ${CMAKE_SOURCE_DIR}/media/lib/eyesee-mpp/libmpp_vi.so
    ${CMAKE_SOURCE_DIR}/media/lib/eyesee-mpp/libmpp_vo.so
    ${CMAKE_SOURCE_DIR}/media/lib/eyesee-mpp/libsample_confparser.so
    ${CMAKE_SOURCE_DIR}/media/lib/eyesee-mpp/libvdecoder.so
    ${CMAKE_SOURCE_DIR}/media/lib/eyesee-mpp/libVE.so
    ${CMAKE_SOURCE_DIR}/media/lib/eyesee-mpp/libvenc_base.so
    ${CMAKE_SOURCE_DIR}/media/lib/eyesee-mpp/libvenc_codec.so
    ${CMAKE_SOURCE_DIR}/media/lib/eyesee-mpp/libvencoder.so
    ${CMAKE_SOURCE_DIR}/media/lib/eyesee-mpp/libvideoengine.so
    ${CMAKE_SOURCE_DIR}/media/lib/libasound.so
    ${CMAKE_SOURCE_DIR}/media/lib/libz.so
)