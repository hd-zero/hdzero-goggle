/*! \file AVAPIs.h
This file describes all the APIs of the AV module in IOTC platform.
AV module is a kind of data communication modules in IOTC platform to provide
fluent streaming Audio / Video data from AV servers to AV clients in
unidirectional way.

\copyright Copyright (c) 2010 by Throughtek Co., Ltd. All Rights Reserved.

Revision Table

Version     | Name             |Date           |Description
------------|------------------|---------------|-------------------
 0.1.0.1    |Joshua            |2010-12-06     |Trial Version
 0.1.0.4    |Joshua            |2011-05-16     |modified avRecvFrameData2
 0.1.1.0    |Kevin             |2011-05-27     |Add send/resend audio capability
 0.1.2.0    |Kevin             |2011-06-30     |Adapt to IOTC multi-channel session mechanism
 1.0.0.0    |Kevin             |2011-07-12     |Formal version, stability testing
 1.0.1.0    |Kevin             |2011-08-02     |Fix Bug : avServStart() when check IOTC error code(-14,-22,-23) not release AvInfo
 1.0.2.0    |Kevin             |2011-08-08     |Fix Bug : 1.linux thread not reclaim so must detach thread 2.avoid avServStop() and avClientStop() to be called repeatedly
 1.1.0.0    |Kevin             |2011-09-14     |Modify : 1.allow set send frame delay time to 0, when set 0 will delay 1ms in every 100 frame interval 2.Max fifo size 256KB->2MB, max frame divided 256->512. Fix Bug : avRecvFrameData() and avRecvAudioData() if pnFrameIdx is NULL will crash. Custom : reduce static memory usage for ARC
 1.2.1.0    |Kevin             |2011-11-24     |Fix Bug : block_FifoExist() mutex dead lock. Add : 1. avServExit() to exit avServStart() anytime. 2. avClientExit() to exit avClientStart() anytime.
 1.2.2.0    |Kevin             |2011-12-29     |Fix Bug : 1.avSendIOCtrl() packet lost because not judge ack by frameNo. 2.Success in avServStart() and failed in avClientStart() when packet lost. Add : 1.avServStart2() use call back function for authentication. 2.avRecvIOCtrl() error code AV_ER_INVALID_ARG means give buf size not enough. 3.avSendIOCtrl() error code AV_ER_SENDIOCTRL_ALREADY_CALLED means cant use the same avIndex to multi-call. 4.avSendIOCtrlExit() force to exit avSendIOCtrl().
 1.2.3.0    |Kevin             |2012-02-22     |Fix Bug : avSendIOCtrl() packet lost because send and receive frameNo confuse.
 1.2.4.0    |Kevin             |2012-03-08     |Add : Big-endian SOC support.
 1.2.5.0    |Kevin             |2012-03-21     |Improve : av session can exceed 128, not limited.
 1.2.6.0    |Kevin             |2012-04-18     |Fix Bug : When create _AVthreadServRecv and _AVthreadServSendFrame thread pass local variable to it not thread-safe instead of alloc a variable to pass.
 1.2.7.0    |Kevin             |2012-05-22     |Fix Bug : avServStart() crash when connection failed because double tutk_platform_free "idx". Modify : TIME_DELAY_INITIAL 4->0 for video not delay.
 1.2.8.0    |Kevin             |2012-08-16     |+ Add deinitialization function: avDeInitialize() <br> + Add error code AV_ER_CLIENT_NO_AVLOGIN returned by avSendAudioData() <br> o Re-arrange error codes to distinguish from the ones defined by IOTC modules
 1.2.8.1    |Kevin             |2012-11-22     |o Add const modifier for input parameters in function prototype
 1.3.1.0    |Kevin, Harry      |2013-03-04     |+ Add new mechanism for receive frame function: avRecvFrameData2().<br> + Add avClientCleanBuf() to clean audio and video buffer.<br> + Add avServSetDelayInterval() for setting packets delay time of video frames.<br> o Fix avCheckAudioBuf() may returns wrong audio frames count.<br> o Fix the bug of avSendIOCtrl() returns AV_ER_SENDIOCTRL_ALREADY_CALLED after avSendIOCtrlExit() called.<br> o Fix memory leak in avServStop().<br> o Fix memory leak in avClientStop().<br> o Fix memory leak in avSendFrameData().
 1.3.2.0    |Kevin, Harry      |2013-03-28     |o Fix avServStart(), avServStart2() and avClientStart() may return wrong channel ID.<br> o Fix avDeInitialize() bug.
 1.3.3.0    |Kevin             |2013-04-10     |o Avoid avServStart(), avServStart2() and avClientStart() re-assign an AV channel ID for a pair of IOTC session ID and IOTC channel ID.
 1.4.0.0    |Kevin, Harry      |2013-05-17     |+ Add avServStart3(), avClientStart2() and avServSetResendSize() for AV re-send supporting.<br> + avClientCleanVideoBuf() to clean video buffer.<br> + avClientCleanAudioBuf() to clean audio buffer.
 1.4.1.0    |Kevin             |2013-06-25     |o Fix re-send mechanism sometimes halt and cause client can't receive video data anymore.<br> o Remove send and receive thread for server and client to improve performance.
 1.4.2.0    |Kevin, Victor     |2013-07-26     |o Fix resend mechanism sometimes halt forever and makes client can't receive video data anymore.<br> o Improve performance by remove send and receive thread for server and client.<br> o Fix argument type error.<br> o Fix Big-endian algorithm in avSendFrameData().<br> o Fix crash issue.
 1.4.2.3    |Alex, Victor      |2013-10-28     |o Fix avServStart2() and avServStart3() may blocks.<br> o Fix algorithm of avSendFrameData() on uCOSII.<br> o Fix critical session in uCOSII.<br> o Stop usage of AV_ER_EXCEED_MAX_ALARM (-20005).<br> o Fix big endian bugs.<br> o Add Lite UID support and AV_ER_NO_PERMISSION(-20023) error code.
 1.4.3.0    |Victor            |2013-11-19     |o Fix variables conflict error.
 1.4.4.0    |Victor            |2014-02-17     |+ Add new api avResendBufUsageRate() to know usage rate of resend buffer.
 1.4.5.0    |TUTK RD team      |2014-08-28     |o Fix crash bug on Win32.<br> o Fix client connection failed when packet lost.<br> - Remove include platform_config.h
 1.5.0.0    |TUTK RD team      |2014-09-22     |+ Add 64-bit library.
 1.5.1.0    |TUTK RD team      |2014-09-30     |+ Add new API avServResetBuffer to reset resend buffer.
 */


