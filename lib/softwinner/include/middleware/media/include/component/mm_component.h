/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : mm_component.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/03/15
  Last Modified :
  Description   : The common defination for multimedia component.
  Function List :
  History       :
******************************************************************************/

#ifndef __IPCLINUX_MM_COMPONENT_H__
#define __IPCLINUX_MM_COMPONENT_H__

//ref platform headers
#include "plat_type.h"
#include "plat_errno.h"

//media api headers to app
#include "mm_common.h"
#include "mm_comm_video.h"

#ifdef __cplusplus
extern "C" {
#endif /* End of #ifdef __cplusplus */

typedef void*   COMP_HANDLETYPE;

typedef enum COMP_STATETYPE
{
    COMP_StateInvalid,      /**< component has detected that it's internal data
                                structures are corrupted to the point that
                                it cannot determine it's state properly */
    COMP_StateLoaded,      /**< component has been loaded but has not completed
                                initialization.  The OMX_SetParameter macro
                                and the OMX_GetParameter macro are the only
                                valid macros allowed to be sent to the
                                component in this state. */
    COMP_StateIdle,        /**< component initialization has been completed
                                successfully and the component is ready to
                                to start. */
    COMP_StateExecuting,   /**< component has accepted the start command and
                                is processing data (if data is available) */
    COMP_StatePause,       /**< component has received pause command */
    COMP_StateWaitForResources, /**< component is waiting for resources, either after
                                preemption or before it gets the resources requested.
                                See specification for complete details. */
    COMP_StateKhronosExtensions = 0x6F000000, /**< Reserved region for introducing Khronos Standard Extensions */
    COMP_StateVendorStartUnused = 0x7F000000, /**< Reserved region for introducing Vendor Extensions */
    COMP_StateMax = 0X7FFFFFFF
} COMP_STATETYPE;

/** The COMP_COMMANDTYPE enumeration is used to specify the action in the
 *  OMX_SendCommand macro.
 *  @ingroup core
 */
typedef enum COMP_COMMANDTYPE
{
    COMP_CommandStateSet,    /**< Change the component state */
    COMP_CommandFlush,       /**< Flush the data queue(s) of a component */
    COMP_CommandPortDisable, /**< Disable a port on a component. */
    COMP_CommandPortEnable,  /**< Enable a port on a component. */
    COMP_CommandMarkBuffer,  /**< Mark a component/buffer for observation */
    COMP_CommandKhronosExtensions = 0x6F000000, /**< Reserved region for introducing Khronos Standard Extensions */
    COMP_CommandVendorStartUnused = 0x7F000000, /**< Reserved region for introducing Vendor Extensions */
    COMP_CommandVendorAddChn,    /**< add a mux channel of recRender,  MUX_CHN_ATTR_S */
    COMP_CommandVendorRemoveChn, /**< remove a mux channel of recRender, chnId */
	COMP_CommandSwitchFile,  //impact switch file, when cache exist: immediately; when cache not exist: smooth.
	COMP_CommandSwitchFileNormal, //normal switch file. cache not exist: smooth.
    COMP_CommandVendorFillBuffer,    //fill audio buffer from audioSink.
	COMP_CommandVendorReturnAllBuffer, //return all buffers which come from other component.
	COMP_CommandVendorChangeANativeWindow,

    COMP_CommandMax = 0X7FFFFFFF
} COMP_COMMANDTYPE;

/** The COMP_INDEXTYPE enumeration is used to select a structure when either
 *  getting or setting parameters and/or configuration data.  Each entry in
 *  this enumeration maps to an OMX specified structure.  When the
 *  OMX_GetParameter, OMX_SetParameter, OMX_GetConfig or OMX_SetConfig methods
 *  are used, the second parameter will always be an entry from this enumeration
 *  and the third entry will be the structure shown in the comments for the entry.
 *  For example, if the application is initializing a cropping function, the
 *  OMX_SetConfig command would have OMX_IndexConfigCommonInputCrop as the second parameter
 *  and would send a pointer to an initialized OMX_RECTTYPE structure as the
 *  third parameter.
 *
 *  The enumeration entries named with the OMX_Config prefix are sent using
 *  the OMX_SetConfig command and the enumeration entries named with the
 *  OMX_PARAM_ prefix are sent using the OMX_SetParameter command.
 */
typedef enum COMP_INDEXTYPE {

    COMP_IndexComponentStartUnused = 0x01000000,
    COMP_IndexParamPriorityMgmt,             /**< reference: OMX_PRIORITYMGMTTYPE */
    COMP_IndexParamAudioInit,                /**< reference: OMX_PORT_PARAM_TYPE */
    COMP_IndexParamImageInit,                /**< reference: OMX_PORT_PARAM_TYPE */
    COMP_IndexParamVideoInit,                /**< reference: OMX_PORT_PARAM_TYPE */
    COMP_IndexParamOtherInit,                /**< reference: OMX_PORT_PARAM_TYPE */
    COMP_IndexParamNumAvailableStreams,      /**< reference: OMX_PARAM_U32TYPE */
    COMP_IndexParamActiveStream,             /**< reference: OMX_PARAM_U32TYPE */
    COMP_IndexParamSuspensionPolicy,         /**< reference: OMX_PARAM_SUSPENSIONPOLICYTYPE */
    COMP_IndexParamComponentSuspended,       /**< reference: OMX_PARAM_SUSPENSIONTYPE */
    COMP_IndexConfigCapturing,               /**< reference: OMX_CONFIG_BOOLEANTYPE */
    COMP_IndexConfigCaptureMode,             /**< reference: OMX_CONFIG_CAPTUREMODETYPE */
    COMP_IndexAutoPauseAfterCapture,         /**< reference: OMX_CONFIG_BOOLEANTYPE */
    COMP_IndexParamContentURI,               /**< reference: OMX_PARAM_CONTENTURITYPE */
    COMP_IndexParamCustomContentPipe,        /**< reference: OMX_PARAM_CONTENTPIPETYPE */
    COMP_IndexParamDisableResourceConcealment, /**< reference: OMX_RESOURCECONCEALMENTTYPE */
    COMP_IndexConfigMetadataItemCount,       /**< reference: OMX_CONFIG_METADATAITEMCOUNTTYPE */
    COMP_IndexConfigContainerNodeCount,      /**< reference: OMX_CONFIG_CONTAINERNODECOUNTTYPE */
    COMP_IndexConfigMetadataItem,            /**< reference: OMX_CONFIG_METADATAITEMTYPE */
    COMP_IndexConfigCounterNodeID,           /**< reference: OMX_CONFIG_CONTAINERNODEIDTYPE */
    COMP_IndexParamMetadataFilterType,       /**< reference: OMX_PARAM_METADATAFILTERTYPE */
    COMP_IndexParamMetadataKeyFilter,        /**< reference: OMX_PARAM_METADATAFILTERTYPE */
    COMP_IndexConfigPriorityMgmt,            /**< reference: OMX_PRIORITYMGMTTYPE */
    COMP_IndexParamStandardComponentRole,    /**< reference: OMX_PARAM_COMPONENTROLETYPE */

    COMP_IndexPortStartUnused = 0x02000000,
    COMP_IndexParamPortDefinition,           /**< reference: OMX_PARAM_PORTDEFINITIONTYPE */
    COMP_IndexParamCompBufferSupplier,       /**< reference: OMX_PARAM_BUFFERSUPPLIERTYPE */
    COMP_IndexReservedStartUnused = 0x03000000,

    /* Audio parameters and configurations */
    COMP_IndexAudioStartUnused = 0x04000000,
    COMP_IndexParamAudioPortFormat,          /**< reference: OMX_AUDIO_PARAM_PORTFORMATTYPE */
    COMP_IndexParamAudioPcm,                 /**< reference: OMX_AUDIO_PARAM_PCMMODETYPE */
    COMP_IndexParamAudioAac,                 /**< reference: OMX_AUDIO_PARAM_AACPROFILETYPE */
    COMP_IndexParamAudioRa,                  /**< reference: OMX_AUDIO_PARAM_RATYPE */
    COMP_IndexParamAudioMp3,                 /**< reference: OMX_AUDIO_PARAM_MP3TYPE */
    COMP_IndexParamAudioAdpcm,               /**< reference: OMX_AUDIO_PARAM_ADPCMTYPE */
    COMP_IndexParamAudioG723,                /**< reference: OMX_AUDIO_PARAM_G723TYPE */
    COMP_IndexParamAudioG729,                /**< reference: OMX_AUDIO_PARAM_G729TYPE */
    COMP_IndexParamAudioAmr,                 /**< reference: OMX_AUDIO_PARAM_AMRTYPE */
    COMP_IndexParamAudioWma,                 /**< reference: OMX_AUDIO_PARAM_WMATYPE */
    COMP_IndexParamAudioSbc,                 /**< reference: OMX_AUDIO_PARAM_SBCTYPE */
    COMP_IndexParamAudioMidi,                /**< reference: OMX_AUDIO_PARAM_MIDITYPE */
    COMP_IndexParamAudioGsm_FR,              /**< reference: OMX_AUDIO_PARAM_GSMFRTYPE */
    COMP_IndexParamAudioMidiLoadUserSound,   /**< reference: OMX_AUDIO_PARAM_MIDILOADUSERSOUNDTYPE */
    COMP_IndexParamAudioG726,                /**< reference: OMX_AUDIO_PARAM_G726TYPE */
    COMP_IndexParamAudioGsm_EFR,             /**< reference: OMX_AUDIO_PARAM_GSMEFRTYPE */
    COMP_IndexParamAudioGsm_HR,              /**< reference: OMX_AUDIO_PARAM_GSMHRTYPE */
    COMP_IndexParamAudioPdc_FR,              /**< reference: OMX_AUDIO_PARAM_PDCFRTYPE */
    COMP_IndexParamAudioPdc_EFR,             /**< reference: OMX_AUDIO_PARAM_PDCEFRTYPE */
    COMP_IndexParamAudioPdc_HR,              /**< reference: OMX_AUDIO_PARAM_PDCHRTYPE */
    COMP_IndexParamAudioTdma_FR,             /**< reference: OMX_AUDIO_PARAM_TDMAFRTYPE */
    COMP_IndexParamAudioTdma_EFR,            /**< reference: OMX_AUDIO_PARAM_TDMAEFRTYPE */
    COMP_IndexParamAudioQcelp8,              /**< reference: OMX_AUDIO_PARAM_QCELP8TYPE */
    COMP_IndexParamAudioQcelp13,             /**< reference: OMX_AUDIO_PARAM_QCELP13TYPE */
    COMP_IndexParamAudioEvrc,                /**< reference: OMX_AUDIO_PARAM_EVRCTYPE */
    COMP_IndexParamAudioSmv,                 /**< reference: OMX_AUDIO_PARAM_SMVTYPE */
    COMP_IndexParamAudioVorbis,              /**< reference: OMX_AUDIO_PARAM_VORBISTYPE */
    COMP_IndexParamAudioFlac,                /**< reference: OMX_AUDIO_PARAM_FLACTYPE */

    COMP_IndexConfigAudioMidiImmediateEvent, /**< reference: OMX_AUDIO_CONFIG_MIDIIMMEDIATEEVENTTYPE */
    COMP_IndexConfigAudioMidiControl,        /**< reference: OMX_AUDIO_CONFIG_MIDICONTROLTYPE */
    COMP_IndexConfigAudioMidiSoundBankProgram, /**< reference: OMX_AUDIO_CONFIG_MIDISOUNDBANKPROGRAMTYPE */
    COMP_IndexConfigAudioMidiStatus,         /**< reference: OMX_AUDIO_CONFIG_MIDISTATUSTYPE */
    COMP_IndexConfigAudioMidiMetaEvent,      /**< reference: OMX_AUDIO_CONFIG_MIDIMETAEVENTTYPE */
    COMP_IndexConfigAudioMidiMetaEventData,  /**< reference: OMX_AUDIO_CONFIG_MIDIMETAEVENTDATATYPE */
    COMP_IndexConfigAudioVolume,             /**< reference: OMX_AUDIO_CONFIG_VOLUMETYPE */
    COMP_IndexConfigAudioBalance,            /**< reference: OMX_AUDIO_CONFIG_BALANCETYPE */
    COMP_IndexConfigAudioChannelMute,        /**< reference: OMX_AUDIO_CONFIG_CHANNELMUTETYPE */
    COMP_IndexConfigAudioMute,               /**< reference: OMX_AUDIO_CONFIG_MUTETYPE */
    COMP_IndexConfigAudioLoudness,           /**< reference: OMX_AUDIO_CONFIG_LOUDNESSTYPE */
    COMP_IndexConfigAudioEchoCancelation,    /**< reference: OMX_AUDIO_CONFIG_ECHOCANCELATIONTYPE */
    COMP_IndexConfigAudioNoiseReduction,     /**< reference: OMX_AUDIO_CONFIG_NOISEREDUCTIONTYPE */
    COMP_IndexConfigAudioBass,               /**< reference: OMX_AUDIO_CONFIG_BASSTYPE */
    COMP_IndexConfigAudioTreble,             /**< reference: OMX_AUDIO_CONFIG_TREBLETYPE */
    COMP_IndexConfigAudioStereoWidening,     /**< reference: OMX_AUDIO_CONFIG_STEREOWIDENINGTYPE */
    COMP_IndexConfigAudioChorus,             /**< reference: OMX_AUDIO_CONFIG_CHORUSTYPE */
    COMP_IndexConfigAudioEqualizer,          /**< reference: OMX_AUDIO_CONFIG_EQUALIZERTYPE */
    COMP_IndexConfigAudioReverberation,      /**< reference: OMX_AUDIO_CONFIG_REVERBERATIONTYPE */
    COMP_IndexConfigAudioChannelVolume,      /**< reference: OMX_AUDIO_CONFIG_CHANNELVOLUMETYPE */

    /* Image specific parameters and configurations */
    COMP_IndexImageStartUnused = 0x05000000,
    COMP_IndexParamImagePortFormat,          /**< reference: OMX_IMAGE_PARAM_PORTFORMATTYPE */
    COMP_IndexParamFlashControl,             /**< reference: OMX_IMAGE_PARAM_FLASHCONTROLTYPE */
    COMP_IndexConfigFocusControl,            /**< reference: OMX_IMAGE_CONFIG_FOCUSCONTROLTYPE */
    COMP_IndexParamQFactor,                  /**< reference: OMX_IMAGE_PARAM_QFACTORTYPE */
    COMP_IndexParamQuantizationTable,        /**< reference: OMX_IMAGE_PARAM_QUANTIZATIONTABLETYPE */
    COMP_IndexParamHuffmanTable,             /**< reference: OMX_IMAGE_PARAM_HUFFMANTTABLETYPE */
    COMP_IndexConfigFlashControl,            /**< reference: OMX_IMAGE_PARAM_FLASHCONTROLTYPE */

    /* Video specific parameters and configurations */
    COMP_IndexVideoStartUnused = 0x06000000,
    COMP_IndexParamVideoPortFormat,          /**< reference: OMX_VIDEO_PARAM_PORTFORMATTYPE */
    COMP_IndexParamVideoQuantization,        /**< reference: OMX_VIDEO_PARAM_QUANTIZATIONTYPE */
    COMP_IndexParamVideoFastUpdate,          /**< reference: OMX_VIDEO_PARAM_VIDEOFASTUPDATETYPE */
    COMP_IndexParamVideoBitrate,             /**< reference: OMX_VIDEO_PARAM_BITRATETYPE */
    COMP_IndexParamVideoMotionVector,        /**< reference: OMX_VIDEO_PARAM_MOTIONVECTORTYPE */
    COMP_IndexParamVideoIntraRefresh,        /**< reference: OMX_VIDEO_PARAM_INTRAREFRESHTYPE */
    COMP_IndexParamVideoErrorCorrection,     /**< reference: OMX_VIDEO_PARAM_ERRORCORRECTIONTYPE */
    COMP_IndexParamVideoVBSMC,               /**< reference: OMX_VIDEO_PARAM_VBSMCTYPE */
    COMP_IndexParamVideoMpeg2,               /**< reference: OMX_VIDEO_PARAM_MPEG2TYPE */
    COMP_IndexParamVideoMpeg4,               /**< reference: OMX_VIDEO_PARAM_MPEG4TYPE */
    COMP_IndexParamVideoWmv,                 /**< reference: OMX_VIDEO_PARAM_WMVTYPE */
    COMP_IndexParamVideoRv,                  /**< reference: OMX_VIDEO_PARAM_RVTYPE */
    COMP_IndexParamVideoAvc,                 /**< reference: OMX_VIDEO_PARAM_AVCTYPE */
    COMP_IndexParamVideoH263,                /**< reference: OMX_VIDEO_PARAM_H263TYPE */
    COMP_IndexParamVideoProfileLevelQuerySupported, /**< reference: OMX_VIDEO_PARAM_PROFILELEVELTYPE */
    COMP_IndexParamVideoProfileLevelCurrent, /**< reference: OMX_VIDEO_PARAM_PROFILELEVELTYPE */
    COMP_IndexConfigVideoBitrate,            /**< reference: OMX_VIDEO_CONFIG_BITRATETYPE */
    COMP_IndexConfigVideoFramerate,          /**< reference: OMX_CONFIG_FRAMERATETYPE */
    COMP_IndexConfigVideoIntraVOPRefresh,    /**< reference: OMX_CONFIG_INTRAREFRESHVOPTYPE */
    COMP_IndexConfigVideoIntraMBRefresh,     /**< reference: OMX_CONFIG_MACROBLOCKERRORMAPTYPE */
    COMP_IndexConfigVideoMBErrorReporting,   /**< reference: OMX_CONFIG_MBERRORREPORTINGTYPE */
    COMP_IndexParamVideoMacroblocksPerFrame, /**< reference: OMX_PARAM_MACROBLOCKSTYPE */
    COMP_IndexConfigVideoMacroBlockErrorMap, /**< reference: OMX_CONFIG_MACROBLOCKERRORMAPTYPE */
    COMP_IndexParamVideoSliceFMO,            /**< reference: OMX_VIDEO_PARAM_AVCSLICEFMO */
    COMP_IndexConfigVideoAVCIntraPeriod,     /**< reference: OMX_VIDEO_CONFIG_AVCINTRAPERIOD */
    COMP_IndexConfigVideoNalSize,            /**< reference: OMX_VIDEO_CONFIG_NALSIZE */

    /* Image & Video common Configurations */
    COMP_IndexCommonStartUnused = 0x07000000,
    COMP_IndexParamCommonDeblocking,         /**< reference: OMX_PARAM_DEBLOCKINGTYPE */
    COMP_IndexParamCommonSensorMode,         /**< reference: OMX_PARAM_SENSORMODETYPE */
    COMP_IndexParamCommonInterleave,         /**< reference: OMX_PARAM_INTERLEAVETYPE */
    COMP_IndexConfigCommonColorFormatConversion, /**< reference: OMX_CONFIG_COLORCONVERSIONTYPE */
    COMP_IndexConfigCommonScale,             /**< reference: OMX_CONFIG_SCALEFACTORTYPE */
    COMP_IndexConfigCommonImageFilter,       /**< reference: OMX_CONFIG_IMAGEFILTERTYPE */
    COMP_IndexConfigCommonColorEnhancement,  /**< reference: OMX_CONFIG_COLORENHANCEMENTTYPE */
    COMP_IndexConfigCommonColorKey,          /**< reference: OMX_CONFIG_COLORKEYTYPE */
    COMP_IndexConfigCommonColorBlend,        /**< reference: OMX_CONFIG_COLORBLENDTYPE */
    COMP_IndexConfigCommonFrameStabilisation,/**< reference: OMX_CONFIG_FRAMESTABTYPE */
    COMP_IndexConfigCommonRotate,            /**< reference: OMX_CONFIG_ROTATIONTYPE */
    COMP_IndexConfigCommonMirror,            /**< reference: OMX_CONFIG_MIRRORTYPE */
    COMP_IndexConfigCommonOutputPosition,    /**< reference: OMX_CONFIG_POINTTYPE */
    COMP_IndexConfigCommonInputCrop,         /**< reference: OMX_CONFIG_RECTTYPE */
    COMP_IndexConfigCommonOutputCrop,        /**< reference: OMX_CONFIG_RECTTYPE */
    COMP_IndexConfigCommonDigitalZoom,       /**< reference: OMX_CONFIG_SCALEFACTORTYPE */
    COMP_IndexConfigCommonOpticalZoom,       /**< reference: OMX_CONFIG_SCALEFACTORTYPE*/
    COMP_IndexConfigCommonWhiteBalance,      /**< reference: OMX_CONFIG_WHITEBALCONTROLTYPE */
    COMP_IndexConfigCommonExposure,          /**< reference: OMX_CONFIG_EXPOSURECONTROLTYPE */
    COMP_IndexConfigCommonContrast,          /**< reference: OMX_CONFIG_CONTRASTTYPE */
    COMP_IndexConfigCommonBrightness,        /**< reference: OMX_CONFIG_BRIGHTNESSTYPE */
    COMP_IndexConfigCommonBacklight,         /**< reference: OMX_CONFIG_BACKLIGHTTYPE */
    COMP_IndexConfigCommonGamma,             /**< reference: OMX_CONFIG_GAMMATYPE */
    COMP_IndexConfigCommonSaturation,        /**< reference: OMX_CONFIG_SATURATIONTYPE */
    COMP_IndexConfigCommonLightness,         /**< reference: OMX_CONFIG_LIGHTNESSTYPE */
    COMP_IndexConfigCommonExclusionRect,     /**< reference: OMX_CONFIG_RECTTYPE */
    COMP_IndexConfigCommonDithering,         /**< reference: OMX_CONFIG_DITHERTYPE */
    COMP_IndexConfigCommonPlaneBlend,        /**< reference: OMX_CONFIG_PLANEBLENDTYPE */
    COMP_IndexConfigCommonExposureValue,     /**< reference: OMX_CONFIG_EXPOSUREVALUETYPE */
    COMP_IndexConfigCommonOutputSize,        /**< reference: OMX_FRAMESIZETYPE */
    COMP_IndexParamCommonExtraQuantData,     /**< reference: OMX_OTHER_EXTRADATATYPE */
    COMP_IndexConfigCommonFocusRegion,       /**< reference: OMX_CONFIG_FOCUSREGIONTYPE */
    COMP_IndexConfigCommonFocusStatus,       /**< reference: OMX_PARAM_FOCUSSTATUSTYPE */
    COMP_IndexConfigCommonTransitionEffect,  /**< reference: OMX_CONFIG_TRANSITIONEFFECTTYPE */

    /* Reserved Configuration range */
    COMP_IndexOtherStartUnused = 0x08000000,
    COMP_IndexParamOtherPortFormat,          /**< reference: OMX_OTHER_PARAM_PORTFORMATTYPE */
    COMP_IndexConfigOtherPower,              /**< reference: OMX_OTHER_CONFIG_POWERTYPE */
    COMP_IndexConfigOtherStats,              /**< reference: OMX_OTHER_CONFIG_STATSTYPE */


    /* Reserved Time range */
    COMP_IndexTimeStartUnused = 0x09000000,
    COMP_IndexConfigTimeScale,               /**< reference: OMX_TIME_CONFIG_SCALETYPE */
    COMP_IndexConfigTimeClockState,          /**< reference: OMX_TIME_CONFIG_CLOCKSTATETYPE */
    COMP_IndexConfigTimeActiveRefClock,      /**< reference: OMX_TIME_CONFIG_ACTIVEREFCLOCKTYPE */
    COMP_IndexConfigTimeCurrentMediaTime,    /**< reference: OMX_TIME_CONFIG_TIMESTAMPTYPE (read only) */
    COMP_IndexConfigTimeCurrentWallTime,     /**< reference: OMX_TIME_CONFIG_TIMESTAMPTYPE (read only) */
    COMP_IndexConfigTimeCurrentAudioReference, /**< reference: OMX_TIME_CONFIG_TIMESTAMPTYPE (write only) */
    COMP_IndexConfigTimeCurrentVideoReference, /**< reference: OMX_TIME_CONFIG_TIMESTAMPTYPE (write only) */
    COMP_IndexConfigTimeMediaTimeRequest,    /**< reference: OMX_TIME_CONFIG_MEDIATIMEREQUESTTYPE (write only) */
    COMP_IndexConfigTimeClientStartTime,     /**<reference:  OMX_TIME_CONFIG_TIMESTAMPTYPE (write only) */
    COMP_IndexConfigTimePosition,            /**< reference: OMX_TIME_CONFIG_TIMESTAMPTYPE */
    COMP_IndexConfigTimeSeekMode,            /**< reference: OMX_TIME_CONFIG_SEEKMODETYPE */


    COMP_IndexKhronosExtensions = 0x6F000000, /**< Reserved region for introducing Khronos Standard Extensions */
    /* Vendor specific area */
    COMP_IndexVendorStartUnused = 0x7F000000,
    /* Vendor specific structures should be in the range of 0x7F000000
       to 0x7FFFFFFE.  This range is not broken out by vendor, so
       private indexes are not guaranteed unique and therefore should
       only be sent to the appropriate component. */

    //for IPCLinux MPP vendor
    COMP_IndexVendorMPPChannelInfo = 0x7F002000,    /**< reference: MPP_CHN_S */
    COMP_IndexVendorMPPChannelFd,   /**< reference: int */
    COMP_IndexVendorGetPortParam,   /**< reference: COMP_PORT_PARAM_TYPE */
    COMP_IndexVendorTunnelInfo,     /**< reference: COMP_INTERNAL_TUNNELINFOTYPE */
    COMP_IndexVendorParamPortExtraDefinition,   /**< reference: COMP_PARAM_PORTEXTRADEFINITIONTYPE */
    COMP_IndexVendorBufferState, /**< reference: OMX_BUFFERSTATE (for audiodec, videodec)*/
    COMP_IndexVendorSeekToPosition, /**< reference: CedarXSeekPara*/
    COMP_IndexVendorSetStreamEof,
    COMP_IndexVendorClearStreamEof,
    COMP_IndexVendorSwitchAudio,
    COMP_IndexVendorSetAVSync,  /**< reference: int */

    /* VENC vendor*/
    COMP_IndexVendorVencChnAttr,   /**< reference: VENC_ATTR_S */
    COMP_IndexVendorVencChnPriority, /**< reference: unsigned int */
    COMP_IndexVendorVencResetChannel,   /**< reference: NULL */
    COMP_IndexVendorVencRecvPicParam,   /**< reference: VENC_RECV_PIC_PARAM_S */
    COMP_IndexVendorVencChnState,       /**< reference: VENC_CHN_STAT_S */
    COMP_IndexVendorVencGetStream,      /**< reference: VEncStream */
    COMP_IndexVendorVencReleaseStream,  /**< reference: VENC_STREAM_S */
    COMP_IndexVendorVencUserData,       /**< reference: DataSection */
    COMP_IndexVendorVencStreamDuration, /**< reference: double */
    COMP_IndexVendorVencRequestIDR,     /**< reference: BOOL */
    COMP_IndexVendorVencRoiCfg,         /**< reference: VENC_ROI_CFG_S */
    COMP_IndexVendorVencQPmap,      /**< reference: VencMBModeCtrl */
    COMP_IndexVendorVencQPmapMBInfoOutput,  /**< reference: VencMBInfo */
    COMP_IndexVendorVencQPmapMBSumInfoOutput,   /**< reference: VencMBSumInfo */
    COMP_IndexVendorVencH264SliceSplit, /**< reference: VENC_PARAM_H264_SLICE_SPLIT_S */
    COMP_IndexVendorVencH264InterPred,  /**< reference: VENC_PARAM_H264_INTER_PRED_S */
    COMP_IndexVendorVencH264IntraPred,  /**< reference: VENC_PARAM_H264_INTRA_PRED_S */
    COMP_IndexVendorVencH264Trans,      /**< reference: VENC_PARAM_H264_TRANS_S */
    COMP_IndexVendorVencH264Entropy,    /**< reference: VENC_PARAM_H264_ENTROPY_S */
    COMP_IndexVendorVencH264Poc,        /**< reference: VENC_PARAM_H264_POC_S */
    COMP_IndexVendorVencH264Dblk,       /**< reference: VENC_PARAM_H264_DBLK_S */
    COMP_IndexVendorVencH264Vui,        /**< reference: VENC_PARAM_H264_VUI_S */
    COMP_IndexVendorVencJpegParam,      /**< reference: VENC_PARAM_JPEG_S */
    COMP_IndexVendorVencJpegExifInfo,   /**< reference: VENC_EXIFINFO_S */
    COMP_IndexVendorVencJpegThumbBuffer,/**< reference: VENC_JPEG_THUMB_BUFFER_S */
    COMP_IndexVendorVencMjpegParam,     /**< reference: VENC_PARAM_MJPEG_S */
    COMP_IndexVendorVencFrameRate,      /**< reference: VENC_FRAME_RATE_S */
    COMP_IndexVendorTimeLapse,          /**< reference: int64_t */
    COMP_IndexVendorVencRcParam,        /**< reference: VENC_RC_PARAM_S */
    COMP_IndexVendorVencRefParam,       /**< reference: VENC_PARAM_REF_S */
    COMP_IndexVendorVencColor2Grey,     /**< reference: VENC_COLOR2GREY_S */
    COMP_IndexVendorVencCrop,           /**< reference: VENC_CROP_CFG_S */
    COMP_IndexVendorVencJpegSnapMode,   /**< reference: VENC_JPEG_SNAP_MODE_E */
    COMP_IndexVendorVencEnableIDR,      /**< reference: BOOL */
    COMP_IndexVendorVencStreamBufInfo,  /**< reference: VENC_STREAM_BUF_INFO_S */
    COMP_IndexVendorVencRcPriority,     /**< reference: VENC_RC_PRIORITY_E */
    COMP_IndexVendorVencH265SliceSplit, /**< reference: VENC_PARAM_H265_SLICE_SPLIT_S */
    COMP_IndexVendorVencH265PredUnit,   /**< reference: VENC_PARAM_H265_PU_S */
    COMP_IndexVendorVencH265Trans,      /**< reference: VENC_PARAM_H265_TRANS_S */
    COMP_IndexVendorVencH265Entropy,    /**< reference: VENC_PARAM_H265_ENTROPY_S */
    COMP_IndexVendorVencH265Dblk,       /**< reference: VENC_PARAM_H265_DBLK_S */
    COMP_IndexVendorVencH265Sao,        /**< reference: VENC_PARAM_H265_SAO_S */
    COMP_IndexVendorVencH265Timing,     /**< reference: VENC_PARAM_H265_TIMING_S */
    COMP_IndexVendorVencFrameLostStrategy,  /**< reference: VENC_PARAM_FRAMELOST_S */
    COMP_IndexVendorVencSuperFrameCfg,  /**< reference: VENC_SUPERFRAME_CFG_S */
    COMP_IndexVendorVencIntraRefresh,   /**< reference: VENC_PARAM_INTRA_REFRESH_S */
    COMP_IndexVendorVencSmartP, /**< reference: VencSmartFun */
    COMP_IndexVendorVencBrightness, /**< reference: VencBrightnessS */
    COMP_IndexVendorVencOsd, /**< reference: VencOverlayInfoS */
    COMP_IndexVendorVEFreq,   /**< reference: int*, unit:MHz */
    COMP_IndexVendorVenc3DNR,   /**reference: int   */
    COMP_IndexVendorVencCacheState,
    COMP_IndexVendorVencHorizonFlip, /** reference: BOOL     */
    COMP_IndexVendorVencAdaptiveIntraInP,  
    COMP_IndexVendorVencEnableNullSkip, /** reference: BOOL     */
    COMP_IndexVendorVencEnablePSkip, /** reference: BOOL     */
    COMP_IndexVendorVencForbidDiscardingFrame, /** reference: BOOL     */
    COMP_IndexVendorSaveBSFile, /** reference: VencSaveBSFile */
    COMP_IndexVendorVencProcSet, /** reference: VeProcSet */

    // below for aenc
    COMP_IndexVendorAencChnAttr,        /**< reference: AENC_CHN_ATTR_S */
    COMP_IndexVendorAencChnPriority,    /**< reference: unsigned int */
    COMP_IndexVendorAencChnState,       /**< reference: AENC_CHN_STAT_S*/
    COMP_IndexVendorAencResetChannel,   /**< reference: NULL */
    COMP_IndexVendorAencGetStream,      /**< reference: AUDIO_STREAM_S */
    COMP_IndexVendorAencReleaseStream,  /**< reference: AUDIO_STREAM_S */

    // below for muxer
    COMP_IndexVendorMuxGroupAttr,   /**< reference: MUX_GRP_ATTR_S */
    COMP_IndexVendorMuxChnAttr,     /**< reference: MuxChnAttr */
    COMP_IndexVendorMuxGetDuration, /**< reference: int64_t */
    COMP_IndexVendorMuxCacheState,  /**< reference: CacheState */
    COMP_IndexVendorMuxSwitchFd,    /**< reference: CdxFdT */
    COMP_IndexVendorSdcardState,    /**< reference: int */
    COMP_IndexVendorsetMuxCacheDuration,  /**< reference: int64_t */
    COMP_IndexVendorExtraData,      /**< reference: VencHeaderData, for h264: spspps */
    //COMP_IndexVendorFsWriteMode, /**< reference: FSWRITEMODE */
    //COMP_IndexVendorFsSimpleCacheSize,   /**< reference: int */
    COMP_IndexVendorMuxSwitchPolicy,   /** RecordFileDurationPolicy  */
    COMP_IndexVendorMuxShutDownType,   /**ShutDownType **/
    
    // below for demux
    COMP_IndexVendorDemuxChnAttr,   /**< reference: DEMUX_ATTR_S */
    COMP_IndexVendorDemuxResetChannel,   /**< reference: NULL */
    COMP_IndexVendorDemuxChnPriority,   /**< not imp*/
    COMP_IndexVendorDemuxChnState,
    COMP_IndexVendorDemuxSetDataSource,
    COMP_IndexVendorDemuxDisableTrack,
    COMP_IndexVendorDemuxDisableMediaType,
    COMP_IndexVendorDemuxDisableProprityTrack,
    COMP_IndexVendorDemuxPreparePorts,
    COMP_IndexVendorDemuxType,
    COMP_IndexVendorDemuxMediaInfo,
    COMP_IndexVendorDemuxMediaType,
    COMP_IndexVendorDemuxOutBuffer, //save demux buf out

    // below for adec
    COMP_IndexVendorAdecChnAttr,   /**< reference: ADEC_CHN_ATTR_S */
    COMP_IndexVendorAdecChnPriority, /**< reference: unsigned int */
    COMP_IndexVendorAdecResetChannel,   /**< reference: NULL */
    COMP_IndexVendorAdecCacheState,  /**< reference: CacheState */
    COMP_IndexVendorAdecSetDecodeMode,
    COMP_IndexVendorAdecSwitchAudio,
    COMP_IndexVendorAdecSetStreamEof,
    COMP_IndexVendorAdecClearStreamEof,
    COMP_IndexVendorAdecSelectAudioOut,
    COMP_IndexVendorAdecSetVolume,
    COMP_IndexVendorAdecSetAudioChannelMute,
    COMP_IndexVendorAdecSwitchAuioChannnel,
    COMP_IndexVendorAdecChnState,
    COMP_IndexVendorAdecGetFrame,           /**< reference: AUDIO_FRAME_S */
    COMP_IndexVendorAdecReleaseFrame,       /**< reference: AUDIO_FRAME_S */

    COMP_IndexVendorAIChnGetValidFrame,     /**< reference: AudioFrame */
    COMP_IndexVendorAIChnGetFreeFrame,      /**< reference: AudioFrame */
    COMP_IndexVendorAIChnReleaseFrame,      /**< reference: AudioFrame */
    COMP_IndexVendorAIChnParameter,         /**< reference: AI_CHN_PARAM_S */
    COMP_IndexVendorAIResetChannel,
    COMP_IndexVendorAISetSaveFileInfo,      /**< reference: AUDIO_SAVE_FILE_INFO_S */
    COMP_IndexVendorAIQueryFileStatus,      /**< reference: AUDIO_SAVE_FILE_INFO_S */
    COMP_IndexVendorAIChnMute,           /**< reference: BOOL* */
    COMP_IndexVendorAOChnPcmCardType,
    COMP_IndexVendorAOChnSendFrame,         /**< reference: AudioFrame */
    COMP_IndexVendorAOQueryChnStat,
    COMP_IndexVendorAOPauseChn,
    COMP_IndexVendorAOResumeChn,
    COMP_IndexVendorAOSetSaveFileInfo,      /**< reference: AUDIO_SAVE_FILE_INFO_S */
    COMP_IndexVendorAOQueryFileStatus,      /**< reference: AUDIO_SAVE_FILE_INFO_S */
    COMP_IndexVendorAIOVqeAttr,             /**< reference: AI_VQE_CONFIG_S */
    COMP_IndexVendorAIOVqeEnable,
    COMP_IndexVendorAIOVqeDisable,
    COMP_IndexVendorAIOReSmpEnable,         /**< reference: AUDIO_SAMPLE_RATE_E */
    COMP_IndexVendorAIOReSmpDisable,

    COMP_IndexVendorAEncSetAudioInfo,       /**< reference: AUDIO_INFO_S */
    COMP_IndexVendorAEncSendFrame,          /**< reference: AudioFrame */
    COMP_IndexVendorAEncGetStream,          /**< reference: AudioStream */
    COMP_IndexVendorAEncReleaseStream,      /**< reference: AudioStream */

	/* VI vendor*/
	COMP_IndexVendorViInit,
	COMP_IndexVendorViExit,
	COMP_IndexVendorViDevAttr,			/**< reference: Get/Set Attr*/
	COMP_IndexVendorViDevStart,
	COMP_IndexVendorViDevStop,
	COMP_IndexVendorViChnInit,
	COMP_IndexVendorViChnExit,
	COMP_IndexVendorViChnAttr,			/**< reference: Get/Set Attr*/
	COMP_IndexVendorViChnStart,
	COMP_IndexVendorViChnStop,
	COMP_IndexVendorViCapStart,
	COMP_IndexVendorViCapStop,
	COMP_IndexVendorViIntEnable,
	COMP_IndexVendorViIntDisable,
	COMP_IndexVendorViGetFrame,
	COMP_IndexVendorViReleaseFrame,
	COMP_IndexVendorViStoreFrame,   /**< store one frame into file, const char* dirPath */
	COMP_IndexVendorViSetOsdMaskRegion,
	COMP_IndexVendorViUpdateOsdMaskRegion,
	COMP_IndexVendorViSetLongExp,   /**< set long exposure mode */

	/* VDA vendor*/
    COMP_IndexVendorVdaChnAttr,     	/**< reference: Get/Set Attr*/
	COMP_IndexVendorVdaStart,	  		/**< reference:  */
	COMP_IndexVendorVdaStop,	  		/**< reference:  */
	COMP_IndexVendorGetVdaResult,	   	/**< reference:	*/
	COMP_IndexVendorReleaseVdaResult,   /**< reference:	*/

	/* ISE vendor*/
	COMP_IndexVendorIseStart,	  		/**< reference:  */
	COMP_IndexVendorIseStop,	  		/**< reference:  */
	COMP_IndexVendorGetIseData,			/**< reference:	*/
	COMP_IndexVendorReleaseIseData,		/**< reference:	*/
	COMP_IndexVendorIseGroupAttr,
	COMP_IndexVendorIseChnAttr,
	COMP_IndexVendorIseAddChn,
	COMP_IndexVendorIseRemoveChn,
	COMP_IndexVendorIseResetChannel,
	COMP_IndexVendorIseFreq,

	/* EIS vendor*/
	COMP_IndexVendorEisEnable,	  		/**< reference: int EnableFlag, 0: disable; 1: enable */
	COMP_IndexVendorEisGetData,			/**< reference:	EIS_PARAMS_S */
	COMP_IndexVendorEisReleaseData,		/**< reference: EIS_PARAMS_S */
	COMP_IndexVendorEisStoreFrame,		/**< reference: const char* */
	COMP_IndexVendorEisFlushChn,		/**< reference: NULL */
	COMP_IndexVendorEisChnAttr,			/**< reference: EIS_ATTR_S */
	COMP_IndexVendorEisVFmtAttr,		/**< reference: EIS_VIDEO_FMT_ATTR_S */
	COMP_IndexVendorEisDataProcAttr,	/**< reference: EIS_DATA_PROC_ATTR_S */
	COMP_IndexVendorEisGyroAttr,		/**< reference: EIS_GYRO_DATA_ATTR_S */
	COMP_IndexVendorEisKmatAttr,		/**< reference: EIS_KMAT_S */
	COMP_IndexVendorEisAlgoModeAttr,	/**< reference: EIS_ALGO_MODE */
	COMP_IndexVendorEisResetChannel,
	COMP_IndexVendorEisFreq,


    /* VDEC vendor*/
    COMP_IndexVendorVdecChnAttr,    /**< reference: VDEC_CHN_ATTR_S */
    COMP_IndexVendorVdecChnState,   /**< reference: VDEC_CHN_STAT_S */
    COMP_IndexVendorVdecResetChannel,   /**< reference: NULL */
    COMP_IndexVendorVdecParam,      /**< reference: VDEC_CHN_PARAM_S */
    COMP_IndexVendorVdecProtocolParam, /**< reference: VDEC_PRTCL_PARAM_S */
    COMP_IndexVendorVdecGetFrame,      /**< reference: VdecOutFrame */
    COMP_IndexVendorVdecReleaseFrame,      /**< reference: VdecOutFrame */
    COMP_IndexVendorVdecRotate,     /**< reference: ROTATE_E */
    COMP_IndexVendorVdecLuma,       /**< reference: VDEC_CHN_LUM_S */
    //COMP_IndexVendorVdecVBVSize,    /**< reference: unsigned int */
    COMP_IndexVendorResolutionChange,
    COMP_IndexVendorFbmBufInfo,  /**< reference: FbmBufInfo, get vdeclib frame buffer requirement*/
    COMP_IndexVendorConfigInputBuffer,  /**< reference: COMP_BUFFERHEADERTYPE */
    COMP_IndexVendorIonFd,
    COMP_IndexVendorCedarvOutputSetting,    /**< reference: EPIXELFORMAT */
    COMP_IndexVendorDisplayFrameRequestMode,    /**< reference: int */
    COMP_IndexVendorFrameBuffers,   /**< reference: VDecCompFrameBuffersParam */
    COMP_IndexVendorReopenVideoEngine,
    COMP_IndexVendorVdecDecodeFrameParam,

    /* VO vendor*/
    COMP_IndexVendorVOChnAttr,   /**< reference: VO_CHN_ATTR_S */
    COMP_IndexVendorVOChnDisplayField,  /**< reference: VO_DISPLAY_FIELD_E */
    COMP_IndexVendorShow,       /**< reference: BOOL*, show pic or hide pic*/
    COMP_IndexVendorInitInstance,   /**< reference: void* */
    COMP_IndexVendorVRenderMode,    /**< reference: VideoRenderMode */
    COMP_IndexVendorVOChnDisplaySize,   /**< reference: SIZE_S */
    COMP_IndexVendorVODispBufNum,   /**< reference: int */
    COMP_IndexVendorStoreFrame, /**< reference: uint64_t* */

    /* Clock vendor*/
    COMP_IndexVendorVps,                     /**< reference: int, set or get vpsspeed to clock_component. 40~-100*/
    COMP_IndexVendorNotifyStartToRunInfo,    /**< reference: CDX_NotifyStartToRunTYPE. command clock comp to notify callerComponent the message:START_TO_RUN. */
    COMP_IndexVendorNotifyStartToRun,
    COMP_IndexVendorAdjustClock,
    COMP_IndexVendorConfigTimeClientForceStart,
    COMP_IndexConfigWallTimeBase,   /**< reference: int64_t, media_time, modify WallTimeBase according to new media_time, implicitly modify media time of clock component */
        
	/* Uvc vendor*/
	COMP_IndexVendorUvcChnAttr,     	/**< reference: Get/Set Attr*/
	COMP_IndexVendorUvcProcessThreadStart,	  		/**< reference:  */
	COMP_IndexVendorUvcGetThreadStart,	
	COMP_IndexVendorUvcProcessThreadStop,	  		/**< reference:  */
	COMP_IndexVendorUvcGetThreadStop,	
	COMP_IndexVendorGetUvcFrame,	   	/**< reference:	*/
	COMP_IndexVendorReleaseUvcFrame,   /**< reference:	*/
	COMP_IndexVendorUvcSetDevInfo,

    /*For text enc*/
    COMP_IndexVendorTencChnAttr, 
    COMP_IndexVendorTencResetChannel,   /**< reference: NULL */
    COMP_IndexVendorTencGetStream,      /**< reference: TEXT_STREAM_S */
    COMP_IndexVendorTencReleaseStream,  /**< reference: TEXT_STREAM_S */
    
    COMP_IndexMax = 0x7FFFFFFF

} COMP_INDEXTYPE;

/** @ingroup comp */
typedef enum COMP_EVENTTYPE
{
    COMP_EventCmdComplete,         /**< component has sucessfully completed a command */
    COMP_EventError,               /**< component has detected an error condition */
    COMP_EventMark,                /**< component has detected a buffer mark */
    COMP_EventPortSettingsChanged, /**< component is reported a port settings change */
    COMP_EventBufferFlag,          /**< component has detected an EOS */
    COMP_EventResourcesAcquired,   /**< component has been granted resources and is
                                        automatically starting the state change from
                                        OMX_StateWaitForResources to OMX_StateIdle. */
    COMP_EventComponentResumed,     /**< Component resumed due to reacquisition of resources */
    COMP_EventDynamicResourcesAvailable, /**< Component has acquired previously unavailable dynamic resources */
    COMP_EventPortFormatDetected,      /**< Component has detected a supported format. */
    COMP_EventKhronosExtensions = 0x6F000000, /**< Reserved region for introducing Khronos Standard Extensions */
    COMP_EventVendorStartUnused = 0x7F000000, /**< Reserved region for introducing Vendor Extensions */
    COMP_EventKeyFrameDecoded,
    COMP_EventBufferPrefilled,  /**< Component fill data to other component, data1: size*/
    COMP_EventBufferMayAvaiable,

    COMP_EventControlPause,
    COMP_EventControlStart,
    COMP_EventBufferUpdate,
    COMP_EventBufferStart,
    COMP_EventBufferEnd,
    COMP_EventWholeBufferUpdate,
    COMP_EventMultiPixelExit,
    COMP_EventVideoDisplaySize,
    COMP_EventRenderingStart,

    COMP_EventMoreBuffer,
    COMP_EventInfo,
	COMP_EventRawdata,
    COMP_EventRecordDone,   /**< reference: int muxerId */

	COMP_EventNeedNextFd,   /**< reference: int muxerId */
	COMP_EventRecVbvFull,
	COMP_EventWriteDiskError,

	COMP_EventRecGpsDataInvalid,
    COMP_EventBsframeAvailable, /**< reference: CDXRecorderBsInfo */

    COMP_EventMax = 0x7FFFFFFF
} COMP_EVENTTYPE;

/** @ingroup comp */
typedef struct COMP_PORT_PARAM_TYPE {
    unsigned int nPorts;             /**< The number of ports for this component */
    unsigned int nStartPortNumber;   /** first port number for this type of port */
} COMP_PORT_PARAM_TYPE;

/** The COMP_BUFFERSUPPLIERTYPE enumeration is used to dictate port supplier
    preference when tunneling between two ports.
    @ingroup tun buf
*/
typedef enum COMP_BUFFERSUPPLIERTYPE
{
    COMP_BufferSupplyUnspecified = 0x0, /**< port supplying the buffers is unspecified,
                                              or don't care */
    COMP_BufferSupplyInput,             /**< input port supplies the buffers */
    COMP_BufferSupplyOutput,            /**< output port supplies the buffers */
    COMP_BufferSupplyKhronosExtensions = 0x6F000000, /**< Reserved region for introducing Khronos Standard Extensions */
    COMP_BufferSupplyVendorStartUnused = 0x7F000000, /**< Reserved region for introducing Vendor Extensions */
    COMP_BufferSupplyMax = 0x7FFFFFFF
} COMP_BUFFERSUPPLIERTYPE;

/** buffer supplier parameter
 * @ingroup tun
 */
typedef struct COMP_PARAM_BUFFERSUPPLIERTYPE {
    unsigned int nPortIndex; /**< port that this structure applies to */
    COMP_BUFFERSUPPLIERTYPE eBufferSupplier; /**< buffer supplier */
} COMP_PARAM_BUFFERSUPPLIERTYPE;

/**< indicates that buffers received by an input port of a tunnel
     may not modify the data in the buffers
     @ingroup tun
 */
#define COMP_PORTTUNNELFLAG_READONLY 0x00000001


/** The OMX_TUNNELSETUPTYPE structure is used to pass data from an output
    port to an input port as part the two ComponentTunnelRequest calls
    resulting from a OMX_SetupTunnel call from the IL Client.
    @ingroup tun
 */
typedef struct COMP_TUNNELSETUPTYPE
{
    unsigned int nTunnelFlags;             /**< bit flags for tunneling */
    COMP_BUFFERSUPPLIERTYPE eSupplier; /**< supplier preference */
} COMP_TUNNELSETUPTYPE;

/** End of Stream Buffer Flag:
  *
  * A component sets EOS when it has no more data to emit on a particular
  * output port. Thus an output port shall set EOS on the last buffer it
  * emits. A component's determination of when an output port should
  * cease sending data is implemenation specific.
  * @ingroup buf
  */

#define COMP_BUFFERFLAG_EOS 0x00000001

/** Start Time Buffer Flag:
 *
 * The source of a stream (e.g. a demux component) sets the STARTTIME
 * flag on the buffer that contains the starting timestamp for the
 * stream. The starting timestamp corresponds to the first data that
 * should be displayed at startup or after a seek.
 * The first timestamp of the stream is not necessarily the start time.
 * For instance, in the case of a seek to a particular video frame,
 * the target frame may be an interframe. Thus the first buffer of
 * the stream will be the intra-frame preceding the target frame and
 * the starttime will occur with the target frame (with any other
 * required frames required to reconstruct the target intervening).
 *
 * The STARTTIME flag is directly associated with the buffer's
 * timestamp ' thus its association to buffer data and its
 * propagation is identical to the timestamp's.
 *
 * When a Sync Component client receives a buffer with the
 * STARTTIME flag it shall perform a SetConfig on its sync port
 * using OMX_ConfigTimeClientStartTime and passing the buffer's
 * timestamp.
 *
 * @ingroup buf
 */

#define COMP_BUFFERFLAG_STARTTIME 0x00000002



/** Decode Only Buffer Flag:
 *
 * The source of a stream (e.g. a demux component) sets the DECODEONLY
 * flag on any buffer that should shall be decoded but should not be
 * displayed. This flag is used, for instance, when a source seeks to
 * a target interframe that requires the decode of frames preceding the
 * target to facilitate the target's reconstruction. In this case the
 * source would emit the frames preceding the target downstream
 * but mark them as decode only.
 *
 * The DECODEONLY is associated with buffer data and propagated in a
 * manner identical to the buffer timestamp.
 *
 * A component that renders data should ignore all buffers with
 * the DECODEONLY flag set.
 *
 * @ingroup buf
 */

#define COMP_BUFFERFLAG_DECODEONLY 0x00000004


/* Data Corrupt Flag: This flag is set when the IL client believes the data in the associated buffer is corrupt
 * @ingroup buf
 */

#define COMP_BUFFERFLAG_DATACORRUPT 0x00000008

/* End of Frame: The buffer contains exactly one end of frame and no data
 *  occurs after the end of frame. This flag is an optional hint. The absence
 *  of this flag does not imply the absence of an end of frame within the buffer.
 * @ingroup buf
*/
#define COMP_BUFFERFLAG_ENDOFFRAME 0x00000010

/* Sync Frame Flag: This flag is set when the buffer content contains a coded sync frame '
 *  a frame that has no dependency on any other frame information
 *  @ingroup buf
 */
#define COMP_BUFFERFLAG_SYNCFRAME 0x00000020

/* Extra data present flag: there is extra data appended to the data stream
 * residing in the buffer
 * @ingroup buf
 */
#define COMP_BUFFERFLAG_EXTRADATA 0x00000040

/** Codec Config Buffer Flag:
* OMX_BUFFERFLAG_CODECCONFIG is an optional flag that is set by an
* output port when all bytes in the buffer form part or all of a set of
* codec specific configuration data.  Examples include SPS/PPS nal units
* for OMX_VIDEO_CodingAVC or AudioSpecificConfig data for
* OMX_AUDIO_CodingAAC.  Any component that for a given stream sets
* OMX_BUFFERFLAG_CODECCONFIG shall not mix codec configuration bytes
* with frame data in the same buffer, and shall send all buffers
* containing codec configuration bytes before any buffers containing
* frame data that those configurations bytes describe.
* If the stream format for a particular codec has a frame specific
* header at the start of each frame, for example OMX_AUDIO_CodingMP3 or
* OMX_AUDIO_CodingAAC in ADTS mode, then these shall be presented as
* normal without setting OMX_BUFFERFLAG_CODECCONFIG.
 * @ingroup buf
 */
#define COMP_BUFFERFLAG_CODECCONFIG 0x00000080



/** @ingroup buf */
typedef struct COMP_BUFFERHEADERTYPE
{
    unsigned char* pBuffer;            /**< Pointer to actual block of memory
                                     that is acting as the buffer */
    unsigned int nAllocLen;          /**< size of the buffer allocated, in bytes */
    unsigned int nFilledLen;         /**< number of bytes currently in the
                                     buffer */
    unsigned int nOffset;            /**< start offset of valid data in bytes from
                                     the start of the buffer */
    void* pAppPrivate;        /**< pointer to any data the application
                                     wants to associate with this buffer */
    void* pPlatformPrivate;   /**< pointer to any data the platform
                                     wants to associate with this buffer */
    void* pInputPortPrivate;  /**< pointer to any data the input port
                                     wants to associate with this buffer */
    void* pOutputPortPrivate; /**< pointer to any data the output port
                                     wants to associate with this buffer */
    int64_t nTimeStamp;       /**< Timestamp corresponding to the sample
                                     starting at the first logical sample
                                     boundary in the buffer. Timestamps of
                                     successive samples within the buffer may
                                     be inferred by adding the duration of the
                                     of the preceding buffer to the timestamp
                                     of the preceding buffer.*/
    unsigned int     nFlags;         /**< buffer specific flags */
    unsigned int nOutputPortIndex;   /**< The index of the output port (if any) using
                                     this buffer */
    unsigned int nInputPortIndex;    /**< The index of the input port (if any) using
                                     this buffer */
} COMP_BUFFERHEADERTYPE;

typedef struct COMP_CALLBACKTYPE
{
    /** The EventHandler method is used to notify the application when an
        event of interest occurs.  Events are defined in the COMP_EVENTTYPE
        enumeration.  Please see that enumeration for details of what will
        be returned for each type of event. Callbacks should not return
        an error to the component, so if an error occurs, the application
        shall handle it internally.  This is a blocking call.

        The application should return from this call within 5 msec to avoid
        blocking the component for an excessively long period of time.

        @param hComponent
            handle of the component to access.  This is the component
            handle returned by the call to the GetHandle function.
        @param pAppData
            pointer to an application defined value that was provided in the
            pAppData parameter to the OMX_GetHandle method for the component.
            This application defined value is provided so that the application
            can have a component specific context when receiving the callback.
        @param eEvent
            Event that the component wants to notify the application about.
        @param nData1
            nData will be the ERRORTYPE for an error event and will be
            an OMX_COMMANDTYPE for a command complete event and OMX_INDEXTYPE for a OMX_PortSettingsChanged event.
         @param nData2
            nData2 will hold further information related to the event. Can be OMX_STATETYPE for
            a OMX_CommandStateSet command or port index for a OMX_PortSettingsChanged event.
            Default value is 0 if not used. )
        @param pEventData
            Pointer to additional event-specific data (see spec for meaning).
      */

   ERRORTYPE (*EventHandler)(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_IN void* pAppData,
        PARAM_IN COMP_EVENTTYPE eEvent,
        PARAM_IN unsigned int nData1,
        PARAM_IN unsigned int nData2,
        PARAM_IN void* pEventData);

    /** The EmptyBufferDone method is used to return emptied buffers from an
        input port back to the application for reuse.  This is a blocking call
        so the application should not attempt to refill the buffers during this
        call, but should queue them and refill them in another thread.  There
        is no error return, so the application shall handle any errors generated
        internally.

        The application should return from this call within 5 msec.

        @param hComponent
            handle of the component to access.  This is the component
            handle returned by the call to the GetHandle function.
        @param pAppData
            pointer to an application defined value that was provided in the
            pAppData parameter to the OMX_GetHandle method for the component.
            This application defined value is provided so that the application
            can have a component specific context when receiving the callback.
        @param pBuffer
            pointer to an COMP_BUFFERHEADERTYPE structure allocated with UseBuffer
            or AllocateBuffer indicating the buffer that was emptied.
        @ingroup buf
     */
    ERRORTYPE (*EmptyBufferDone)(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_IN void* pAppData,
        PARAM_IN COMP_BUFFERHEADERTYPE* pBuffer);

    /** The FillBufferDone method is used to return filled buffers from an
        output port back to the application for emptying and then reuse.
        This is a blocking call so the application should not attempt to
        empty the buffers during this call, but should queue the buffers
        and empty them in another thread.  There is no error return, so
        the application shall handle any errors generated internally.  The
        application shall also update the buffer header to indicate the
        number of bytes placed into the buffer.

        The application should return from this call within 5 msec.

        @param hComponent
            handle of the component to access.  This is the component
            handle returned by the call to the GetHandle function.
        @param pAppData
            pointer to an application defined value that was provided in the
            pAppData parameter to the OMX_GetHandle method for the component.
            This application defined value is provided so that the application
            can have a component specific context when receiving the callback.
        @param pBuffer
            pointer to an COMP_BUFFERHEADERTYPE structure allocated with UseBuffer
            or AllocateBuffer indicating the buffer that was filled.
        @ingroup buf
     */
    ERRORTYPE (*FillBufferDone)(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_IN void* pAppData,
        PARAM_IN COMP_BUFFERHEADERTYPE* pBuffer);

} COMP_CALLBACKTYPE;

/** The OMX_DIRTYPE enumeration is used to indicate if a port is an input or
    an output port.  This enumeration is common across all component types.
 */
typedef enum COMP_DIRTYPE
{
    COMP_DirInput,              /**< Port is an input port */
    COMP_DirOutput,             /**< Port is an output port */
    COMP_DirMax = 0x7FFFFFFF
} COMP_DIRTYPE;

/** @ingroup comp */
typedef enum COMP_PORTDOMAINTYPE {
    COMP_PortDomainAudio,
    COMP_PortDomainVideo,
    COMP_PortDomainImage,
    COMP_PortDomainOther,
    COMP_PortDomainKhronosExtensions = 0x6F000000, /**< Reserved region for introducing Khronos Standard Extensions */
    COMP_PortDomainVendorStartUnused = 0x7F000000, /**< Reserved region for introducing Vendor Extensions */
    COMP_PortDomainSubtitle,
    COMP_PortDomainText,	//for recorder mp4 gps info.
    COMP_PortDomainMax = 0x7ffffff
} COMP_PORTDOMAINTYPE;

/** The PortDefinition structure is used to define all of the parameters
 *  necessary for the compliant component to setup an input or an output audio
 *  path.  If additional information is needed to define the parameters of the
 *  port (such as frequency), additional structures must be sent such as the
 *  OMX_AUDIO_PARAM_PCMMODETYPE structure to supply the extra parameters for the port.
 */
typedef struct COMP_AUDIO_PORTDEFINITIONTYPE {
    char* cMIMEType;            /**< MIME type of data for the port */
    PAYLOAD_TYPE_E eEncoding;  /**< Type of data expected for this
                                          port (e.g. PCM, AMR, MP3, etc) */
} COMP_AUDIO_PORTDEFINITIONTYPE;

/**
 * Data structure used to define a video path.  The number of Video paths for
 * input and output will vary by type of the Video component.
 *
 *    Input (aka Source) : zero Inputs, one Output,
 *    Splitter           : one Input, 2 or more Outputs,
 *    Processing Element : one Input, one output,
 *    Mixer              : 2 or more inputs, one output,
 *    Output (aka Sink)  : one Input, zero outputs.
 *
 * The PortDefinition structure is used to define all of the parameters
 * necessary for the compliant component to setup an input or an output video
 * path.  If additional vendor specific data is required, it should be
 * transmitted to the component using the CustomCommand function.  Compliant
 * components will prepopulate this structure with optimal values during the
 * GetDefaultInitParams command.
 *
 * STRUCT MEMBERS:
 *  cMIMEType             : MIME type of data for the port
 *  pNativeRender         : Platform specific reference for a display if a
 *                          sync, otherwise this field is 0
 *  nFrameWidth           : Width of frame to be used on channel if
 *                          uncompressed format is used.  Use 0 for unknown,
 *                          don't care or variable
 *  nFrameHeight          : Height of frame to be used on channel if
 *                          uncompressed format is used. Use 0 for unknown,
 *                          don't care or variable
 *  nStride               : Number of bytes per span of an image
 *                          (i.e. indicates the number of bytes to get
 *                          from span N to span N+1, where negative stride
 *                          indicates the image is bottom up
 *  nSliceHeight          : Height used when encoding in slices
 *  nBitrate              : Bit rate of frame to be used on channel if
 *                          compressed format is used. Use 0 for unknown,
 *                          don't care or variable
 *  xFramerate            : Frame rate to be used on channel if uncompressed
 *                          format is used. Use 0 for unknown, don't care or
 *                          variable.  Units are Q16 frames per second.
 *  bFlagErrorConcealment : Turns on error concealment if it is supported by
 *                          the OMX component
 *  eCompressionFormat    : Compression format used in this instance of the
 *                          component. When OMX_VIDEO_CodingUnused is
 *                          specified, eColorFormat is used
 *  eColorFormat : Decompressed format used by this component
 *  pNativeWindow : Platform specific reference for a window object if a
 *                          display sink , otherwise this field is 0x0.
 */
typedef struct COMP_VIDEO_PORTDEFINITIONTYPE {
    char* cMIMEType;
    unsigned int nFrameWidth;
    unsigned int nFrameHeight;
    int nStride;
    unsigned int nSliceHeight;
    unsigned int nBitrate;
    unsigned int xFramerate;
    BOOL bFlagErrorConcealment;
    PAYLOAD_TYPE_E eCompressionFormat;
    PIXEL_FORMAT_E eColorFormat;
} COMP_VIDEO_PORTDEFINITIONTYPE;

/** @ingroup comp */
typedef struct COMP_PARAM_PORTDEFINITIONTYPE {
    unsigned int nPortIndex;            /**< Port number the structure applies to */
    COMP_DIRTYPE eDir;              /**< Direction (input or output) of this port */
    BOOL bEnabled;             /**< Ports default to enabled and are enabled/disabled by
                                        OMX_CommandPortEnable/OMX_CommandPortDisable.
                                        When disabled a port is unpopulated. A disabled port
                                        is not populated with buffers on a transition to IDLE. */
    COMP_PORTDOMAINTYPE eDomain;    /**< Domain of the port. Determines the contents of metadata below. */
    union {
        COMP_AUDIO_PORTDEFINITIONTYPE audio;
        COMP_VIDEO_PORTDEFINITIONTYPE video;
    } format;
} COMP_PARAM_PORTDEFINITIONTYPE;

typedef struct COMP_PARAM_PORTEXTRADEFINITIONTYPE
{
    unsigned int nPortIndex;
    void* pVendorInfo; //add by vendor
}COMP_PARAM_PORTEXTRADEFINITIONTYPE;

/** Enumeration of possible reference clocks to the media time. */
typedef enum COMP_TIME_REFCLOCKTYPE {
      COMP_TIME_RefClockNone,    /**< Use no references. */
      COMP_TIME_RefClockAudio,	/**< Use references sent through OMX_IndexConfigTimeCurrentAudioReference */
      COMP_TIME_RefClockVideo,   /**< Use references sent through OMX_IndexConfigTimeCurrentVideoReference */
      COMP_TIME_RefClockKhronosExtensions = 0x6F000000, /**< Reserved region for introducing Khronos Standard Extensions */ 
      COMP_TIME_RefClockVendorStartUnused = 0x7F000000, /**< Reserved region for introducing Vendor Extensions */
      COMP_TIME_RefClockMax = 0x7FFFFFFF
} COMP_TIME_REFCLOCKTYPE;

/** Enumeration of clock states. */
typedef enum COMP_TIME_CLOCKSTATE {
      COMP_TIME_ClockStateRunning,             /**< Clock running. */
      COMP_TIME_ClockStateWaitingForStartTime, /**< Clock waiting until the 
                                               *   prescribed clients emit their
                                               *   start time. */
      COMP_TIME_ClockStateStopped,             /**< Clock stopped. */
      COMP_TIME_ClockStateKhronosExtensions = 0x6F000000, /**< Reserved region for introducing Khronos Standard Extensions */ 
      COMP_TIME_ClockStateVendorStartUnused = 0x7F000000, /**< Reserved region for introducing Vendor Extensions */
      COMP_TIME_ClockStateMax = 0x7FFFFFFF
} COMP_TIME_CLOCKSTATE;

/** Bits used to identify a clock port. Used in OMX_TIME_CONFIG_CLOCKSTATETYPE's nWaitMask field */
#define COMP_CLOCKPORT0 0x00000001
#define COMP_CLOCKPORT1 0x00000002
#define COMP_CLOCKPORT2 0x00000004
#define COMP_CLOCKPORT3 0x00000008
#define COMP_CLOCKPORT4 0x00000010
#define COMP_CLOCKPORT5 0x00000020
#define COMP_CLOCKPORT6 0x00000040
#define COMP_CLOCKPORT7 0x00000080

/** Structure representing the current mode of the media clock. 
 *  IL Client uses this config to change or query the mode of the 
 *  media clock of the clock component. Applicable only to clock
 *  component. 
 *  
 *  On a SetConfig if eState is OMX_TIME_ClockStateRunning media time
 *  starts immediately at the prescribed start time. If
 *  OMX_TIME_ClockStateWaitingForStartTime the Clock Component ignores
 *  the given nStartTime and waits for all clients specified in the 
 *  nWaitMask to send starttimes (via 
 *  OMX_IndexConfigTimeClientStartTime). The Clock Component then starts 
 *  the media clock using the earliest start time supplied. */    
typedef struct COMP_TIME_CONFIG_CLOCKSTATETYPE {
    unsigned int nSize;              /**< size of the structure in bytes */
    COMP_TIME_CLOCKSTATE eState;     /**< State of the media time. */
    int64_t nStartTime;       /**< Start time of the media time. */
    int64_t nOffset;          /**< Time to offset the media time by 
                                 * (e.g. preroll). Media time will be
                                 * reported to be nOffset ticks earlier.     
                                 */
    unsigned int nWaitMask;          /**< Mask of OMX_CLOCKPORT values. */
} COMP_TIME_CONFIG_CLOCKSTATETYPE;

/** Structure representing the reference clock currently being used to
 *  compute media time. IL client uses this config to change or query the 
 *  clock component's active reference clock */
typedef struct COMP_TIME_CONFIG_ACTIVEREFCLOCKTYPE {
    COMP_TIME_REFCLOCKTYPE eClock;   /**< Reference clock used to compute media time */                        
} COMP_TIME_CONFIG_ACTIVEREFCLOCKTYPE;

typedef struct MM_COMPONENTTYPE
{
    /** pComponentPrivate is a pointer to the component private data area.
        This member is allocated and initialized by the component when the
        component is first loaded.  The application should not access this
        data area. */
    void* pComponentPrivate;

    /** pApplicationPrivate is a pointer that is a parameter to the
        OMX_GetHandle method, and contains an application private value
        provided by the IL client.  This application private data is
        returned to the IL Client by OMX in all callbacks */
    void* pApplicationPrivate;

    /** refer to OMX_SendCommand in OMX_core.h or the OMX IL
        specification for details on the SendCommand method.
     */
    ERRORTYPE (*SendCommand)(
            PARAM_IN  COMP_HANDLETYPE hComponent,
            PARAM_IN  COMP_COMMANDTYPE Cmd,
            PARAM_IN  unsigned int nParam1,
            PARAM_IN  void* pCmdData);


    /** refer to OMX_GetConfig in OMX_core.h or the OMX IL
        specification for details on the GetConfig method.
     */
    ERRORTYPE (*GetConfig)(
            PARAM_IN  COMP_HANDLETYPE hComponent,
            PARAM_IN  COMP_INDEXTYPE nIndex,
            PARAM_INOUT void* pComponentConfigStructure);


    /** refer to OMX_SetConfig in OMX_core.h or the OMX IL
        specification for details on the SetConfig method.
     */
    ERRORTYPE (*SetConfig)(
            PARAM_IN  COMP_HANDLETYPE hComponent,
            PARAM_IN  COMP_INDEXTYPE nIndex,
            PARAM_IN  void* pComponentConfigStructure);



    /** refer to OMX_GetState in OMX_core.h or the OMX IL
        specification for details on the GetState method.
     */
    ERRORTYPE (*GetState)(
            PARAM_IN  COMP_HANDLETYPE hComponent,
            PARAM_OUT COMP_STATETYPE* pState);

    ERRORTYPE (*GetFristAudioFrmPts)(
            PARAM_IN  COMP_HANDLETYPE hComponent,
            PARAM_OUT long long * a_pts);


    /** The ComponentTunnelRequest method will interact with another OMX
        component to determine if tunneling is possible and to setup the
        tunneling.  The return codes for this method can be used to
        determine if tunneling is not possible, or if tunneling is not
        supported.

        Base profile components (i.e. non-interop) do not support this
        method and should return OMX_ErrorNotImplemented

        The interop profile component MUST support tunneling to another
        interop profile component with a compatible port parameters.
        A component may also support proprietary communication.

        If proprietary communication is supported the negotiation of
        proprietary communication is done outside of OMX in a vendor
        specific way. It is only required that the proper result be
        returned and the details of how the setup is done is left
        to the component implementation.

        When this method is invoked when nPort in an output port, the
        component will:
        1.  Populate the pTunnelSetup structure with the output port's
            requirements and constraints for the tunnel.

        When this method is invoked when nPort in an input port, the
        component will:
        1.  Query the necessary parameters from the output port to
            determine if the ports are compatible for tunneling
        2.  If the ports are compatible, the component should store
            the tunnel step provided by the output port
        3.  Determine which port (either input or output) is the buffer
            supplier, and call OMX_SetParameter on the output port to
            indicate this selection.

        The component will return from this call within 5 msec.

        @param [in] hComp
            Handle of the component to be accessed.  This is the component
            handle returned by the call to the OMX_GetHandle method.
        @param [in] nPort
            nPort is used to select the port on the component to be used
            for tunneling.
        @param [in] hTunneledComp
            Handle of the component to tunnel with.  This is the component
            handle returned by the call to the OMX_GetHandle method.  When
            this parameter is 0x0 the component should setup the port for
            communication with the application / IL Client.
        @param [in] nPortOutput
            nPortOutput is used indicate the port the component should
            tunnel with.
        @param [in] pTunnelSetup
            Pointer to the tunnel setup structure.  When nPort is an output port
            the component should populate the fields of this structure.  When
            When nPort is an input port the component should review the setup
            provided by the component with the output port.
        @return ERRORTYPE
            If the command successfully executes, the return code will be
            OMX_ErrorNone.  Otherwise the appropriate OMX error will be returned.
        @ingroup tun
    */

    ERRORTYPE (*ComponentTunnelRequest)(
        PARAM_IN  COMP_HANDLETYPE hComp,
        PARAM_IN  unsigned int nPort,
        PARAM_IN  COMP_HANDLETYPE hTunneledComp,
        PARAM_IN  unsigned int nTunneledPort,
        PARAM_INOUT  COMP_TUNNELSETUPTYPE* pTunnelSetup);

    /** refer to OMX_EmptyThisBuffer in OMX_core.h or the OMX IL
        specification for details on the EmptyThisBuffer method.
        @ingroup buf
     */
    ERRORTYPE (*EmptyThisBuffer)(
            PARAM_IN  COMP_HANDLETYPE hComponent,
            PARAM_IN  COMP_BUFFERHEADERTYPE* pBuffer);

    /** refer to OMX_FillThisBuffer in OMX_core.h or the OMX IL
        specification for details on the FillThisBuffer method.
        @ingroup buf
     */
    ERRORTYPE (*FillThisBuffer)(
            PARAM_IN  COMP_HANDLETYPE hComponent,
            PARAM_IN  COMP_BUFFERHEADERTYPE* pBuffer);

    /** The SetCallbacks method is used by the core to specify the callback
        structure from the application to the component.  This is a blocking
        call.  The component will return from this call within 5 msec.
        @param [in] hComponent
            Handle of the component to be accessed.  This is the component
            handle returned by the call to the GetHandle function.
        @param [in] pCallbacks
            pointer to an COMP_CALLBACKTYPE structure used to provide the
            callback information to the component
        @param [in] pAppData
            pointer to an application defined value.  It is anticipated that
            the application will pass a pointer to a data structure or a "this
            pointer" in this area to allow the callback (in the application)
            to determine the context of the call
        @return ERRORTYPE
            If the command successfully executes, the return code will be
            OMX_ErrorNone.  Otherwise the appropriate OMX error will be returned.
     */
    ERRORTYPE (*SetCallbacks)(
            PARAM_IN  COMP_HANDLETYPE hComponent,
            PARAM_IN  COMP_CALLBACKTYPE* pCallbacks,
            PARAM_IN  void* pAppData);

    /** ComponentDeInit method is used to deinitialize the component
        providing a means to free any resources allocated at component
        initialization.  NOTE:  After this call the component handle is
        not valid for further use.
        @param [in] hComponent
            Handle of the component to be accessed.  This is the component
            handle returned by the call to the GetHandle function.
        @return ERRORTYPE
            If the command successfully executes, the return code will be
            OMX_ErrorNone.  Otherwise the appropriate OMX error will be returned.
     */
    ERRORTYPE (*ComponentDeInit)(
            PARAM_IN  COMP_HANDLETYPE hComponent);

} MM_COMPONENTTYPE;


/* OMX Component headers is included to enable the core to use
   macros for functions into the component for OMX release 1.0.  
   Developers should not access any structures or data from within
   the component header directly */


/** Send a command to the component.  This call is a non-blocking call.
    The component should check the parameters and then queue the command
    to the component thread to be executed.  The component thread shall 
    send the EventHandler() callback at the conclusion of the command. 
    This macro will go directly from the application to the component (via
    a core macro).  The component will return from this call within 5 msec.
    
    When the command is "OMX_CommandStateSet" the component will queue a
    state transition to the new state idenfied in nParam.
    
    When the command is "OMX_CommandFlush", to flush a port's buffer queues,
    the command will force the component to return all buffers NOT CURRENTLY 
    BEING PROCESSED to the application, in the order in which the buffers 
    were received.
    
    When the command is "OMX_CommandPortDisable" or 
    "OMX_CommandPortEnable", the component's port (given by the value of
    nParam) will be stopped or restarted. 
    
    When the command "OMX_CommandMarkBuffer" is used to mark a buffer, the
    pCmdData will point to a OMX_MARKTYPE structure containing the component
    handle of the component to examine the buffer chain for the mark.  nParam1
    contains the index of the port on which the buffer mark is applied.

    Specification text for more details. 
    
    @param [in] hComponent
        handle of component to execute the command
    @param [in] Cmd
        Command for the component to execute
    @param [in] nParam
        Parameter for the command to be executed.  When Cmd has the value 
        OMX_CommandStateSet, value is a member of OMX_STATETYPE.  When Cmd has 
        the value OMX_CommandFlush, value of nParam indicates which port(s) 
        to flush. -1 is used to flush all ports a single port index will 
        only flush that port.  When Cmd has the value "OMX_CommandPortDisable"
        or "OMX_CommandPortEnable", the component's port is given by 
        the value of nParam.  When Cmd has the value "OMX_CommandMarkBuffer"
        the components pot is given by the value of nParam.
    @param [in] pCmdData
        Parameter pointing to the OMX_MARKTYPE structure when Cmd has the value
        "OMX_CommandMarkBuffer".     
    @return OMX_ERRORTYPE
        If the command successfully executes, the return code will be
        OMX_ErrorNone.  Otherwise the appropriate OMX error will be returned.
    @ingroup comp
 */
#define COMP_SendCommand(                                    \
         hComponent,                                        \
         Cmd,                                               \
         nParam,                                            \
         pCmdData)                                          \
     ((MM_COMPONENTTYPE*)hComponent)->SendCommand(         \
         hComponent,                                        \
         Cmd,                                               \
         nParam,                                            \
         pCmdData)                          /* Macro End */


