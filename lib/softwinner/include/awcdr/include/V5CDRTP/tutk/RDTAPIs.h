/*! \file RDTAPIs.h
This file describes all the APIs of the RDT module in IOTC platform.
RDT module is a kind of data communication modules in IOTC platform to provide
Reliable Data Transfer among RDT servers and RDT clients in bi-directional way.

\copyright Copyright (c) 2010 by Throughtek Co., Ltd. All Rights Reserved.

Revision Table

Version     | Name             |Date           |Description
------------|------------------|---------------|-------------------
 0.1        |Joshua            |2011-03-29     |Trial Version
 0.1.1.0    |Kevin             |2011-06-07     |Workable Version
 0.1.2.0    |Kevin             |2011-06-30     |Adapt to IOTC multi-channel session mechanism
 1.0.0.0    |Kevin             |2011-07-12     |Formal version, stability testing
 1.1.0.0    |Charlie & Kevin   |2011-08-29     |New formal version, can't be compatible with below version 1.1.0.0
 1.2.0.0    |Kevin             |2011-10-03     |Add : 1.RDTInfo_t struct member "ErrorCode" to report IOTC session error 2.RDT_Status_Check() to know timeout status etc. Fix Bug : ___ListItem_SendList_RemovebyType() function will cause double tutk_platform_free or use memory already be freed. Improve : destroy procedure no need to wait until timeout
 1.3.0.0    |Kevin             |2011-11-15     |Add : Control network flow to fit bandwidth dynamically. Improve : when RDT_Destroy() call set timeout to 2 sec.
 1.3.1.0    |Kevin             |2011-11-28     |Fix Bug : RDT_Destroy() not add ID will cause ack to remove wrong packet. Improve : Send data via internet have good performance.
 1.3.2.0    |Kevin             |2011-12-19     |Fix Bug : RDT_Destroy() not release RDT_ID. Add : 1.Big-endian SOC suport. Improve : 1.Send data via Lan have good performance. 2.Reduce CPU usage.
 1.3.3.0    |Kevin             |2012-02-04     |Fix Bug : 1.Floating exception when WAN mode. 2.Access NULL pointer in send thread when WAN mode. 3.BufSizeInSendQueue info error.
 1.3.4.0    |Kevin             |2012-03-30     |Fix Bug : When one site call RDT_Destroy() will cause remote site put FIN packet into sendList and lost data packet to send. Modify : RDT_Read() return read size and if data come in return immediately.
 1.3.5.0    |Kevin             |2012-08-16     |+ Add error code RDT_ER_RCV_DATA_END for RDT_Read() to indicate remote site has destroyed the RDT channel<br> o Re-arrange error codes to distinguish from the ones defined by IOTC modules<br> o Modify API name from RDT_Set_Max_Session_Number() to RDT_Set_Max_Channel_Number() for unifying naming rule <br> - Remove error codes: RDT_ER_INVALID_ARG, RDT_ER_IOTC_SENDTO_FAILED, RDT_ER_IOTC_SESSION_CLOSE_BY_REMOTE, RDT_ER_IOTC_REMOTE_TIMEOUT_DISCONNECT, RDT_ER_IOTC_CH_NOT_ON
 1.3.7.0    |Kevin             |2012-10-12     |o Improve RDT transfer rate <br> o Add const modifier for input parameters in function prototype
 1.3.8.0    |Han-Lin, Harry    |2013-03-28     |+ Add RDT_Set_Log_Path() to set log file name, path and maximum size.<br> + Log error codes in specified file.<br> o Modify RDT connection timeout from 10 to 20 seconds.
 1.3.9.0    |Han-Lin, Harry    |2013-04-17     |o Modify congestion control, dynamic window size and timeout.
 1.3.10.0   |Han-Lin, Harry    |2013-05-07     |+ Add CRC16 mechanism.<br> o Fix file pointer leak in log function.
 1.4.0.0    |Han-Lin           |2013-06-26     |o Modify queue process algorithm to improve performance.
 1.4.1.0    |Kevin             |2013-08-28     |+ Add RDT_Abort() API to close a channel with non blocking mode.<br> + Add error code RDT_ER_REMOTE_ABORT for RDT_Write(), RDT_Read(), RDT_Destroy() and RDT_Abort().<br> + Add error code #RDT_ER_LOCAL_ABORT for RDT_Write() and RDT_Read().<br> + Add error code #RDT_ER_CHANNEL_OCCUPIED for RDT_Create().<br> o Fix CRC error in a Big Endian platform.
 1.4.1.5    |Han-Lin, Victor   |2013-10-28     |o Use call back function instead of polling when reading packets.<br> o Fix RDT algorithm in LAN mode to improve performance. o Fix compiler warnings in Win32 platform.
 1.4.2.0    |Han-Lin, Alex     |2013-11-19     |o Improve transmission machanism.<br> o Fix variables conflict error.<br> o Fix variables conflict error.<br> - Remove CRC machanism.
 1.4.3.0    |Han-Lin           |2014-02-17     |o Improve transmission machanism.
 1.5.0.0    |TUTK RD team      |2014-06-06     |o Improve multi-connection performance.
 1.6.0.0    |TUTK RD team      |2014-08-28     |o Fix re-connection error when do it frequently.<br> o Improve RDT_Abort() function call will exit immediately.<br> o Improve RDT connection performance.<br> + Add RDT_Create_Exit() for exit RDT_Create() process before reach timeout.<br> - Remove include platform_config.h
 1.7.0.0    |TUTK RD team      |2014-09-22     |o Fix RDT_Abort() not release channel resource.<br> + Add 64-bit library.
 1.7.2.0    |TUTK RD team      |2014-10-17     |o Fix process rarely dead lock.<br> o Improve call RDT_Abort() can let RDT_Read() to exit before reach timeout.
 1.8.1.1    |TUTK RD team      |2015-05-20     |o Add SetMaxRecvBuff and Reduce CPU loading .
 */