#ifndef _AVAPIs_H_
#define _AVAPIs_H_

/* ============================================================================
 * Platform Dependant Macro Definition
 * ============================================================================
 */

#ifdef _WIN32
/** @cond */
#ifdef IOTC_STATIC_LIB
#define AVAPI_API
#elif defined AVAPI_EXPORTS
#define AVAPI_API __declspec(dllexport)
#else
#define AVAPI_API __declspec(dllimport)
#endif // #ifdef P2PAPI_EXPORTS
/** @endcond */
#endif // #ifdef _WIN32

#ifdef IOTC_ARC_HOPE312
#define AVAPI_API
#define _stdcall
#endif // #ifdef IOTC_ARC_HOPE312

#if defined(__linux__) || defined (__APPLE__)
#define AVAPI_API
#define _stdcall
#endif // #ifdef __linux__

#ifdef IOTC_UCOSII
#define AVAPI_API
#define _stdcall
#endif // #ifdef IOTC_UCOSII

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* ============================================================================
 * Generic Macro Definition
 * ============================================================================
 */

/** The maximum size of AV IO Control data to be sent, in unit of byte. */
#define AV_MAX_IOCTRL_DATA_SIZE					1024

/** The maximum size of audio data to be sent, in unit of byte. */
#define AV_MAX_AUDIO_DATA_SIZE					1280

/* ============================================================================
 * Error Code Declaration
 * ============================================================================
 */
/** The function is performed successfully. */
#define		AV_ER_NoERROR						 0

/** The passed-in arguments for the function are incorrect */
#define		AV_ER_INVALID_ARG					-20000

/** The buffer to receive is too small to store */
#define		AV_ER_BUFPARA_MAXSIZE_INSUFF		-20001

/** The number of AV channels has reached maximum.
 * The maximum number of AV channels is determined by the passed-in
 * argument of avInitialize() */
#define		AV_ER_EXCEED_MAX_CHANNEL			-20002

/** Insufficient memory for allocation */
#define		AV_ER_MEM_INSUFF					-20003

/** AV fails to create threads. Please check if OS has ability to create threads for AV. */
#define		AV_ER_FAIL_CREATE_THREAD			-20004

/** A warning error code to indicate that the sending queue of video frame of an AV server
 * is almost full, probably caused by slow handling of an AV client or network
 * issue. Please note that this is just a warning, the video frame is actually
 * put in the queue. */
#define 	AV_ER_EXCEED_MAX_ALARM				-20005

/** The frame to be sent exceeds the currently remaining video frame buffer.
 * The maximum of video frame buffer is controlled by avServSetMaxBufSize() */
#define		AV_ER_EXCEED_MAX_SIZE				-20006

/** The specified AV server has no response */
#define		AV_ER_SERV_NO_RESPONSE				-20007

/** An AV client does not call avClientStart() yet */
#define		AV_ER_CLIENT_NO_AVLOGIN				-20008

/** The client fails in authentication due to incorrect view account or password */
#define		AV_ER_WRONG_VIEWACCorPWD			-20009

/** The IOTC session of specified AV channel is not valid */
#define		AV_ER_INVALID_SID					-20010

/** The specified timeout has expired during some operation */
#define		AV_ER_TIMEOUT						-20011

/** The data is not ready for receiving yet. */
#define		AV_ER_DATA_NOREADY					-20012

/** Some parts of a frame are lost during receiving */
#define		AV_ER_INCOMPLETE_FRAME				-20013

/** The whole frame is lost during receiving */
#define		AV_ER_LOSED_THIS_FRAME				-20014

/** The remote site already closes the IOTC session.
 * Please call IOTC_Session_Close() to release local IOTC session resource */
#define		AV_ER_SESSION_CLOSE_BY_REMOTE		-20015

/** This IOTC session is disconnected because remote site has no any response
 * after a specified timeout expires. */
#define		AV_ER_REMOTE_TIMEOUT_DISCONNECT		-20016

/** The AV server start process is terminated by avServExit() */
#define		AV_ER_SERVER_EXIT		    		-20017

/** The AV client start process is terminated by avClientExit() */
#define		AV_ER_CLIENT_EXIT		    		-20018

/** AV module has not been initialized */
#define		AV_ER_NOT_INITIALIZED	    		-20019

/** By design, an AV client cannot send frame and audio data to an AV server */
#define		AV_ER_CLIENT_NOT_SUPPORT	   		-20020

/** The AV channel of specified AV channel ID is already in sending IO control process */
#define		AV_ER_SENDIOCTRL_ALREADY_CALLED	   	-20021

/** The sending IO control process is terminated by avSendIOCtrlExit() */
#define		AV_ER_SENDIOCTRL_EXIT		    	-20022

/** The UID is a lite UID */
#define		AV_ER_NO_PERMISSION                     -20023

/** The length of password is wrong */
#define		AV_ER_WRONG_ACCPWD_LENGTH				-20024

/** IOTC session has been closed */
#define		AV_ER_IOTC_SESSION_CLOSED				-20025

/** IOTC is deinitialized */
#define		AV_ER_IOTC_DEINITIALIZED				-20026

/** IOTC channel is used by other av channel */
#define		AV_ER_IOTC_CHANNEL_IN_USED				-20027

/* ============================================================================
 * Enumeration Declaration
 * ============================================================================
 */

/**
 * \details The types for AV IO control. AV IO control are used	for AV servers
 *			and AV clients to communicate control message with each other.
 *			The types contains system defined types and user defined ones.
 *			The range of system defined	types is from 0x00 to 0xFF, while
 *			user defined ones is starting from 0x100.
 */