/** The OMX_GetParameter macro will get one of the current parameter 
    settings from the component.  This macro cannot only be invoked when 
    the component is in the OMX_StateInvalid state.  The nParamIndex
    parameter is used to indicate which structure is being requested from
    the component.  The application shall allocate the correct structure 
    and shall fill in the structure size and version information before 
    invoking this macro.  When the parameter applies to a port, the
    caller shall fill in the appropriate nPortIndex value indicating the
    port on which the parameter applies. If the component has not had 
    any settings changed, then the component should return a set of 
    valid DEFAULT  parameters for the component.  This is a blocking 
    call.  
    
    The component should return from this call within 20 msec.
    
    @param [in] hComponent
        Handle of the component to be accessed.  This is the component
        handle returned by the call to the OMX_GetHandle function.
    @param [in] nParamIndex
        Index of the structure to be filled.  This value is from the
        OMX_INDEXTYPE enumeration.
    @param [in,out] pComponentParameterStructure
        Pointer to application allocated structure to be filled by the 
        component.
    @return OMX_ERRORTYPE
        If the command successfully executes, the return code will be
        OMX_ErrorNone.  Otherwise the appropriate OMX error will be returned.
    @ingroup comp
 */
#define COMP_GetParameter(                                   \
        hComponent,                                         \
        nParamIndex,                                        \
        pComponentParameterStructure)                        \
    ((MM_COMPONENTTYPE*)hComponent)->GetParameter(         \
        hComponent,                                         \
        nParamIndex,                                        \
        pComponentParameterStructure)    /* Macro End */