#ifndef _RDTAPIs_H_
#define _RDTAPIs_H_

/* ============================================================================
 * Platform Dependant Macro Definition
 * ============================================================================
 */



#ifdef _WIN32
/** @cond */
#ifdef IOTC_STATIC_LIB
#define RDTAPI_API
#elif defined RDTAPI_EXPORTS
#define RDTAPI_API __declspec(dllexport)
#else
#define RDTAPI_API __declspec(dllimport)
#endif // #ifdef P2PAPI_EXPORTS
/** @endcond */
#endif // #ifdef _WIN32

#ifdef IOTC_ARC_HOPE312
	#define RDTAPI_API
#endif // #ifdef IOTC_ARC_HOPE312

#if defined(__linux__) || defined (__APPLE__) || defined(__pnacl__)
#define RDTAPI_API
#endif // #ifdef __linux__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ============================================================================
 * Generic Macro Definition
 * ============================================================================
 */

#define MAX_DEFAULT_RDT_CHANNEL_NUMBER				128

/* ============================================================================
 * Error Code Declaration
 * ============================================================================
 */
/** The function is performed successfully. */
#define		RDT_ER_NoERROR							 0

/** RDT module is not initialized yet. Please use RDT_Initialize() for initialization. */
#define		RDT_ER_NOT_INITIALIZED					-10000

/** RDT module is already initialized. It is not necessary to re-initialize. */
#define		RDT_ER_ALREADY_INITIALIZED				-10001

/** The number of RDT channels has reached maximum.
 * Please use RDT_Set_Max_Channel_Number() to set up the max number of RDT channels.
 * By default, the maximum channel number is #MAX_DEFAULT_RDT_CHANNEL_NUMBER. */
#define		RDT_ER_EXCEED_MAX_CHANNEL				-10002

/** Insufficient memory for allocation */
#define		RDT_ER_MEM_INSUFF						-10003

/** RDT module fails to create threads. Please check if OS has ability to
 * create threads for RDT module. */
#define		RDT_ER_FAIL_CREATE_THREAD				-10004

/** RDT module fails to create Mutexs when doing initialization. Please check
 * if OS has sufficient Mutexs for RDT module. */