typedef enum
{
	/// Used by AV clients to tell AV servers to change the time interval
	/// of sending packets by avSendFrameData() when AV clients are too slow
	/// to handle frame data.
	IOTYPE_INNER_SND_DATA_DELAY = 0xFF,


	/// The starting enum value of user defined IO types. It is suggested
	/// to assign this to the first one of user defined types, like <br>
	/// enum IOTYPE_USER_DEFINED { <br>
	/// IOTYPE_USER_PLAY = IOTYPE_USER_DEFINED_START, <br>
	/// IOTYPE_USER_STOP <br>
	/// }
	IOTYPE_USER_DEFINED_START = 0x100
} AVIOCtrlType;

typedef enum _AV_RESET_TARGET
{
	RESET_VIDEO = 0,
	RESET_AUDIO,
	RESET_ALL
} AV_RESET_TARGET;


/* ============================================================================
 * Type Definition
 * ============================================================================
 */

/**
 * \details The prototype of authentication function used for an AV server.
 *			The authentication function is set when an AV server starts by avServStart2().
 *			The AV server will call back authentication function when an AV
 *			client wants to connect with szViewAccount and szViewAccount
 * \param szViewAccount [in] The view account provided by AV clients for authentication
 * \param szViewPassword [in] The view password provided by AV clients for authentication
 *
 * \return true if authentication passes and false if fails.
 *
 */
typedef int (_stdcall *authFn)(char *szViewAccount,char *szViewPassword);

/* ============================================================================
 * Function Declaration
 * ============================================================================
 */

/**
 * \brief Get the version of AV module
 *
 * \details This function returns the version of AV module.
 *
 * \return The version of AV module from high byte to low byte, for example,
 *			0x01020304 means the version is 1.2.3.4
 *
 * \see IOTC_Get_Version(), RDT_GetRDTApiVer()
 */
AVAPI_API int avGetAVApiVer(void);


/**
 * \brief Reset the Video or Audio buffer of AV server
 *
 * \details This function returns AV_ER_NoERROR if no error
 *
 * \param avIndex [in] The target avIndex
 * \param eTarget [in] Reset type could be RESET_VDEIO, RESET_AUDIO, RESET_ALL
 * \param Timeout_ms [in] The timeout value in ms to execute this function, set value 0 to wait forever
 *
 * \return This function returns AV_ER_NoERROR if no error
 *
 */
AVAPI_API int avServResetBuffer(int avIndex, AV_RESET_TARGET eTarget, unsigned int Timeout_ms);


/**
 * \brief Initialize AV module
 *
 * \details This function is used by AV servers or AV clients to initialize AV
 *			module and shall be called before any AV module related function
 *			is invoked.
 *
 * \param nMaxChannelNum [in] The max number of AV channels. If it is
 *			specified less than 1, AV will set max number of AV channels as 1.
 *
 * \return The actual maximum number of AV channels to be set.
 * \return Error code if return value < 0
 *			- #AV_ER_NO_PERMISSION the AV module does not support Lite UID
 *
 */
AVAPI_API int avInitialize(int nMaxChannelNum);


/**
 * \brief Deinitialize AV module
 *
 * \details This function will deinitialize AV module.
 *
 * \return #AV_ER_NoERROR if deinitialize successfully
 * \return Error code if return value < 0
 *			- #AV_ER_NOT_INITIALIZED the AV module is not initialized yet
 *
 * \attention AV module shall be deinitialized before IOTC module is
 *				deinitialized.
 */
AVAPI_API int avDeInitialize(void);


/**
 * \brief Start an AV server
 *
 * \details Start an AV server with predefined view account and password.
 *			Any AV client wanting to connect with this AV server shall
 *			provide matched view account and password.
 *
 * \param nIOTCSessionID [in] The session ID of the IOTC session to start AV server
 * \param cszViewAccount [in] The predefined view account
 * \param cszViewPassword [in] The predefined view password
 * \param nTimeout [in] The timeout for this function in unit of second
 *						Specify it as 0 will make AV server start process return
 *						until an AV client connects successfully.
 * \param nServType [in] The user-defined service type. An AV client will get
 *						this value when it invokes avClientStart() successfully.
 * \param nIOTCChannelID [in] The channel ID of the channel to start AV server
 *
 * \return AV channel ID if return value >= 0
 * \return Error code if return value < 0
 *			- #AV_ER_INVALID_ARG The IOTC session ID is incorrect or reuse of IOTC session ID
                and IOTC channel ID.
 *			- #AV_ER_NOT_INITIALIZED AV module is not initialized yet
 *			- #AV_ER_EXCEED_MAX_CHANNEL The number of AV channels has reached maximum
 *			- #AV_ER_FAIL_CREATE_THREAD Fails to create threads
 *			- #AV_ER_SESSION_CLOSE_BY_REMOTE The remote site already closes
 *				this IOTC session
 *			- #AV_ER_REMOTE_TIMEOUT_DISCONNECT The timeout expires because
 *				remote site has no response.
 *			- #AV_ER_INVALID_SID The IOTC session ID is not valid
 *			- #AV_ER_SERVER_EXIT Users stop this function with avServExit() in another thread
 *			- #AV_ER_TIMEOUT The timeout specified by nTimeout expires before
 *				AV start is performed completely
 *			- #AV_ER_NO_PERMISSION the AV module does not support Lite UID
 *
 * \see avServStart2(), avServExit(), avServStop()
 *
 * \attention (1) This function is a block process.<br><br>
 *				(2) The IOTC channel of	specified channel ID will be turned on automatically
 *				by avServStart()
 *
 */
AVAPI_API int  avServStart(int nIOTCSessionID, const char *cszViewAccount, const char *cszViewPassword, unsigned int nTimeout, unsigned int nServType, unsigned char nIOTCChannelID);


