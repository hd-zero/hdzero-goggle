#ifndef CDX_ERRORTYPE_H
#define CDX_ERRORTYPE_H


enum {
    CDX_OK     					= 0,
    CDX_ERROR 					= -1,
    CDX_ERROR_UNKNOWN 			= -1,

    CDX_NO_NATIVE_WINDOW = 10,
    CDX_ERROR_UNSUPPORT 		= -100,
    CDX_ERROR_UNSUPPORT_USESFT  = -101,
    CDX_ERROR_STOP_PLAYER		= -102,
    CDX_ERROR_UNSUPPORT_RAWMUSIC = -103,
	CDX_ERROR_OUT_OF_MEMORY      = -104,

    /*
     * enum below is copied from MediaErrors.h
     */
    /*
    MEDIA_ERROR_BASE        = -1000,

    ERROR_ALREADY_CONNECTED = MEDIA_ERROR_BASE,
    ERROR_NOT_CONNECTED     = MEDIA_ERROR_BASE - 1,
    ERROR_UNKNOWN_HOST      = MEDIA_ERROR_BASE - 2,
    ERROR_CANNOT_CONNECT    = MEDIA_ERROR_BASE - 3,
    ERROR_IO                = MEDIA_ERROR_BASE - 4,
    ERROR_CONNECTION_LOST   = MEDIA_ERROR_BASE - 5,
    ERROR_MALFORMED         = MEDIA_ERROR_BASE - 7,
    ERROR_OUT_OF_RANGE      = MEDIA_ERROR_BASE - 8,
    ERROR_BUFFER_TOO_SMALL  = MEDIA_ERROR_BASE - 9,
    ERROR_UNSUPPORTED       = MEDIA_ERROR_BASE - 10,
    ERROR_END_OF_STREAM     = MEDIA_ERROR_BASE - 11,

    // Not technically an error.
    INFO_FORMAT_CHANGED    = MEDIA_ERROR_BASE - 12,
    INFO_DISCONTINUITY     = MEDIA_ERROR_BASE - 13,
    INFO_OUTPUT_BUFFERS_CHANGED = MEDIA_ERROR_BASE - 14,
    INFO_VENDOR_LEAF_ATOM  = -1500,

    // The following constant values should be in sync with
    // drm/drm_framework_common.h
    DRM_ERROR_BASE = -2000,

    ERROR_DRM_UNKNOWN                       = DRM_ERROR_BASE,
    ERROR_DRM_NO_LICENSE                    = DRM_ERROR_BASE - 1,
    ERROR_DRM_LICENSE_EXPIRED               = DRM_ERROR_BASE - 2,
    ERROR_DRM_SESSION_NOT_OPENED            = DRM_ERROR_BASE - 3,
    ERROR_DRM_DECRYPT_UNIT_NOT_INITIALIZED  = DRM_ERROR_BASE - 4,
    ERROR_DRM_DECRYPT                       = DRM_ERROR_BASE - 5,
    ERROR_DRM_CANNOT_HANDLE                 = DRM_ERROR_BASE - 6,
    ERROR_DRM_TAMPER_DETECTED               = DRM_ERROR_BASE - 7,

    ERROR_DRM_VENDOR_MAX                    = DRM_ERROR_BASE - 500,
    ERROR_DRM_VENDOR_MIN                    = DRM_ERROR_BASE - 999,

    // Deprecated
    ERROR_DRM_WV_VENDOR_MAX                 = ERROR_DRM_VENDOR_MAX,
    ERROR_DRM_WV_VENDOR_MIN                 = ERROR_DRM_VENDOR_MIN,

    // Heartbeat Error Codes
    HEARTBEAT_ERROR_BASE = -3000,
    ERROR_HEARTBEAT_TERMINATE_REQUESTED                     = HEARTBEAT_ERROR_BASE,
    */
};
#endif