/** The OMX_SetParameter macro will send an initialization parameter
    structure to a component.  Each structure shall be sent one at a time,
    in a separate invocation of the macro.  This macro can only be
    invoked when the component is in the OMX_StateLoaded state, or the
    port is disabled (when the parameter applies to a port). The 
    nParamIndex parameter is used to indicate which structure is being
    passed to the component.  The application shall allocate the 
    correct structure and shall fill in the structure size and version 
    information (as well as the actual data) before invoking this macro.
    The application is free to dispose of this structure after the call
    as the component is required to copy any data it shall retain.  This 
    is a blocking call.  
    
    The component should return from this call within 20 msec.
    
    @param [in] hComponent
        Handle of the component to be accessed.  This is the component
        handle returned by the call to the OMX_GetHandle function.
    @param [in] nIndex
        Index of the structure to be sent.  This value is from the
        OMX_INDEXTYPE enumeration.
    @param [in] pComponentParameterStructure
        pointer to application allocated structure to be used for
        initialization by the component.
    @return OMX_ERRORTYPE
        If the command successfully executes, the return code will be
        OMX_ErrorNone.  Otherwise the appropriate OMX error will be returned.
    @ingroup comp
 */
#define COMP_SetParameter(                                   \
        hComponent,                                         \
        nParamIndex,                                        \
        pComponentParameterStructure)                        \
    ((MM_COMPONENTTYPE*)hComponent)->SetParameter(         \
        hComponent,                                         \
        nParamIndex,                                        \
        pComponentParameterStructure)    /* Macro End */