/**
 * \brief Start an AV server
 *
 * \details Start an AV server with user defined authentication function.
 *			Any AV client wanting to connect with this AV server shall
 *			pass the authentication with view account and password.
 *
 * \param nIOTCSessionID [in] The session ID of the IOTC session to create AV server
 * \param pfxAuthFn [in] The function pointer to an authentication function
 * \param nTimeout [in] The timeout for this function in unit of second.
 *						Specify it as 0 will make AV server start process return
 *						until an AV client connects successfully.
 * \param nServType [in] The user-defined service type. An AV client will get
 *						this value when it invokes avClientStart() successfully.
 * \param nIOTCChannelID [in] The channel ID of the channel to create AV server
 *
 * \return AV channel ID if return value >= 0
 * \return Error code if return value < 0
 *			- #AV_ER_INVALID_ARG The IOTC session ID is incorrect or reuse of IOTC session ID
                and IOTC channel ID.
 *			- #AV_ER_NOT_INITIALIZED AV module is not initialized yet
 *			- #AV_ER_EXCEED_MAX_CHANNEL The number of AV channels has reached maximum
 *			- #AV_ER_FAIL_CREATE_THREAD Fails to create threads
 *			- #AV_ER_SESSION_CLOSE_BY_REMOTE The remote site already closes
 *				this IOTC session
 *			- #AV_ER_REMOTE_TIMEOUT_DISCONNECT The timeout expires because
 *				remote site has no response.
 *			- #AV_ER_INVALID_SID The IOTC session ID is not valid
 *			- #AV_ER_SERVER_EXIT Users stop this function with avServExit() in another thread
 *			- #AV_ER_TIMEOUT The timeout specified by nTimeout expires before
 *				AV start is performed completely
 *			- #AV_ER_NO_PERMISSION the AV module does not support Lite UID
 *
 * \see avServStart(), avServExit(), avServStop()
 *
 * \attention (1) This function is a block process.<br><br>
 *			  (2) The IOTC channel of	specified channel ID will be turned on automatically
 *				by avServStart2()
 *
 */
AVAPI_API int  avServStart2(int nIOTCSessionID, authFn pfxAuthFn, unsigned int nTimeout, unsigned int nServType, unsigned char nIOTCChannelID);

/**
 * \brief Start an AV server
 *
 * \details Start an AV re-send supported server with user defined authentication function.
 *			Any AV client wanting to connect with this AV server shall
 *			pass the authentication with view account and password. Whether the re-send mechanism
 *          is enabled or not depends on AV client settings and will set the result into
 *          pnResend parameter.
 *
 * \param nIOTCSessionID [in] The session ID of the IOTC session to create AV server
 * \param pfxAuthFn [in] The function pointer to an authentication function
 * \param nTimeout [in] The timeout for this function in unit of second.
 *						Specify it as 0 will make AV server start process return
 *						until an AV client connects successfully.
 * \param nServType [in] The user-defined service type. An AV client will get
 *                       this value when it invokes avClientStart() successfully.
 * \param nIOTCChannelID [in] The channel ID of the channel to create AV server
 *
 * \param pnResend [out] Set the re-send is enabled or not.
 *
 * \return AV channel ID if return value >= 0
 * \return Error code if return value < 0
 *			- #AV_ER_INVALID_ARG The IOTC session ID is incorrect or reuse of IOTC session ID
 *               and IOTC channel ID.
 *			- #AV_ER_NOT_INITIALIZED AV module is not initialized yet
 *			- #AV_ER_EXCEED_MAX_CHANNEL The number of AV channels has reached maximum
 *			- #AV_ER_FAIL_CREATE_THREAD Fails to create threads
 *			- #AV_ER_SESSION_CLOSE_BY_REMOTE The remote site already closes
 *				this IOTC session
 *			- #AV_ER_REMOTE_TIMEOUT_DISCONNECT The timeout expires because
 *				remote site has no response.
 *			- #AV_ER_INVALID_SID The IOTC session ID is not valid
 *			- #AV_ER_SERVER_EXIT Users stop this function with avServExit() in another thread
 *			- #AV_ER_TIMEOUT The timeout specified by nTimeout expires before
 *				AV start is performed completely
 *			- #AV_ER_NO_PERMISSION the AV module does not support Lite UID
 *
 * \see avServStart(), avServStart2(), avServExit(), avServStop()
 *
 * \attention (1) This function is a block process.<br><br>
 *			  (2) The IOTC channel of specified channel ID will be turned on automatically
 *			 	   by avServStart3()
 *
 */
AVAPI_API int  avServStart3(int nIOTCSessionID, authFn pfxAuthFn, unsigned int nTimeout, unsigned int nServType, unsigned char nIOTCChannelID, int *pnResend);


/**
 * \brief Used by an AV server exit avServStart() or avServStart2() process
 *
 * \details Since avServStart() and avServStart2() are block processes and
 *			that means the caller has to wait for AV start or specified timeout
 *			expires	before these two functions return. In some cases,
 *			caller may want	to exit AV start process immediately by this
 *			function in another thread.
 *
 * \param nIOTCSessionID [in] The session ID of the IOTC session to exit AV start process
 * \param nIOTCChannelID [in] The channel ID of the channel to exit AV start process
 *
 */
AVAPI_API void avServExit(int nIOTCSessionID, unsigned char nIOTCChannelID);


/**
 * \brief Stop an AV server
 *
 * \details An AV server stop AV channel by this function if this channel is
 *			no longer required.
 *
 * \param nAVChannelID [in] The channel ID of the AV channel to be stopped
 *
 */
AVAPI_API void avServStop(int nAVChannelID);

/**
 * \brief Set re-send buffer size.
 *
 * \details Use this API to set the re-send buffer size if re-send mechanism is enabled.
 *          Default re-send buffer size is 256KB and recommend size is 1 second data.
 *
 * \param nAVChannelID [in] The channel ID of the AV channel to be set.
 * \param nSize [in] The size of re-send buffer, in unit of kilo-byte.
 *
 */
AVAPI_API void avServSetResendSize(int nAVChannelID, unsigned int nSize);


