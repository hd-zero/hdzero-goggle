/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : camera.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/06/08
  Last Modified :
  Description   : camera wrap MPP components.
  Function List :
  History       :
******************************************************************************/
#ifndef __IPCLINUX_CAMERA_H__
#define __IPCLINUX_CAMERA_H__


#ifdef __cplusplus
extern "C" {
#endif

/**
 * A set of bit masks for specifying how the received preview frames are
 * handled before the previewCallback() call.
 *
 * The least significant 3 bits of an "int" value are used for this purpose:
 *
 * ..... 0 0 0
 *       ^ ^ ^
 *       | | |---------> determine whether the callback is enabled or not
 *       | |-----------> determine whether the callback is one-shot or not
 *       |-------------> determine whether the frame is copied out or not
 *
 * WARNING: When a frame is sent directly without copying, it is the frame
 * receiver's responsiblity to make sure that the frame data won't get
 * corrupted by subsequent preview frames filled by the camera. This flag is
 * recommended only when copying out data brings significant performance price
 * and the handling/processing of the received frame data is always faster than
 * the preview frame rate so that data corruption won't occur.
 *
 * For instance,
 * 1. 0x00 disables the callback. In this case, copy out and one shot bits
 *    are ignored.
 * 2. 0x01 enables a callback without copying out the received frames. A
 *    typical use case is the Camcorder application to avoid making costly
 *    frame copies.
 * 3. 0x05 is enabling a callback with frame copied out repeatedly. A typical
 *    use case is the Camera application.
 * 4. 0x07 is enabling a callback with frame copied out only once. A typical
 *    use case is the Barcode scanner application.
 */

enum {
    CAMERA_FRAME_CALLBACK_FLAG_ENABLE_MASK = 0x01,
    CAMERA_FRAME_CALLBACK_FLAG_ONE_SHOT_MASK = 0x02,
    CAMERA_FRAME_CALLBACK_FLAG_COPY_OUT_MASK = 0x04,
    /** Typical use cases */
    CAMERA_FRAME_CALLBACK_FLAG_NOOP = 0x00,
    CAMERA_FRAME_CALLBACK_FLAG_CAMCORDER = 0x01,
    CAMERA_FRAME_CALLBACK_FLAG_CAMERA = 0x05,
    CAMERA_FRAME_CALLBACK_FLAG_BARCODE_SCANNER = 0x07
};

/** msgType in notifyCallback and dataCallback functions */
enum {
    CAMERA_MSG_ERROR =              (1 << 0),  /* notifyCallback, CAMERA_ERROR_UNKNOWN */
    CAMERA_MSG_SHUTTER =            (1 << 1),  /* notifyCallback */
    CAMERA_MSG_FOCUS =              (1 << 2),  /* notifyCallback */
    CAMERA_MSG_ZOOM =               (1 << 3),  /* notifyCallback */
    CAMERA_MSG_PREVIEW_FRAME =      (1 << 4),  /* dataCallback */
    CAMERA_MSG_VIDEO_FRAME =        (1 << 5),  /* data_timestamp_callback */
    CAMERA_MSG_POSTVIEW_FRAME =     (1 << 6),  /* dataCallback */
    CAMERA_MSG_RAW_IMAGE =          (1 << 7),  /* dataCallback */
    CAMERA_MSG_COMPRESSED_IMAGE =   (1 << 8),  /* dataCallback */
    CAMERA_MSG_RAW_IMAGE_NOTIFY =   (1 << 9),  /* dataCallback */
    // Preview frame metadata. This can be combined with
    // CAMERA_MSG_PREVIEW_FRAME in dataCallback. For example, the apps can
    // request FRAME and METADATA. Or the apps can request only FRAME or only
    // METADATA.
    CAMERA_MSG_PREVIEW_METADATA =   (1 << 10),  /* dataCallback */
    // Notify on autofocus start and stop. This is useful in continuous
    // autofocus - FOCUS_MODE_CONTINUOUS_VIDEO and FOCUS_MODE_CONTINUOUS_PICTURE.
    CAMERA_MSG_FOCUS_MOVE =         (1 << 11),  /* notifyCallback */

    CAMERA_MSG_CONTINUOUSSNAP=      (1 << 12),  /* notifyCallback for continuous snap */
    CAMERA_MSG_SNAP=                (1 << 13),  /* notifyCallback of setting camera idle  for single snap */
    CAMERA_MSG_SNAP_THUMB=          (1 << 14),  /* notifyCallback of saving thumb for single snap */

    CAMERA_MSG_AWMD =               (1 << 15),	/* MOTION_DETECTION_ENABLE */
    CAMERA_MSG_ADAS_METADATA =      (1 << 16),	/* for ADAS */
    CAMERA_MSG_QRDECODE =           (1 << 17),	/* MOTION_DETECTION_ENABLE */
    CAMERA_MSG_INFO =               (1 << 18),  /* notifyCallback,  CameraMsgInfoType*/
    CAMERA_MSG_MOD_DATA =           (1 << 19),  /* MOD data:AW_AI_CVE_DTCA_RESULT_S, dataCallback*/
    CAMERA_MSG_FACEDETECT_DATA =    (1 << 20),  /* face detect data:AW_AI_EVE_EVENT_RESULT_S, dataCallback*/
    CAMERA_MSG_VLPR_DATA =          (1 << 21),  /* VLPR data:AW_AI_CVE_VLPR_RULT_S, dataCallback*/
//by andy
    CAMERA_MSG_BDII_DATA =          (1 << 22),  /* VLPR data:CVE_BDII_RES_OUT_S, dataCallback*/

    CAMERA_MSG_ALL_MSGS =           (~(1 << 23))
};

/** cmdType in sendCommand functions */
enum {
    CAMERA_CMD_START_SMOOTH_ZOOM = 1,
    CAMERA_CMD_STOP_SMOOTH_ZOOM = 2,

    /**
     * Set the clockwise rotation of preview display (setPreviewDisplay) in
     * degrees. This affects the preview frames and the picture displayed after
     * snapshot. This method is useful for portrait mode applications. Note
     * that preview display of front-facing cameras is flipped horizontally
     * before the rotation, that is, the image is reflected along the central
     * vertical axis of the camera sensor. So the users can see themselves as
     * looking into a mirror.
     *
     * This does not affect the order of byte array of
     * CAMERA_MSG_PREVIEW_FRAME, CAMERA_MSG_VIDEO_FRAME,
     * CAMERA_MSG_POSTVIEW_FRAME, CAMERA_MSG_RAW_IMAGE, or
     * CAMERA_MSG_COMPRESSED_IMAGE. This is allowed to be set during preview
     * since API level 14.
     */
    CAMERA_CMD_SET_DISPLAY_ORIENTATION = 3,

    /**
     * cmdType to disable/enable shutter sound. In sendCommand passing arg1 =
     * 0 will disable, while passing arg1 = 1 will enable the shutter sound.
     */
    CAMERA_CMD_ENABLE_SHUTTER_SOUND = 4,

    /* cmdType to play recording sound */
    CAMERA_CMD_PLAY_RECORDING_SOUND = 5,

    /**
     * Start the face detection. This should be called after preview is started.
     * The camera will notify the listener of CAMERA_MSG_FACE and the detected
     * faces in the preview frame. The detected faces may be the same as the
     * previous ones. Apps should call CAMERA_CMD_STOP_FACE_DETECTION to stop
     * the face detection. This method is supported if CameraParameters
     * KEY_MAX_NUM_HW_DETECTED_FACES or KEY_MAX_NUM_SW_DETECTED_FACES is
     * bigger than 0. Hardware and software face detection should not be running
     * at the same time. If the face detection has started, apps should not send
     * this again.
     *
     * In hardware face detection mode, CameraParameters KEY_WHITE_BALANCE,
     * KEY_FOCUS_AREAS and KEY_METERING_AREAS have no effect.
     *
     * arg1 is the face detection type. It can be CAMERA_FACE_DETECTION_HW or
     * CAMERA_FACE_DETECTION_SW. If the type of face detection requested is not
     * supported, the HAL must return BAD_VALUE.
     */
    CAMERA_CMD_START_FACE_DETECTION = 6,

    /**
     * Stop the face detection.
     */
    CAMERA_CMD_STOP_FACE_DETECTION = 7,

    /**
     * Enable/disable focus move callback (CAMERA_MSG_FOCUS_MOVE). Passing
     * arg1 = 0 will disable, while passing arg1 = 1 will enable the callback.
     */
    CAMERA_CMD_ENABLE_FOCUS_MOVE_MSG = 8,

    /**
     * Ping camera service to see if camera hardware is released.
     *
     * When any camera method returns error, the client can use ping command
     * to see if the camera has been taken away by other clients. If the result
     * is NO_ERROR, it means the camera hardware is not released. If the result
     * is not NO_ERROR, the camera has been released and the existing client
     * can silently finish itself or show a dialog.
     */
    CAMERA_CMD_PING = 9,
    

    /**
     * Configure the number of video buffers used for recording. The intended
     * video buffer count for recording is passed as arg1, which must be
     * greater than 0. This command must be sent before recording is started.
     * This command returns INVALID_OPERATION error if it is sent after video
     * recording is started, or the command is not supported at all. This
     * command also returns a BAD_VALUE error if the intended video buffer
     * count is non-positive or too big to be realized.
     */
    CAMERA_CMD_SET_VIDEO_BUFFER_COUNT = 10,

	/* MOTION_DETECTION_ENABLE start */
    CAMERA_CMD_START_AWMD = 11,
    CAMERA_CMD_STOP_AWMD = 12,
    CAMERA_CMD_SET_AWMD_SENSITIVITY_LEVEL = 13,
	/* MOTION_DETECTION_ENABLE end */

	/* for ADAS start */
	CAMERA_CMD_ADAS_START_DETECTION = 14,
	CAMERA_CMD_ADAS_STOP_DETECTION = 15,
	CAMERA_CMD_ADAS_SET_LANELINE_OFFSET_SENSITY = 16,
	CAMERA_CMD_ADAS_SET_DISTANCE_DETECT_LEVEL = 17,
	CAMERA_CMD_ADAS_SET_CAR_SPEED = 18,
	CAMERA_CMD_ADAS_AUDIO_VOL = 19,
	CAMERA_CMD_ADAS_MODE = 20,
	CAMERA_CMD_ADAS_TIPS_MODE = 21,
	/* for ADAS end */

    CAMERA_CMD_START_RENDER = 22,
    CAMERA_CMD_STOP_RENDER = 23,

	CAMERA_CMD_SET_COLORFX = 24,

	CAMERA_CMD_SET_QRDECODE = 25,

    CAMERA_CMD_CANCEL_CONTINUOUS_SNAP = 26,

    CAMERA_CMD_SET_SCREEN_ID = 0xFF000000,
	CAMERA_CMD_SET_CEDARX_RECORDER = 0xFF000001,
};

/** camera fatal errors */
typedef enum {
    CAMERA_ERROR_UNKNOWN = 1,
    /**
     * Camera was released because another client has connected to the camera.
     * The original client should call Camera::disconnect immediately after
     * getting this notification. Otherwise, the camera will be released by
     * camera service in a short time. The client should not call any method
     * (except disconnect and sending CAMERA_CMD_PING) after getting this.
     */
    CAMERA_ERROR_RELEASED = 2,

    CAMERA_ERROR_SELECT_TIMEOUT = 3,
    CAMERA_ERROR_PTS_CHANGE = 4,
    CAMERA_ERROR_TAKE_PIC_FAIL = 5, // added for notifying failure of taking picture

    CAMERA_ERROR_SERVER_DIED = 100,
}CameraMsgErrorType;

/** camera msg info  */
typedef enum {
    /**
     * notify app that camera has take first frame and render it.
     */
    CAMERA_INFO_RENDERING_START = 1,
    
    CAMERA_INFO_BUTT,
}CameraMsgInfoType;

enum {
    /** The facing of the camera is opposite to that of the screen. */
    CAMERA_FACING_BACK = 0,
    /** The facing of the camera is the same as that of the screen. */
    CAMERA_FACING_FRONT = 1
};

enum {
    CAMERA_WATERMARK_DISABLE = 0,
    CAMERA_WATERMARK_ALL,
    CAMERA_WATERMARK_ONLY_MAIN_CH,
    CAMERA_WATERMARK_ONLY_SUB_CH,
    CAMERA_WATERMARK_LIST_END,
};

#ifdef __cplusplus
}
#endif

#endif /* __IPCLINUX_CAMERA_H__ */