/** The OMX_GetConfig macro will get one of the configuration structures 
    from a component.  This macro can be invoked anytime after the 
    component has been loaded.  The nParamIndex call parameter is used to 
    indicate which structure is being requested from the component.  The 
    application shall allocate the correct structure and shall fill in the 
    structure size and version information before invoking this macro.  
    If the component has not had this configuration parameter sent before, 
    then the component should return a set of valid DEFAULT values for the 
    component.  This is a blocking call.  
    
    The component should return from this call within 5 msec.
    
    @param [in] hComponent
        Handle of the component to be accessed.  This is the component
        handle returned by the call to the OMX_GetHandle function.
    @param [in] nIndex
        Index of the structure to be filled.  This value is from the
        OMX_INDEXTYPE enumeration.
    @param [in,out] pComponentConfigStructure
        pointer to application allocated structure to be filled by the 
        component.
    @return OMX_ERRORTYPE
        If the command successfully executes, the return code will be
        OMX_ErrorNone.  Otherwise the appropriate OMX error will be returned.
    @ingroup comp
*/        
#define COMP_GetConfig(                                      \
        hComponent,                                         \
        nConfigIndex,                                       \
        pComponentConfigStructure)                           \
    ((MM_COMPONENTTYPE*)hComponent)->GetConfig(            \
        hComponent,                                         \
        nConfigIndex,                                       \
        pComponentConfigStructure)       /* Macro End */