#define		RDT_ER_FAIL_CREATE_MUTEX				-10005

/** RDT channel has been destroyed. Probably caused by local or remote site
 * calls RDT_Destroy(), or remote site has closed IOTC session. */
#define		RDT_ER_RDT_DESTROYED					-10006

/** The specified timeout has expired during the execution of some RDT module service.
 * For most cases, it is caused by slow response of remote site or network connection issues */
#define		RDT_ER_TIMEOUT							-10007

/** The specified RDT channel ID is valid */
#define		RDT_ER_INVALID_RDT_ID					-10008

/** The remote site has finished sending data, then destroy the RDT channel.
 * The local site will get this error code by RDT_Read() when there is no more
 * data from this RDT channel. */
#define		RDT_ER_RCV_DATA_END						-10009

/** The remote site want to abort the RDT channel immediately and don't care data transmission.
 * The local site will get this error code by RDT_Read(), RDT_Write(), RDT_Destroy() for handling
 * this RDT channel to close. */
#define		RDT_ER_REMOTE_ABORT						-10010

/** The local site called RDT_Abort() so the RDT channel is already not available. */
#define		RDT_ER_LOCAL_ABORT						-10011

/** The specific IOTC session and channel ID is used now so can't use the same resource.
 *  You can choose other IOTC channel for RDT use or wait RDT_Abort() to release resource automatically. */
#define		RDT_ER_CHANNEL_OCCUPIED					-10012

/** This is a lite UID and it does not support RDT module. */
#define		RDT_ER_NO_PERMISSION					-10013

/** The arguments passed to a function is invalid. */
#define		RDT_ER_INVALID_ARG						-10014

/** The local site called RDT_Create_Exit() so the RDT channel exit creating. */
#define		RDT_ER_LOCAL_EXIT						-10015

/** The remote site called RDT_Create_Exit() so the RDT channel exit creating. */
#define		RDT_ER_REMOTE_EXIT						-10016

/** The RDT write buffer is full. Try again after min 1ms  this Error code only return when RDT_Set_Max_SendBuff_Size is be set. */
#define		RDT_ER_SEND_BUFFER_FULL					-10017

/** All RDT connection should call RDT_Abort or RDT_Destroy before doing RDT_Deinitialize  */
#define		RDT_ER_UNCLOSED_CONNECTION_DETECTED		-10018

/* ============================================================================
 * Structure Definition
 * ============================================================================
 */

/**
 * \details RDT channel status, containing the current status of a RDT channel created
 *  between a RDT server and a RDT client. Users can use RDT_Status_Check()
 *  to get RDT status.
 */
struct st_RDT_Status
{
	unsigned short Timeout;  //!< The remaining time, in unit of second, to keep the RDT channel alive
	unsigned short TimeoutThreshold; //!< RDT channel will be terminated if timeout exceeds this threshold, in unit of seconds
	unsigned int BufSizeInSendQueue; //!< The size, in unit of byte, of buffer remaining in the sending queue in the local site
	unsigned int BufSizeInRecvQueue; //!< The size, in unit of byte, of buffer remaining in the receiving queue in the local site
};

/* ============================================================================
 * Function Declaration
 * ============================================================================
 */
/**
 * \brief Get the version of RDT module
 *
 * \details This function returns the version of RDT module
 *
 * \return The version of RDT module from high byte to low byte, for example, 0x01020304
 *			means the version is 1.2.3.4
 *
 * \see IOTC_Get_Version(), avGetAVApiVer()
 */
RDTAPI_API	int RDT_GetRDTApiVer();


/**
 * \brief Set the max number of RDT channels
 *
 * \details This function set the max number of allowable RDT channels.
 *			The max number of RDT channels limits the max number of
 *			RDT channels that a RDT server can establish with multiple RDT clients
 *			from RDT server's point of view, while it limits the max number
 *			of RDT channels that a RDT client can establish with multiple RDT servers
 *			from RDT client's point of view. A RDT server or a RDT
 *			client could use this function to reduce the number of RDT channels
 *			in order to save some memory usage.
 *
 * \param nMaxChannelNum [in] The max number of RDT channels
 *
 * \attention	(1) This function is optional if users do not want to change the
 *				default max number of RDT channels. However, if users really
 *				wants to change it, this function shall be called before
 *				RDT_Initialize(). <br><br>
 *				(2) The default maximum RDT channel number is #MAX_DEFAULT_RDT_CHANNEL_NUMBER
 *				in all platforms.
 */