/**
 * \brief An AV server sends frame data to an AV client
 *
 * \details An AV server uses this function to send frame data to AV client
 *
 * \param nAVChannelID [in] The channel ID of the AV channel to be sent
 * \param cabFrameData [in] The frame data to be sent
 * \param nFrameDataSize [in] The size of the frame data
 * \param cabFrameInfo [in] The video frame information to be sent
 * \param nFrameInfoSize [in] The size of the video frame information
 *
 * \return #AV_ER_NoERROR if sending successfully
 * \return Error code if return value < 0
 *			- #AV_ER_INVALID_ARG The AV channel ID is not valid or frame data is null
 *			- #AV_ER_CLIENT_NOT_SUPPORT An AV client uses this function to send frame data
 *			- #AV_ER_SESSION_CLOSE_BY_REMOTE The remote site already closes
 *				this IOTC session
 *			- #AV_ER_REMOTE_TIMEOUT_DISCONNECT The timeout expires because
 *				remote site has no response.
 *			- #AV_ER_INVALID_SID The IOTC session of this AV channel is not valid
 *			- #AV_ER_CLIENT_NO_AVLOGIN An AV client does not pass authentication yet
 *			- #AV_ER_EXCEED_MAX_SIZE The frame data and frame info to be sent exceeds
 *				currently remaining video frame buffer. The max size of video frame
 *				buffer is determined by avServSetMaxBufSize()
 *			- #AV_ER_MEM_INSUFF Insufficient memory for allocation
 *			- #AV_ER_NO_PERMISSION the AV module does not support Lite UID
 *
 * \see avSendAudioData()
 *
 */
AVAPI_API int  avSendFrameData(int nAVChannelID, const char *cabFrameData, int nFrameDataSize,
								const void *cabFrameInfo, int nFrameInfoSize);


/**
 * \brief An AV server sends audio data to an AV client
 *
 * \details An AV server uses this function to send audio data to AV client
 *
 * \param nAVChannelID [in] The channel ID of the AV channel to be sent
 * \param cabAudioData [in] The audio data to be sent
 * \param nAudioDataSize [in] The size of the audio data
 * \param cabFrameInfo [in] The audio frame information to be sent
 * \param nFrameInfoSize [in] The size of the audio frame information
 *
 * \return #AV_ER_NoERROR if sending successfully
 * \return Error code if return value < 0
 *			- #AV_ER_INVALID_ARG The AV channel ID is not valid or frame data is null
 *			- #AV_ER_CLIENT_NOT_SUPPORT An AV client uses this function to send frame data
 *			- #AV_ER_SESSION_CLOSE_BY_REMOTE The remote site already closes
 *				this IOTC session
 *			- #AV_ER_REMOTE_TIMEOUT_DISCONNECT The timeout expires because
 *				remote site has no response.
 *			- #AV_ER_INVALID_SID The IOTC session of this AV channel is not valid
 *			- #AV_ER_CLIENT_NO_AVLOGIN An AV client does not pass authentication yet
 *			- #AV_ER_MEM_INSUFF Insufficient memory for allocation
 *			- #AV_ER_EXCEED_MAX_SIZE The audio data and frame info to be sent exceeds
 *				#AV_MAX_AUDIO_DATA_SIZE
*			- #AV_ER_NO_PERMISSION the AV module does not support Lite UID
 *
 * \see avSendFrameData()
 *
 */
AVAPI_API int  avSendAudioData(int nAVChannelID, const char *cabAudioData, int nAudioDataSize,
								const void *cabFrameInfo, int nFrameInfoSize);


/**
 * \brief Set interval of sending video data in AV server
 *
 * \details An AV server how to send video data to AV client.
 *			It determined the delay time at regular interval between how many packets.
 *
 * \param nAVChannelID [in] The channel ID of the AV channel to be sent
 * \param nPacketNum [in] How many number of packet as a regular interval
 * \param nDelayMs [in] Delay time in unit of million-second
 *
 * \return #AV_ER_NoERROR if set successfully
 * \return Error code if return value < 0
 *			- #AV_ER_INVALID_ARG The AV channel ID is not valid
 *
 *
 */
AVAPI_API int avServSetDelayInterval(int nAVChannelID, unsigned short nPacketNum, unsigned short nDelayMs);


/**
 * \brief Start an AV client
 *
 * \details Start an AV client by providing view account and password.
 *			It shall pass the authentication of the AV server before receiving
 *			AV data.
 *
 * \param nIOTCSessionID [in] The session ID of the IOTC session to start AV client
 * \param cszViewAccount [in] The view account for authentication
 * \param cszViewPassword [in] The view password for authentication
 * \param nTimeout [in] The timeout for this function in unit of second
 *						Specify it as 0 will make this AV client try connection
 *						once and this process will exit immediately if not
 *						connection is unsuccessful.
 * \param pnServType [out] The user-defined service type set when an AV server
 *						starts. Can be NULL.
 * \param nIOTCChannelID [in] The channel ID of the channel to start AV client
 *
 * \return AV channel ID if return value >= 0
 * \return Error code if return value < 0
 *			- #AV_ER_INVALID_ARG The IOTC session ID is incorrect or reuse of IOTC session ID
 *              and IOTC channel ID.
 *			- #AV_ER_NOT_INITIALIZED AV module is not initialized yet
 *			- #AV_ER_EXCEED_MAX_CHANNEL The number of AV channels has reached maximum
 *			- #AV_ER_FAIL_CREATE_THREAD Fails to create threads
 *			- #AV_ER_SERV_NO_RESPONSE The AV server has no response
 *			- #AV_ER_SESSION_CLOSE_BY_REMOTE The remote site already closes
 *				this IOTC session
 *			- #AV_ER_REMOTE_TIMEOUT_DISCONNECT The timeout expires because
 *				remote site has no response.
 *			- #AV_ER_INVALID_SID The IOTC session ID is not valid
 *			- #AV_ER_CLIENT_EXIT Users stop this function with avClientExit() in another thread
 *			- #AV_ER_TIMEOUT The timeout specified by nTimeout expires before
 *				AV start is performed completely
 *			- #AV_ER_WRONG_VIEWACCorPWD The client fails in authentication due
 *				to incorrect view account or password
 *			- #AV_ER_NO_PERMISSION the AV module does not support Lite UID
 *
 * \see avClientStop(), avClientExit()
 *
 * \attention (1) This function is a block process.<br><br>
 *				 (2) The IOTC channel of specified channel ID will be turned on automatically
 *				by avClientStart().
 *
 */
AVAPI_API int  avClientStart(int nIOTCSessionID, const char *cszViewAccount, const char *cszViewPassword,
								unsigned int nTimeout,unsigned int *pnServType, unsigned char nIOTCChannelID);