/** The OMX_SetConfig macro will send one of the configuration 
    structures to a component.  Each structure shall be sent one at a time,
    each in a separate invocation of the macro.  This macro can be invoked 
    anytime after the component has been loaded.  The application shall 
    allocate the correct structure and shall fill in the structure size 
    and version information (as well as the actual data) before invoking 
    this macro.  The application is free to dispose of this structure after 
    the call as the component is required to copy any data it shall retain.  
    This is a blocking call.  
    
    The component should return from this call within 5 msec.
    
    @param [in] hComponent
        Handle of the component to be accessed.  This is the component
        handle returned by the call to the OMX_GetHandle function.
    @param [in] nConfigIndex
        Index of the structure to be sent.  This value is from the
        OMX_INDEXTYPE enumeration above.
    @param [in] pComponentConfigStructure
        pointer to application allocated structure to be used for
        initialization by the component.
    @return OMX_ERRORTYPE
        If the command successfully executes, the return code will be
        OMX_ErrorNone.  Otherwise the appropriate OMX error will be returned.
    @ingroup comp
 */
#define COMP_SetConfig(                                      \
        hComponent,                                         \
        nConfigIndex,                                       \
        pComponentConfigStructure)                           \
    ((MM_COMPONENTTYPE*)hComponent)->SetConfig(            \
        hComponent,                                         \
        nConfigIndex,                                       \
        pComponentConfigStructure)       /* Macro End */