RDTAPI_API	void RDT_Set_Max_Channel_Number(unsigned int nMaxChannelNum);


/**
 * \brief Initialize RDT module
 *
 * \details This function is used by RDT servers or RDT clients to initialize RDT
 *			module and shall be called before any RDT module related function
 *			is invoked, except RDT_Set_Max_Channel_Number().
 *
 * \return The actual maximum number of allowable RDT channels if initializing successfully
 * \return Error code if return value < 0
 *			- #RDT_ER_ALREADY_INITIALIZED RDT module is already initialized
 *			- #RDT_ER_FAIL_CREATE_MUTEX Fails to create Mutexs
 *
 * \see IOTC_Initialize(), IOTC_Initialize2(), RDT_DeInitialize()
 *
 * \attention IOTC module is needed to be initialized before initializing RDT module.
 *				That is, please invoke IOTC_Initialize() or IOTC_Initialize2()
 *				before this function.
 */
RDTAPI_API	int RDT_Initialize();


/**
 * \brief Deinitialize RDT module
 *
 * \details This function will deinitialize RDT module.
 *
 * \return #RDT_ER_NoERROR if deinitialize successfully
 * \return Error code if return value < 0
 *			- #RDT_ER_NOT_INITIALIZED if RDT module is not initialized yet.
 *			- #RDT_ER_UNCLOSED_CONNECTION_DETECTED if RDT connections are not closed
 *
 * \see RDT_Initialize()
 *
 * \attention (1) RDT module shall be deinitialized before IOTC module is
 *				deinitialized.
 *			  (2) All RDT connection should call RDT_Abort or RDT_Destroy 
 *				before doing RDT_Deinitialize
 *
 */
RDTAPI_API	int RDT_DeInitialize();


/**
 * \brief Create a RDT channel
 *
 * \details This function will create a RDT channel based on specified IOTC channel
 *
 * \param nIOTCSessionID [in] The session ID of the IOTC session to create RDT channel
 * \param nTimeout [in] The timeout for this function in unit of million-second.
 *			Specify it as 0 will make this function block forever until a RDT
 *			channel is successfully created or error happens
 * \param nIOTCChannelID [in] The channel ID of the IOTC channel to create RDT channel
 *
 * \return RDT channel ID if return value >= 0
 * \return Error code if return value < 0
 *			- #RDT_ER_NOT_INITIALIZED RDT module is not initialized yet
 *			- #RDT_ER_EXCEED_MAX_CHANNEL The number of RDT channels has reached maximum
 *			- #RDT_ER_MEM_INSUFF Insufficient memory for allocation
 *			- #RDT_ER_FAIL_CREATE_MUTEX Fails to create Mutex
 *			- #RDT_ER_FAIL_CREATE_THREAD Fails to create threads
 *			- #RDT_ER_RDT_DESTROYED RDT has been destroyed, probably caused
 *				by disconnection from remote site
 *			- #IOTC_ER_NOT_INITIALIZED The IOTC module is not initialized yet
 *			- #IOTC_ER_INVALID_SID The specified IOTC session ID is not valid
 *			- #IOTC_ER_SESSION_CLOSE_BY_REMOTE The IOTC session of specified
 *				session ID has been closed by remote site
 *			- #IOTC_ER_REMOTE_TIMEOUT_DISCONNECT The timeout defined by #IOTC_SESSION_ALIVE_TIMEOUT
 *				expires because	remote site has no response
 *			- #RDT_ER_TIMEOUT The timeout specified by nTimeout expires before
 *				RDT creation is performed completely
 *			- #RDT_ER_CHANNEL_OCCUPIED The specific IOTC session and channel ID not available,
 *				it means the resource not release yet. If caused by RDT_Abort() called must wait
 *				a minute for the resource release automatically.
 *			- #RDT_ER_REMOTE_EXIT Remote site exit this RDT channel connection
 *
 * \see RDT_Destroy() and RDT_Abort()
 *
 * \attention (1) The IOTC channel of specified channel ID will be turned on automatically
 *				by RDT_Create()
 *				(2) This function will need 288 bytes stack size.
 *
 */