/**
 * \brief Start an AV client
 *
 * \details Start an AV re-send supported client by providing view account and password.
 *			It shall pass the authentication of the AV server before receiving
 *			AV data. Whether the re-send mechanism is enabled or not depends on AV server settings
 *          and will set the result into pnResend parameter.
 *
 * \param nIOTCSessionID [in] The session ID of the IOTC session to start AV client
 * \param cszViewAccount [in] The view account for authentication
 * \param cszViewPassword [in] The view password for authentication
 * \param nTimeout [in] The timeout for this function in unit of second
 *						Specify it as 0 will make this AV client try connection
 *						once and this process will exit immediately if not
 *						connection is unsuccessful.
 * \param pnServType [out] The user-defined service type set when an AV server
 *						starts. Can be NULL.
 * \param nIOTCChannelID [in] The channel ID of the channel to start AV client
 * \param pnResend [out] The re-send is enabled or not.
 *
 * \return AV channel ID if return value >= 0
 * \return Error code if return value < 0
 *			- #AV_ER_INVALID_ARG The IOTC session ID is incorrect or reuse of IOTC session ID
                and IOTC channel ID.
 *			- #AV_ER_NOT_INITIALIZED AV module is not initialized yet
 *			- #AV_ER_EXCEED_MAX_CHANNEL The number of AV channels has reached maximum
 *			- #AV_ER_FAIL_CREATE_THREAD Fails to create threads
 *			- #AV_ER_SERV_NO_RESPONSE The AV server has no response
 *			- #AV_ER_SESSION_CLOSE_BY_REMOTE The remote site already closes
 *				this IOTC session
 *			- #AV_ER_REMOTE_TIMEOUT_DISCONNECT The timeout expires because
 *				remote site has no response.
 *			- #AV_ER_INVALID_SID The IOTC session ID is not valid
 *			- #AV_ER_CLIENT_EXIT Users stop this function with avClientExit() in another thread
 *			- #AV_ER_TIMEOUT The timeout specified by nTimeout expires before
 *				AV start is performed completely
 *			- #AV_ER_WRONG_VIEWACCorPWD The client fails in authentication due
 *				to incorrect view account or password
 *			- #AV_ER_NO_PERMISSION the AV module does not support Lite UID
 *
 * \see avClientStop(), avClientExit()
 *
 * \attention (1) This function is a block process.<br><br>
 *			  (2) The IOTC channel of specified channel ID will be turned on automatically
 *				  by avClientStart2().<br><br>
 *            (3) If AV client uses avClientStart2() to enable AV re-send mechanism, it has
 *                to use avRecvFrameData2() to receive video data.
 *
 */
AVAPI_API int  avClientStart2(int nIOTCSessionID, const char *cszViewAccount, const char *cszViewPassword,
								unsigned int nTimeout,unsigned int *pnServType, unsigned char nIOTCChannelID, int *pnResend);

/**
 * \brief Used by an AV client exit avClientStart() process
 *
 * \details Since avClientStart() is a block process and
 *			that means the caller has to wait for AV start or specified timeout
 *			expires	before these two functions return. In some cases,
 *			caller may want	to exit AV start process immediately by this
 *			function in another thread.
 *
 * \param nIOTCSessionID [in] The session ID of the IOTC session to exit AV start process
 * \param nIOTCChannelID [in] The channel ID of the channel to exit AV start process
 *
 */
AVAPI_API void avClientExit(int nIOTCSessionID, unsigned char nIOTCChannelID);


/**
 * \brief Stop an AV client
 *
 * \details An AV client stop AV channel by this function if this channel is
 *			no longer required.
 *
 * \param nAVChannelID [in] The channel ID of the AV channel to be stopped
 *
 */
AVAPI_API void avClientStop(int nAVChannelID);


/**
 * \brief An AV client receives frame data from an AV server
 *
 * \details An AV client uses this function to receive frame data from AV server
 *
 * \param nAVChannelID [in] The channel ID of the AV channel to be received
 * \param abFrameData [out] The frame data to be received
 * \param nFrameDataMaxSize [in] The max size of the frame data
 * \param abFrameInfo [out] The video frame information to be received
 * \param nFrameInfoMaxSize [in] The max size of the video frame information
 * \param pnFrameIdx [out] The index of current receiving video frame
 *
 * \return The actual length of received result stored in abFrameData if successfully
 * \return Error code if return value < 0
 *			- #AV_ER_INVALID_ARG The AV channel ID is not valid or frame data is null
 *			- #AV_ER_SESSION_CLOSE_BY_REMOTE The remote site already closes
 *				this IOTC session
 *			- #AV_ER_REMOTE_TIMEOUT_DISCONNECT The timeout expires because
 *				remote site has no response.
 *			- #AV_ER_INVALID_SID The IOTC session of this AV channel is not valid
 *			- #AV_ER_DATA_NOREADY The data is not ready for receiving yet.
 *			- #AV_ER_LOSED_THIS_FRAME The whole frame is lost during receiving
 *			- #AV_ER_BUFPARA_MAXSIZE_INSUFF The frame to be received exceeds
 *				the size of abFrameData, i.e. nFrameDataMaxSize
 *			- #AV_ER_MEM_INSUFF Insufficient memory for allocation
 *			- #AV_ER_INCOMPLETE_FRAME Some parts of a frame are lost during receiving
 *			- #AV_ER_NO_PERMISSION the AV module does not support Lite UID
 *
 * \see avRecvAudioData()
 *
 */
AVAPI_API int avRecvFrameData(int nAVChannelID, char *abFrameData, int nFrameDataMaxSize,
								char *abFrameInfo, int nFrameInfoMaxSize, unsigned int *pnFrameIdx);