/** The OMX_GetState macro will invoke the component to get the current 
    state of the component and place the state value into the location
    pointed to by pState.  
    
    The component should return from this call within 5 msec.
    
    @param [in] hComponent
        Handle of the component to be accessed.  This is the component
        handle returned by the call to the OMX_GetHandle function.
    @param [out] pState
        pointer to the location to receive the state.  The value returned
        is one of the OMX_STATETYPE members 
    @return OMX_ERRORTYPE
        If the command successfully executes, the return code will be
        OMX_ErrorNone.  Otherwise the appropriate OMX error will be returned.
    @ingroup comp
 */
#define COMP_GetState(                                       \
        hComponent,                                         \
        pState)                                             \
    ((MM_COMPONENTTYPE*)hComponent)->GetState(             \
        hComponent,                                         \
        pState)                         /* Macro End */


/** The OMX_EmptyThisBuffer macro will send a buffer full of data to an 
    input port of a component.  The buffer will be emptied by the component
    and returned to the application via the EmptyBufferDone call back.
    This is a non-blocking call in that the component will record the buffer
    and return immediately and then empty the buffer, later, at the proper 
    time.  As expected, this macro may be invoked only while the component 
    is in the OMX_StateExecuting.  If nPortIndex does not specify an input
    port, the component shall return an error.  
    
    The component should return from this call within 5 msec.
    
    @param [in] hComponent
        Handle of the component to be accessed.  This is the component
        handle returned by the call to the OMX_GetHandle function.
    @param [in] pBuffer
        pointer to an OMX_BUFFERHEADERTYPE structure allocated with UseBuffer
        or AllocateBuffer.
    @return OMX_ERRORTYPE
        If the command successfully executes, the return code will be
        OMX_ErrorNone.  Otherwise the appropriate OMX error will be returned.
    @ingroup comp buf
 */