RDTAPI_API	int RDT_Create(int nIOTCSessionID, int nTimeout, unsigned char nIOTCChannelID);


/**
 * \brief Exit the process of a specific RDT channel create
 *
 * \details This function will create a RDT channel based on specified IOTC channel
 *
 * \param nIOTCSessionID [in] The session ID argument passed to call RDT_Create()
 * \param nIOTCChannelID [in] The channel ID argument passed to call RDT_Create()
 *
 * \return RDT_ER_NoERROR if function called successfully
 * \return Error code if return value < 0
 *			- #RDT_ER_INVALID_ARG RDT nIOTCSessionID or nIOTCChannelID is valid
 *			- #RDT_ER_INVALID_RDT_ID can't find related RDT channel ID via nIOTCSessionID and nIOTCChannelID
 *
 * \see RDT_Create()
 *
 * \attention It will send exit message to remote but old version can't recognize it,
 *				so must wait until timeout for RDT_Create() return.
 *
 */
RDTAPI_API	int RDT_Create_Exit(int nIOTCSessionID, unsigned char nIOTCChannelID);

/**
 * \brief Destroy a RDT channel
 *
 * \details This function will destroy the RDT channel specified by a channel ID
 *
 * \param nRDTChannelID [in] The channel ID of the RDT channel to destroy
 *
 * \return RDT_ER_NoERROR if destroying successfully
 * \return Error code if return value < 0
 *			- #RDT_ER_NOT_INITIALIZED RDT is not initialized yet
 *			- #RDT_ER_INVALID_RDT_ID The specified RDT channel ID is not valid
 *			- #RDT_ER_REMOTE_ABORT Remote site abort this RDT channel connection so
 *				the data may not send/receive to/from remote site completely.
 *
 * \see RDT_Create()
 *
 * \attention The IOTC channel containing RDT channel will be turned off after the RDT
 *				channel is destroyed. It will block until remote site call RDT_Destroy()
 				or IOTC session disconnected.
 */
RDTAPI_API	int RDT_Destroy(int nRDTChannelID);


/**
 * \brief Abort a RDT channel
 *
 * \details This function will abort the RDT channel connection specified by a channel ID
 *
 * \param nRDTChannelID [in] The channel ID of the RDT channel to abort
 *
 * \return RDT_ER_NoERROR if abort successfully
 * \return Error code if return value < 0
 *			- #RDT_ER_NOT_INITIALIZED RDT is not initialized yet
 *			- #RDT_ER_INVALID_RDT_ID The specified RDT channel ID is not valid
 *			- #RDT_ER_REMOTE_ABORT Remote site already aborted this RDT channel connection
 *				so RDT_Abort() is useless now.
 *
 * \see RDT_Create()
 *
 * \attention It will ignore all data received or not send yet,
 				force to abort the RDT channel connection.
 */
RDTAPI_API	int RDT_Abort(int nRDTChannelID);


/**
* \brief Set Max Send Buffer Size , when buffer is full RDT_Write will return aaaaaaaaaaaaaa of log file
*
* \details Set the absolute path of log file
*
* \param path [in] The path of log file, NULL = disable Log
*
* \param nMaxSize [in] The maximum size of log file in Bytes, 0 = unlimit
*
*/
RDTAPI_API	int RDT_Set_Max_SendBuffer_Size(int nRDTChannelID, int nMaxSendBufferSize);