/**
 * \brief A new version AV client receives frame data from an AV server
 *
 * \details An AV client uses this function to receive frame data from AV server
 *
 * \param nAVChannelID [in] The channel ID of the AV channel to be received
 * \param abFrameData [out] The frame data to be received
 * \param nFrameDataMaxSize [in] The max size of the frame data
 * \param pnActualFrameSize [in] The actual size of frame data to be received, maybe less than expected size
 * \param pnExpectedFrameSize [in] The size of frame data expect to be received that sent from av server
 * \param abFrameInfo [out] The video frame information to be received
 * \param nFrameInfoMaxSize [in] The max size of the video frame information
 * \param pnActualFrameInfoSize [in] The actual size of the video frame information to be received
 * \param pnFrameIdx [out] The index of current receiving video frame
 *
 * \return The actual length of received result stored in abFrameData if successfully
 * \return Error code if return value < 0
 *			- #AV_ER_INVALID_ARG The AV channel ID is not valid or frame data is null
 *			- #AV_ER_SESSION_CLOSE_BY_REMOTE The remote site already closes
 *				this IOTC session
 *			- #AV_ER_REMOTE_TIMEOUT_DISCONNECT The timeout expires because
 *				remote site has no response.
 *			- #AV_ER_INVALID_SID The IOTC session of this AV channel is not valid
 *			- #AV_ER_DATA_NOREADY The data is not ready for receiving yet.
 *			- #AV_ER_LOSED_THIS_FRAME The whole frame is lost during receiving
 *			- #AV_ER_BUFPARA_MAXSIZE_INSUFF The frame to be received exceeds
 *				the size of abFrameData, i.e. nFrameDataMaxSize
 *			- #AV_ER_MEM_INSUFF Insufficient memory for allocation
 *			- #AV_ER_INCOMPLETE_FRAME Some parts of a frame are lost during receiving
 *			- #AV_ER_NO_PERMISSION the AV module does not support Lite UID
 *
 * \see avRecvAudioData()
 *
 */
AVAPI_API int avRecvFrameData2(int nAVChannelID, char *abFrameData, int nFrameDataMaxSize, int *pnActualFrameSize,
								int *pnExpectedFrameSize, char *abFrameInfo, int nFrameInfoMaxSize,
								int *pnActualFrameInfoSize, unsigned int *pnFrameIdx);

/**
 * \brief An AV client receives audio data from an AV server
 *
 * \details An AV client uses this function to receive audio data from AV server
 *
 * \param nAVChannelID [in] The channel ID of the AV channel to be received
 * \param abAudioData [out] The audio data to be received
 * \param nAudioDataMaxSize [in] The max size of the audio data
 * \param abFrameInfo [out] The audio frame information to be received
 * \param nFrameInfoMaxSize [in] The max size of the audio frame information
 * \param pnFrameIdx [out] The index of current receiving audio frame
 *
 * \return The actual length of received result stored in abAudioData if successfully
 * \return Error code if return value < 0
 *			- #AV_ER_INVALID_ARG The AV channel ID is not valid or frame data is null
 *			- #AV_ER_SESSION_CLOSE_BY_REMOTE The remote site already closes
 *				this IOTC session
 *			- #AV_ER_REMOTE_TIMEOUT_DISCONNECT The timeout expires because
 *				remote site has no response.
 *			- #AV_ER_INVALID_SID The IOTC session of this AV channel is not valid
 *			- #AV_ER_DATA_NOREADY The data is not ready for receiving yet.
 *			- #AV_ER_LOSED_THIS_FRAME The whole frame is lost during receiving
 *			- #AV_ER_BUFPARA_MAXSIZE_INSUFF The data and frame info to be received
 *				exceeds	the size of abAudioData and abFrameInfo, respectively.
 *			- #AV_ER_NO_PERMISSION the AV module does not support Lite UID
 *
 * \see avRecvFrameData()
 *
 */
AVAPI_API int avRecvAudioData(int nAVChannelID, char *abAudioData, int nAudioDataMaxSize,
								char *abFrameInfo, int nFrameInfoMaxSize, unsigned int *pnFrameIdx);


/**
 * \brief Get the frame count of audio buffer remaining in the queue
 *
 * \details An AV client uses this function to get the frame count of audio buffer
 *			that is still remaining in the receiving queue, then determine
 *			whether it is a right time to invoke avRecvAudioData().
 *			Keeping audio buffer in audio queue instead of receiving immediately
 *			can greatly improve the discontinuous audio issues. However,
 *			it is not good to receive audio in a very long time since audio
 *			buffer will overflow and audio data will be lost.
 *
 * \param nAVChannelID [in] The channel ID of the AV channel to check audio buffer
 *
 * \return The frame count of audio buffer
 */
AVAPI_API int avCheckAudioBuf(int nAVChannelID);


/**
 * \brief Set the maximum video frame buffer used in AV client
 *
 * \details AV client sets the maximum video frame buffer by this function.
 *			The size of video frame buffer will affect the streaming fluency.
 *			The default size of video frame buffer is 1MB.
 *
 * \param nMaxBufSize The maximum video frame buffer, in unit of kilo-byte
 *
 */
AVAPI_API void avClientSetMaxBufSize(unsigned int nMaxBufSize);


/**
 * \brief Clean the video buffer both in client and device, and clean the audio buffer of the client.
 *
 * \details A client with multiple device connection application should call
 *			this function to clean AV buffer while switch to another devices.
 *
 * \param nAVChannelID [in] The channel ID of the AV channel to clean buffer
 *
 * \return Error code if return value < 0
 *			- #AV_ER_INVALID_ARG The AV channel ID is not valid or frame data is null
 */
AVAPI_API int avClientCleanBuf(int nAVChannelID);


/**
 * \brief Clean the video buffer(both in client and device)
 *
 * \details A client with multiple device connection application should call
 *			this function to clean video buffer while switch to another devices.
 *
 * \param nAVChannelID [in] The channel ID of the AV channel to clean buffer
 *
 * \return Error code if return value < 0
 *			- #AV_ER_INVALID_ARG The AV channel ID is not valid or frame data is null
 */
AVAPI_API int avClientCleanVideoBuf(int nAVChannelID);

/**
* \brief Clean the local video and audio buffer of the client
*
* \details This function is used to clean the video and audio buffer that the client
*			has already received
*
* \param nAVChannelID [in] The channel ID of the AV channel to clean buffer
*
* \return Error code if return value < 0
*			- #AV_ER_INVALID_ARG The AV channel ID is not valid or frame data is null
*/
AVAPI_API int avClientCleanLocalBuf(int nAVChannelID);