#define COMP_EmptyThisBuffer(                                \
        hComponent,                                         \
        pBuffer)                                            \
    ((MM_COMPONENTTYPE*)hComponent)->EmptyThisBuffer(      \
        hComponent,                                         \
        pBuffer)                        /* Macro End */


/** The OMX_FillThisBuffer macro will send an empty buffer to an 
    output port of a component.  The buffer will be filled by the component
    and returned to the application via the FillBufferDone call back.
    This is a non-blocking call in that the component will record the buffer
    and return immediately and then fill the buffer, later, at the proper 
    time.  As expected, this macro may be invoked only while the component 
    is in the OMX_ExecutingState.  If nPortIndex does not specify an output
    port, the component shall return an error.  
    
    The component should return from this call within 5 msec.
    
    @param [in] hComponent
        Handle of the component to be accessed.  This is the component
        handle returned by the call to the OMX_GetHandle function.
    @param [in] pBuffer
        pointer to an OMX_BUFFERHEADERTYPE structure allocated with UseBuffer
        or AllocateBuffer.
    @return OMX_ERRORTYPE
        If the command successfully executes, the return code will be
        OMX_ErrorNone.  Otherwise the appropriate OMX error will be returned.
    @ingroup comp buf
 */
#define COMP_FillThisBuffer(                                 \
        hComponent,                                         \
        pBuffer)                                            \
    ((MM_COMPONENTTYPE*)hComponent)->FillThisBuffer(       \
        hComponent,                                         \
        pBuffer)                        /* Macro End */