/**
 * \brief Write data through a RDT channel
 *
 * \details Called by a RDT server or a RDT client to write data through
 *			a specified RDT channel to the other.
 *
 * \param nRDTChannelID [in] The channel ID of the RDT channel to write data
 * \param cabBuf [in] The array of byte buffer containing the data to write
 * \param nBufSize [in] The length of the byte buffer
 *
 * \return The actual length of buffer to be written if write successfully
 * \return Error code if return value < 0
 *			- #RDT_ER_NOT_INITIALIZED RDT module is not initialized yet
 *			- #RDT_ER_INVALID_RDT_ID The specified RDT channel ID is not valid
 *			- #RDT_ER_RDT_DESTROYED RDT module has been destroyed, probably caused
 *				by disconnection from remote site
 *			- #IOTC_ER_NOT_INITIALIZED The IOTC module is not initialized yet
 *			- #IOTC_ER_INVALID_SID The specified IOTC session ID is not valid
 *			- #IOTC_ER_SESSION_CLOSE_BY_REMOTE The IOTC session of specified
 *				session ID has been closed by remote site
 *			- #IOTC_ER_REMOTE_TIMEOUT_DISCONNECT The timeout defined by #IOTC_SESSION_ALIVE_TIMEOUT
 *				expires because	remote site has no response
 *			- #RDT_ER_REMOTE_ABORT Remote site abort this RDT channel connection
 *			- #RDT_ER_LOCAL_ABORT Local site abort this RDT channel connection
 *
 * \see RDT_Read()
 *
 * \attention This function will need 5088 bytes stack size.
 *
 */
RDTAPI_API	int RDT_Write(int nRDTChannelID, const char *cabBuf, int nBufSize);


/**
 * \brief Read data through a RDT channel
 *
 * \details Called by a RDT server or a RDT client to read data through
 *			a specified RDT channel from the other.
 *
 * \param nRDTChannelID [in] The channel ID of the RDT channel to read data
 * \param abBuf [out] The array of byte buffer to receive read result
 * \param nBufSize [in] The maximum length of the byte buffer
 * \param nTimeout [in] The timeout for this function in unit of million-second
 *
 * \return The actual length of read result stored in abBuf if read successfully
 * \return Error code if return value < 0
 *			- #RDT_ER_NOT_INITIALIZED RDT module is not initialized yet
 *			- #RDT_ER_INVALID_RDT_ID The specified RDT channel ID is not valid
 *			- #RDT_ER_RDT_DESTROYED RDT module has been destroyed, probably caused
 *				by disconnection from remote site
 *			- #RDT_ER_TIMEOUT The timeout specified by nTimeout expires before
 *				read process is performed completely
 *			- #RDT_ER_RCV_DATA_END No more data can be read from the RDT channel
 *				and this RDT channel has been destroyed by the remote site
 *			- #RDT_ER_REMOTE_ABORT Remote site abort this RDT channel connection
 *			- #RDT_ER_LOCAL_ABORT Local site abort this RDT channel connection
 *			- #RDT_ER_REMOTE_EXIT Remote site exit this RDT channel connection
 *			- #RDT_ER_INVALID_ARG nBufSize or nTimeout argument is wrong
 *
 * \see RDT_Write()
 *
 * \attention This function will need 292 bytes stack size.
 *
 */
RDTAPI_API	int RDT_Read(int nRDTChannelID, char *abBuf, int nBufSize, int nTimeout);


/**
 * \brief Get status of a RDT channel
 *
 * \details A RDT server or a RDT client may use this function to get the
 *			status of a specified RDT channel
 *
 * \param nRDTChannelID [in] The channel ID of the RDT channel to get status
 * \param psRDT_Status [out] The status of specified RDT channel
 *
 * \return #RDT_ER_NoERROR if getting the RDT status successfully
 * \return Error code if return value < 0
 *			- #RDT_ER_NOT_INITIALIZED RDT module is not initialized yet
 *			- #RDT_ER_INVALID_RDT_ID The specified RDT channel ID is not valid
 */
RDTAPI_API	int RDT_Status_Check(int nRDTChannelID, struct st_RDT_Status *psRDT_Status);

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
RDTAPI_API	void RDT_Set_Log_Path(const char *path, int nMaxSize);






#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* _RDTAPIs_H_ */