/**
* \brief Clean the local video buffer of the client
*
* \details This function is used to clean the video buffer that the client
*			has already received
*
* \param nAVChannelID [in] The channel ID of the AV channel to clean buffer
*
* \return Error code if return value < 0
*			- #AV_ER_INVALID_ARG The AV channel ID is not valid or frame data is null
*/
AVAPI_API int avClientCleanLocalVideoBuf(int nAVChannelID);
/**
 * \brief Clean the local audio buffer of the client
 *
 * \details A client with multiple device connection application should call
 *			this function to clean audio buffer while switch listen to speaker.
 *
 * \param nAVChannelID [in] The channel ID of the audio channel to clean buffer
 *
 * \return Error code if return value < 0
 *			- #AV_ER_INVALID_ARG The AV channel ID is not valid or frame data is null
 */
AVAPI_API int avClientCleanAudioBuf(int nAVChannelID);


/**
 * \brief Send AV IO control
 *
 * \details This function is used by AV servers or AV clients to send a
 *			AV IO control.
 *
 * \param nAVChannelID [in] The channel ID of the AV channel to send IO control
 * \param nIOCtrlType [in] The type of IO control
 * \param cabIOCtrlData [in] The buffer of IO control data
 * \param nIOCtrlDataSize [in] The length of IO control data
 *
 * \return #AV_ER_NoERROR if sending successfully
 * \return Error code if return value < 0
 *			- #AV_ER_INVALID_ARG The AV channel ID is not valid
 *			- #AV_ER_SENDIOCTRL_ALREADY_CALLED This AV channel is already in sending
 *				IO control process
 *			- #AV_ER_SESSION_CLOSE_BY_REMOTE The remote site already closes
 *				this IOTC session
 *			- #AV_ER_REMOTE_TIMEOUT_DISCONNECT The timeout expires because
 *				remote site has no response.
 *			- #AV_ER_INVALID_SID The IOTC session of this AV channel is not valid
 *			- #AV_ER_SENDIOCTRL_EXIT avSendIOCtrlExit() is called before this
 *				function is returned
 *			- #AV_ER_EXCEED_MAX_SIZE The IO control data and type to be sent exceeds
 *				#AV_MAX_IOCTRL_DATA_SIZE
 *			- #AV_ER_NO_PERMISSION the AV module does not support Lite UID
 *
 * \see avRecvIOCtrl(), avSendIOCtrlExit()
 *
 * \attention This function is a block process and it will return until
 *				having acknowledgment from the receiver.
 */
AVAPI_API int avSendIOCtrl(int nAVChannelID, unsigned int nIOCtrlType, const char *cabIOCtrlData, int nIOCtrlDataSize);


/**
 * \brief Receive AV IO control
 *
 * \details This function is used by AV servers or AV clients to receive a
 *			AV IO control.
 *
 * \param nAVChannelID [in] The channel ID of the AV channel to receive IO control
 * \param pnIOCtrlType [out] The type of received IO control
 * \param abIOCtrlData [out] The buffer of received IO control data
 * \param nIOCtrlMaxDataSize [in] The max length of buffer of received IO control data
 * \param nTimeout [in] The timeout for this function in unit of million-second, give 0 means return immediately
 *
 * \return The actual length of received result stored in abIOCtrlData if
 *			receiving IO control successfully
 * \return Error code if return value < 0
 *			- #AV_ER_INVALID_ARG The AV channel ID is not valid or IO control type
 *				/ data is null
 *			- #AV_ER_SESSION_CLOSE_BY_REMOTE The remote site already closes
 *				this IOTC session
 *			- #AV_ER_REMOTE_TIMEOUT_DISCONNECT The timeout expires because
 *				remote site has no response.
 *			- #AV_ER_INVALID_SID The IOTC session of this AV channel is not valid
 *			- #AV_ER_DATA_NOREADY The IO control is not ready for receiving yet.
 *			- #AV_ER_TIMEOUT The timeout specified by nTimeout expires before
 *				receiving IO control process is performed completely
 *			- #AV_ER_BUFPARA_MAXSIZE_INSUFF The IO control data to be received exceeds
 *				the size of abIOCtrlData, i.e. nIOCtrlMaxDataSize
 *			- #AV_ER_NO_PERMISSION the AV module does not support Lite UID
 *
 * \see avSendIOCtrl()
 *
 */
AVAPI_API int avRecvIOCtrl(int nAVChannelID, unsigned int *pnIOCtrlType, char *abIOCtrlData, int nIOCtrlMaxDataSize, unsigned int nTimeout);


/**
 * \brief Used by an AV server or an AV client to exit sending IO control process
 *
 * \details Since avSendIOCtrl() process is a block process and that means
 *			the caller has to wait for the acknowledgment from the receipt
 *			before avSendIOCtrl() returns. In some cases, caller may want
 *			to exit sending immediately by this function in another thread.
 *
 * \param nAVChannelID [in] The channel ID of the AV channel in sending IO control
 *
 * \return #AV_ER_NoERROR if sending successfully
 * \return Error code if return value < 0
 *			- #AV_ER_INVALID_ARG The AV channel ID is not valid
 */
AVAPI_API int avSendIOCtrlExit(int nAVChannelID);

/**
 * \brief Used by an AV server, in order to know the usage rate of resend buffer
 *
 * \details AV server calls this function to know the usage rate of resend buffer.
 *          It will return a float value between 0 and 1.
 *          0 means that resend buffer is empty, 1 means that resend buffer is full.
 *          The others mean usage rate.
 *
 * \param nAVChannelID [in] The channel ID of the AV channel
 *
 * \return Usage rate of resend buffer, if return value >= 0.
 * \return Error code if return value < 0
 *          - #AV_ER_INVALID_ARG The AV channel ID is not valid
 */
AVAPI_API float avResendBufUsageRate (int nAVChannelID);

/**
* \brief Set path of log file
*
* \details Set the absolute path of log file
*
* \param path [in] The path of log file, NULL = disable Log
*
* \param nMaxSize [in] The maximum size of log file in Bytes, 0 = unlimit
*
*/
AVAPI_API void AV_Set_Log_Path(char *path, int nMaxSize);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* _AVAPIs_H_ */