/** Structure representing a time stamp used with the following configs 
 * on the Clock Component (CC):
 * 
 * OMX_IndexConfigTimeCurrentWallTime: query of the CC's current wall
 *     time
 * OMX_IndexConfigTimeCurrentMediaTime: query of the CC's current media
 *     time
 * OMX_IndexConfigTimeCurrentAudioReference and  
 * OMX_IndexConfigTimeCurrentVideoReference: audio/video reference 
 *     clock sending SC its reference time
 * OMX_IndexConfigTimeClientStartTime: a Clock Component client sends 
 *     this structure to the Clock Component via a SetConfig on its 
 *     client port when it receives a buffer with
 *     OMX_BUFFERFLAG_STARTTIME set. It must use the timestamp
 *     specified by that buffer for nStartTimestamp. 
 *
 * It's also used with the following config on components in general:
 *
 * OMX_IndexConfigTimePosition: IL client querying component position 
 * (GetConfig) or commanding a component to seek to the given location
 * (SetConfig)
 */	
typedef struct COMP_TIME_CONFIG_TIMESTAMPTYPE {
    int nPortIndex;             /**< port that this structure applies to */
    int64_t nTimestamp;  	    /**< timestamp .*/ 
} COMP_TIME_CONFIG_TIMESTAMPTYPE;

#define CDX_ComponentNameDemux              "OMX.CedarX.demux"
#define CDX_ComponentNameVideoDecoder       "OMX.CedarX.videoDecoder"
#define CDX_ComponentNameAudioDecoder       "OMX.CedarX.audioDecoder"
#define CDX_ComponentNameSubtitleDecoder    "OMX.CedarX.subtitleDecoder"
#define CDX_ComponentNameVideoRender        "OMX.CedarX.videoRender"
#define CDX_ComponentNameAudioRender        "OMX.CedarX.audioRender"
#define CDX_ComponentNameSubtitleRender     "OMX.CedarX.subtitleRender"
#define CDX_ComponentNameClock              "OMX.CedarX.clock"
#define CDX_ComponentNameVideoSource        "OMX.CedarX.videoSource"
#define CDX_ComponentNameAudioSource        "OMX.CedarX.audioSource"
#define CDX_ComponentNameTextSource         "OMX.CedarX.textSource"
#define CDX_ComponentNameVideoEncoder       "OMX.CedarX.videoEncoder"
#define CDX_ComponentNameAudioEncoder       "OMX.CedarX.audioEncoder"
#define CDX_ComponentNameTextEncoder        "OMX.CedarX.textEncoder"
#define CDX_ComponentNameMuxer              "OMX.CedarX.muxer"
#define CDX_ComponentNameStreamDemux        "OMX.CedarX.streamDemux"

#define CDX_ComponentNameAIChannel          "OMX.CedarX.AIChannel"
#define CDX_ComponentNameAOChannel          "OMX.CedarX.AOChannel"

#define CDX_ComponentNameViScale        	"OMX.CedarX.ViScale"
#define CDX_ComponentNameVda        		"OMX.CedarX.Vda"
#define CDX_ComponentNameISE        		"OMX.CedarX.ISE"
#define CDX_ComponentNameEIS        		"OMX.CedarX.EIS"

#define CDX_ComponentNameUVCInput        	"OMX.CedarX.UVCInput"

typedef ERRORTYPE (*ComponentInit)(PARAM_IN COMP_HANDLETYPE hComponent);
typedef struct CDX_COMPONENTREGISTRY
{
    char*      name;							// Component name
    ComponentInit   comp_init_fn;           // create instance fn ptr
}CDX_COMPONENTREGISTRY;

typedef enum COMP_TUNNEL_TYPE{
	TUNNEL_TYPE_COMMON = 0x100,
	TUNNEL_TYPE_CLOCK ,
}COMP_TUNNEL_TYPE;

typedef struct COMP_INTERNAL_TUNNELINFOTYPE{
	unsigned int nPortIndex;
	COMP_HANDLETYPE hTunnel;
	unsigned int nTunnelPortIndex;
	COMP_TUNNEL_TYPE eTunnelType;
}COMP_INTERNAL_TUNNELINFOTYPE;

typedef struct CDX_TUNNELINFOTYPE{
	COMP_HANDLETYPE hTunnelMaster;
	COMP_HANDLETYPE hTunnelSlave;
	unsigned int nMasterPortIndex;
	unsigned int nSlavePortIndex;
	COMP_TUNNEL_TYPE eTunnelType;
	struct CDX_TUNNELINFOTYPE *hNext;
}CDX_TUNNELINFOTYPE;

typedef struct CDX_TUNNELLINKTYPE{
	CDX_TUNNELINFOTYPE *head;
	CDX_TUNNELINFOTYPE *tail;
}CDX_TUNNELLINKTYPE;

/** The OMX_GetHandle method will locate the component specified by the
    component name given, load that component into memory and then invoke
    the component's methods to create an instance of the component.

    The core should return from this call within 20 msec.

    @param [out] pHandle
        pointer to an COMP_HANDLETYPE pointer to be filled in by this method.
    @param [in] cComponentName
        pointer to a null terminated string with the component name.  The
        names of the components are strings less than 127 bytes in length
        plus the trailing null for a maximum size of 128 bytes.  An example
        of a valid component name is "OMX.TI.AUDIO.DSP.MIXER\0".  Names are
        assigned by the vendor, but shall start with "OMX." and then have
        the Vendor designation next.
    @param [in] pAppData
        pointer to an application defined value that will be returned
        during callbacks so that the application can identify the source
        of the callback.
    @param [in] pCallBacks
        pointer to a COMP_CALLBACKTYPE structure that will be passed to the
        component to initialize it with.
    @return ERRORTYPE
        If the command successfully executes, the return code will be
        OMX_ErrorNone.  Otherwise the appropriate OMX error will be returned.
    @ingroup core
 */
ERRORTYPE  COMP_GetHandle(
    PARAM_OUT COMP_HANDLETYPE* pHandle,
    PARAM_IN  char* cComponentName,
    PARAM_IN  void* pAppData,
    PARAM_IN  COMP_CALLBACKTYPE* pCallBacks);


/** The OMX_FreeHandle method will free a handle allocated by the OMX_GetHandle
    method.  If the component reference count goes to zero, the component will
    be unloaded from memory.

    The core should return from this call within 20 msec when the component is
    in the OMX_StateLoaded state.

    @param [in] hComponent
        Handle of the component to be accessed.  This is the component
        handle returned by the call to the GetHandle function.
    @return ERRORTYPE
        If the command successfully executes, the return code will be
        OMX_ErrorNone.  Otherwise the appropriate OMX error will be returned.
    @ingroup core
 */
ERRORTYPE  COMP_FreeHandle(
    PARAM_IN  COMP_HANDLETYPE hComponent);



/** The OMX_SetupTunnel method will handle the necessary calls to the components
    to setup the specified tunnel the two components.  NOTE: This is
    an actual method (not a #define macro).  This method will make calls into
    the component ComponentTunnelRequest method to do the actual tunnel
    connection.

    The ComponentTunnelRequest method on both components will be called.
    This method shall not be called unless the component is in the
    OMX_StateLoaded state except when the ports used for the tunnel are
    disabled. In this case, the component may be in the OMX_StateExecuting,
    OMX_StatePause, or OMX_StateIdle states.

    The core should return from this call within 20 msec.

    @param [in] hOutput
        Handle of the component to be accessed.  Also this is the handle
        of the component whose port, specified in the nPortOutput parameter
        will be used the source for the tunnel. This is the component handle
        returned by the call to the OMX_GetHandle function.  There is a
        requirement that hOutput be the source for the data when
        tunelling (i.e. nPortOutput is an output port).  If 0x0, the component
        specified in hInput will have it's port specified in nPortInput
        setup for communication with the application / IL client.
    @param [in] nPortOutput
        nPortOutput is used to select the source port on component to be
        used in the tunnel.
    @param [in] hInput
        This is the component to setup the tunnel with. This is the handle
        of the component whose port, specified in the nPortInput parameter
        will be used the destination for the tunnel. This is the component handle
        returned by the call to the OMX_GetHandle function.  There is a
        requirement that hInput be the destination for the data when
        tunelling (i.e. nPortInut is an input port).   If 0x0, the component
        specified in hOutput will have it's port specified in nPortPOutput
        setup for communication with the application / IL client.
    @param [in] nPortInput
        nPortInput is used to select the destination port on component to be
        used in the tunnel.
    @return ERRORTYPE
        If the command successfully executes, the return code will be
        OMX_ErrorNone.  Otherwise the appropriate OMX error will be returned.
        When OMX_ErrorNotImplemented is returned, one or both components is
        a non-interop component and does not support tunneling.

        On failure, the ports of both components are setup for communication
        with the application / IL Client.
    @ingroup core tun
 */
ERRORTYPE  COMP_SetupTunnel(
    PARAM_IN  COMP_HANDLETYPE hOutput,
    PARAM_IN  unsigned int nPortOutput,
    PARAM_IN  COMP_HANDLETYPE hInput,
    PARAM_IN  unsigned int nPortInput);

ERRORTYPE  COMP_ResetTunnel(
    PARAM_IN  COMP_HANDLETYPE hOutput,
    PARAM_IN  unsigned int nPortOutput,
    PARAM_IN  COMP_HANDLETYPE hInput,
    PARAM_IN  unsigned int nPortInput);

ERRORTYPE COMP_AddTunnelInfoChain(CDX_TUNNELLINKTYPE *cdx_tunnel_link,PARAM_IN  COMP_HANDLETYPE hOutput,PARAM_IN  unsigned int nPortOutput,PARAM_IN  COMP_HANDLETYPE hInput,PARAM_IN  unsigned int nPortInput);
ERRORTYPE COMP_DeleteTunnelInfoChain(CDX_TUNNELLINKTYPE *cdx_tunnel_link);
ERRORTYPE COMP_QueryTunnelInfoChain(PARAM_IN CDX_TUNNELLINKTYPE *cdx_tunnel_link, PARAM_IN COMP_HANDLETYPE master, PARAM_IN COMP_HANDLETYPE slave, PARAM_OUT CDX_TUNNELINFOTYPE *cdx_tunnel_info);
ERRORTYPE COMP_UpdateTunnelSlavePortDefine(PARAM_IN  COMP_HANDLETYPE hOutput,PARAM_IN  unsigned int nPortOutput,PARAM_IN  COMP_HANDLETYPE hInput,PARAM_IN  unsigned int nPortInput);

#define ERR_COMPCORE_NOMEM      DEF_ERR(MOD_ID_COMPCORE, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
#define ERR_COMPCORE_UNEXIST    DEF_ERR(MOD_ID_COMPCORE, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
#define ERR_COMPCORE_NOT_SUPPORT    DEF_ERR(MOD_ID_COMPCORE, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)


#define CompSendEvent(pComp, event, data1, data2)   \
    pComp->pCallbacks->EventHandler(pComp->hSelf, pComp->pAppData, event, data1, data2, NULL)

#ifdef __cplusplus
}
#endif /* End of #ifdef __cplusplus */

#endif  /* __IPCLINUX_MM_COMPONENT_H__ */

