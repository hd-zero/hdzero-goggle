/*! \file IOTCAPIs.h
This file describes all the APIs of the IOTC module in IOTC platform.
IOTC module is a kind of data communication modules to provide basic data
transfer among devices and clients.

\copyright Copyright (c) 2010 by Throughtek Co., Ltd. All Rights Reserved.

Revision Table

Version     | Name             |Date           |Description
------------|------------------|---------------|-------------------
 0.1        |Charlie           |2010-11-15     |Trial Version
 0.2        |Charlie           |2010-12-28     |IOTC_Get_Version(), Session Alive
 0.2.1      |Charlie           |2011-02-28     |Header structure --> Flag, IOTC_Connect return IOTC_ER_DEVICE_NOT_LISTENING, if Target device is not listening
 0.3        |Charlie           |2011-03-02     |Support LAN Mode.
 0.4        |Kevin             |2011-04-22     |Support ASIX110/220 8051 platform
 0.4.0.1    |Charlie           |2011-04-28     |Fix bug of LanSearch
 0.4.0.2    |Kevin             |2011-04-28     |Fix bug of gBuf multi-thread access currently.
 0.4.0.3    |Charlie           |2011-05-06     |Fix ReadBuf bug because multi-thread read/write competition
 0.4.0.4    |Charlie           |2011-05-10     |Hope 312 can't support fopen, so undefined ShowVersionInfoinLogFile() for Hope' _ARC_COMPILER
 0.5.0.0    |Charlie/Kevin     |2011-06-30     |A major revision change of IOTCAPIs, lead-in multi-channel mechanism and including using IOTC protocol 0.2
 1.0.0.0    |Kevin             |2011-07-12     |Formal version, stability testing
 1.0.1.0    |Kevin             |2011-07-25     |Fix Bug : __Search_DeviceInfo() and __Search_VPGServerList(), add threshold
 1.0.2.0    |Kevin             |2011-07-27     |Fix Bug : _Check_P2P_Possible(), improve _Check_Nat_Type(), Modify IOTC_TIME_OUT_SECOND_P2P_REQUEST 60->10
 1.1.0.0    |Kevin             |2011-07-29     |Fix Bug : Before login reset nat check status, client query device got unlicensed respond not return error code IOTC_ER_UNLICENSE. Modify : judge precheck respond last login time 300->120, Got MSG_LAN_SEARCH_R start with _IOTC_Send_Knock() procedure. Add API : IOTC_Search_Device_XXX() and IOTC_Search_Stop()
 1.1.1.0    |Kevin             |2011-08-01     |Fix Bug : _IOTC_Check_Master() and _IOTC_Check_Server(), check one server once -> check all server once. Modify : IOTC_TIME_OUT_SECOND_P2P_REQUEST 10->6
 1.1.2.0    |Kevin             |2011-08-08     |Fix Bug : 1.Linux thread not reclaim so must detach thread 2.when only do search device done not release session index. Modify : not wait resolve master result until call IOTC_Device_Login() or IOTC_Connect_XXX(), it will improve LAN mode performance
 1.2.0.0    |Kevin             |2011-08-29     |Improve : Alloc read buffer dynamically instead of declare static memory Fix Bug : 1.IOTC_Session_Channel_OFF() not clean read buffer 2.SessionInfo member HeaderPacketCount not separate with channel
 1.2.1.0    |Kevin             |2011-09-08     |Add : 1.IOTC_Initialize() setup p2p socket buffer size 2.judge UID if valid function. Modify : remove query time threshold of __Search_DeviceInfo() and __Search_VPGServerList(). Fix Bug : in IOTC_Device_Login() give wrong argument when call _IOTC_SendQuryServerList() to query third and fourth master. Custom : reduce static memory usage for ARC
 1.3.0.0    |Kevin             |2011-11-15     |Fix Bug : 1.ASIX not convert NAT and version of login info from little endian to big endian. 2.iPhone and Win32 Lan search 3.use wrong VPG info to login. 4.Lan search send ARP to query will cause AP crash, send x.x.x.255 instead. Add : 1.use UID to bind lan search port. 2.UID toupper. 3.Network flow mechanism to know response and packet lost rate. Custom : replace malloc and tutk_platform_free with p2p_malloc and p2p_free for ARC. Improve : 1.IOTC_DeInitialize() to use pthread_join like function to wait thread exit. 2. IOTC_Device_Login() return IOTC_ER_NoERROR when master address is "127.0.0.1" for only use LAN mode. Private : 1.search all device on LAN 2.force relay mode to connect.
 1.3.1.0    |Kevin             |2011-11-24     |Fix Bug : _IOTC_SendAlive() via relay always give RLYSessionID 0 Improve : 1.When call IOTC_DeInitialize() to close all session. 2.When receive remote session close or keep alive timeout not clean read buf until user call IOTC_Session_Close().
 1.3.2.0    |Kevin             |2011-12-21     |Fix Bug : 1. IOTC_Connect_ByUID() duplicate call at the same time will return wrong SID. 2.IOTC_Connect_ByName() return IOTC_ER_UNLICENSE error and Lan search error. 3.IOTC_Connect_ByUID() use wrong VPG to send query to third and fourth Master. Add : 1.IOTC_Set_Max_Session_Number() to setup session number for control memory usage. 2._IOTC_thread_Device_Login also watch a alive timeout counter for avoiding _IOTC_thread_Session_Alive crash. Modify : int IOTC_Get_Login_Info(), if return > 0 means login failure count.
 1.4.0.0    |Kevin             |2012-02-02     |Fix Bug : Mutex lock use error on WIN32 platform. Add : 1.IOTC_sListen() and IOTC_sConnect() create AES encryption session. 2.Big-endian SOC support: device and client and AES encryption.
 1.4.1.0    |Kevin             |2012-02-22     |Fix Bug : IOTC_Connect_ByName() not work.
 1.4.2.0    |Kevin             |2012-03-05     |Fix Bug : Relay mode session close Mutex dead lock.
 1.5.0.0    |Kevin             |2012-03-21     |Fix Bug : 1.When use wifi and ethernet not disable will cause crash on WIN32 platform. 2.Secure session cant use when RLY mode. 3.Secure session will cause packet content wrong when encode and decode currently. Add : 1.Lan search all to get device name and password information. 2.IOTC_Session_Get_Free_Channel() to get a available channel. 3.IOTC_IsDeviceSecureMode() for client to know if device is secure mode. 4.LAN mode in session info. Modify : login period 60->40 for avoiding router flush routing table. Improve : 1.When IOTC_Initialize() give IP can skip resolve domain name. 2.Use getsockname() to get local IP instead of resolving domain name.
 1.6.0.0    |Kevin             |2012-06-13     |Fix Bug : 1.Lan search setup connection. 2.IOTC_Device_Login() failed when Internet available. Add : IOTC_Listen_Exit() to force exit IOTC_Listen().
 1.7.0.0    |Kevin             |2012-08-16     |+ Add APIs to support both non-secure and secure mode: IOTC_Listen2(), IOTC_Connect_ByUID2() and IOTC_Connect_ByName2() <br> + Add a mechanism to get login status by callback: IOTC_Get_Login_Info_ByCallBackFn() <br> + Add error code IOTC_ER_NOT_INITIALIZED returned by IOTC_DeInitialize()<br> o Improve device login and client connection time more quickly <br> o Return IOTC_ER_NETWORK_UNREACHABLE in IOTC_Device_Login() when network is unavailable<br> o Change the error code IOTC_ER_UNKNOWN_DEVICE returned by IOTC_Connect_ByUID() to IOTC_ER_SERVER_NOT_RESPONSE for clarification purpose <br> o Change the error code IOTC_ER_TIMEOUT returned by IOTC_Connect_ByXXX() to IOTC_ER_FAIL_SETUP_RELAY for clarification purpose<br> o Fix memory leak caused by IOTC_Device_Login() fails<br> - Remove error codes: IOTC_ER_SENDTO_FAILED, IOTC_ER_DEVICE_NOT_LOGIN, IOTC_ER_DEVICE_NOT_SECURE_LISTEN
 1.8.5.0    |Kevin             |2012-11-15     |+ Support TCP80 <br> + Support iOS6 <br> + Add error codes for blacklist and whitelist mechanism <br> + Add a mechanism to check session status by callback: IOTC_Session_Check_ByCallBackFn <br> + Add error code IOTC_ER_FAIL_CREATE_SOCKET, IOTC_ER_FAIL_SOCKET_OPT, IOTC_ER_FAIL_SOCKET_BIND, IOTC_ER_NOT_SUPPORT_RELAY returned by IOTC_Connect_ByUID(), IOTC_Connect_ByUID2(), IOTC_Connect_ByName() and IOTC_Connect_ByName2() <br> + Support Mips_Realtek8196C_3.4.6 <br> + Support Mips_Realtek8196D_4.4.5 <br> + Support Mips_RalinkRTF5350_3.4.2 <br> + Support ARM9_GM8126_4.4.7 <br> + Support ARM9_N32905U1DN_4.2.1 <br> - Delete ARM9_GM8126_3.4.6 <br> - Delete MIPS_RalinkRT5350_4.3.3 <br> - Remove error code IOTC_ER_FAIL_CREATE_SOCKET, IOTC_ER_FAIL_SOCKET_OPT, IOTC_ER_FAIL_SOCKET_BIND from IOTC_Initialize() and IOTC_Initialize2() <br> o Fix IOTC_Connect_ByUID() issue in multi-thread usage <br> o Fix connection issue for second time IOTC_Initialize() after IOTC_DeInitialize() <br> o Change the algorithm of random port generation <br> o Improve LAN search successful rate <br> o Improve the speed of detection whether network is reachable <br> o Modify relay not to be established again with already connected UID <br> o Add const modifier for input parameters in function prototype.
 1.9.1.0    |Kevin, Harry      |2013-03-04     |+ Add IOTC_Lan_Search().<br> + Add error code #IOTC_ER_NO_SERVER_LIST for IOTC_Connect_ByUID() and IOTC_Connect_ByUID2().<br> + Add error code #IOTC_ER_DEVICE_MULTI_LOGIN for IOTC_Connect_ByUID() and IOTC_Connect_ByUID2().<br> + Add error code #IOTC_ER_INVALID_ARG for IOTC_Lan_Search().<br> + Support Arm9_Hi3531_4.4.1 platform.<br> + Support Mips_RalinkRT3352_3.4.2 platform.<br> o Fix bug for getting local ip address.<br> o Fix continue notify when login status changed.<br> o Change IOTC_ER_NO_TCP_SUPPORT to #IOTC_ER_NO_PERMISSION for advance UID identification.<br> - Remove IOTC_Connect_ByName and IOTC_Connect_ByName2 APIs.
 1.9.2.0    |Han-Lin, Harry    |2013-03-28     |+ Add IOTC_Set_Log_Path() to set log file name, path and maximum size.<br> + Log error codes in specified file.<br> o Fix incorrect returning IOTC_ER_NO_SERVER_LIST when Master is shutdown for a long time.
 1.9.3.0    |Kevin, Harry      |2013-04-03     |o Fix TCP connection to IOTC server confirmation error.<br> o Fix return precise TCP connection error code.
 1.9.4.0    |Kevin, Han-Lin    |2013-04-09     |o Fix IOTC_Device_Login() blocking bug.<br> o Fix TCP receive data error.<br> o Fix packers handling bug.
 1.9.5.0    |Kevin, Han-Lin    |2013-04-18     |o Fix IOTC_Device_Login() blocking bug.<br> o Fix relay connection unstable when use UDP and TCP in different side.<br> o Fix TCP receive data error.<br> o Fix IOTC_Session_Close() keeps VPG information on device side.
 1.10.0.0   |Kevin             |2013-04-30     |+ Add IOTC_Connect_ByUID_Parallel() and IOTC_Get_SessionID() to support connecting to device concurrently.<br> + Add IOTC_Connect_Stop_BySID() to stop a specific session connecting to device.
 1.10.1.0   |Hanlin, Harry     |2013-05-07     |o Fix log function cannot be disabled.<br> o Fix the handle of TCP packets with header magic error.
 1.10.2.0   |Kevin, Han-Lin    |2013-05-16     |+ Add critical section for uCOSII.<br> o Fix log function blocks.<br> o Fix for storing session number by better variable type .
 1.10.3.0   |Kevin, Han-Lin    |2013-06-25     |o Fix algorithm for NAT type checking function may not called on type 1.<br> o Fix error handling algorithm when receive EAGAIN in TCP connection.<br> o Fix algorithm to avoid returning IOTC_ER_DEVICE_NOT_LISTENING.<br> o Fix bug when data size near max buffer size.<br> o Fix algorithm to process P2P packet correctly when device is not listening .
 1.10.4.0   |TUTK RD team      |2013-07-26     |o Fix IOTC_Connect may blocked.<br> o Fix relay disconnected issue.<br> o Fix inconsistency connection mode between device and client, and cause -42 error code.<br> o Fix memory leak issue.<br> o Fix incorrect network detection when WAN port of router is disconnected.<br> o Fix multiple calling of connect_byUID() return -19 error.<br> o Fix IOTC_Session_Write performance and returns -16 issue.<br> o Fix IOTC_Connect_ByUID may blocked.
 1.10.5.0   |TUTK RD team      |2013-07-26     |o Fix incorrect connection packages when connection and disconnection in a short period.<br> o Fix keep alive packages transmission mechanism.
 1.10.6.0   |TUTK RD team      |2013-08-28     |o Fix connecting to multi-device may return -19 error code.<br> * Fix UDP socket receive buffer size to avoid loss packets.<br> * Fix IOTC_Device_Login() will occupy a session ID temporary.<br> + Add IOTC_Set_Partial_Encryption() to control encryption length.<br> o Fix NAT type check error to prevent from become RELAY mode after first connection.<br> o Fix TCP socket management and error code handling to increase TCP relay stability.<br> o Fix variable initialize to prevent from Win32 crashes.<br> o Fix network detection return -41 issue.<br> * Fix crashes when checking session status.<br> o Fix IOTC_Session_Read() data handling in timeout status.
 1.10.6.4   |TUTK RD team      |2013-10-28     |o Fix error detection in Win32.<br> o Fix critical session in uCOSII.<br> o Fix Winsock issues in win32 static link.<br> o Fix compiler warnings in some arm9 platform.<br> + Support Lite UID.<br> o Fix LAN connection timeout extends to 1.5s.<br> o Fix bugs in receiving data.
 1.10.7.0   |TUTK RD team      |2013-11-19     |o Fix socket bind error.<br> o Fix variables conflict error.
 1.10.8.0   |TUTK RD team      |2014-02-17     |o Fix big endian bug.<br> o Add memory information in uCOSII.<br> o Fix udp port error.<br> o Improve connecting machanism in uCOSII.<br> o Fix race condition bug in MAC OS.<br> o Improve NAT Type detection.<br> + Support different functional UID.<br> + Add new api IOTC_Set_Device_Name() to set Device Name.<br> + Add new api IOTC_Lan_Search2() to know Device Name in Lan.
 1.11.0.0   |TUTK RD team      |2014-08-28     |o Fix crash bug on Win32.<br> o Fix misjudge NAT type3.<br> o Fix re-connection error when do it frequently.<br> o Fix choose a not workable Server on TCP mode.<br> o Improve connection performance a little bit.<br> o Fix login failed when re-query Master failed.<br> + Add support class A/B/C LAN search.<br> + Add IOTC_Setup_LANConnection_Timeout() for setup time to try LAN connection.<br> + Add IOTC_TCPRelayOnly_TurnOn() for setup connection only via TCP relay mode.<br> - Remove include platform_config.h
 1.12.0.0   |TUTK RD team      |2014-09-22     |+ Add 64-bit library.
 1.13.0.0   |TUTK RD team      |2014-10-15     |o Fix session ID increase leak in some situationo.<br> Improve device can handle concurrent connection from client.<br> o Improve P2P connection stability.
 */

#ifndef _IOTCAPIs_H_
#define _IOTCAPIs_H_

/* ============================================================================
 * Platform Dependant Macro Definition
 * ============================================================================
 */

#ifdef _WIN32
	#ifdef IOTC_STATIC_LIB
		#define P2PAPI_API
	#elif defined P2PAPI_EXPORTS
		#define P2PAPI_API __declspec(dllexport)
	#else
		#define P2PAPI_API __declspec(dllimport)
	#endif
#else // #ifdef _WIN32
	#define P2PAPI_API
#endif //#ifdef _WIN32


#ifndef _WIN32
	#define __stdcall
#endif // #ifndef _WIN32


#ifdef _WIN32
/** @cond */


/** @endcond */
/** The default max number of IOTC sessions in IOTC module.
 * It is platform dependent and refer to source code for more detail. */
#define MAX_DEFAULT_IOTC_SESSION_NUMBER				128
#endif // #ifdef _WIN32

#ifdef IOTC_ARC_HOPE312
#define MAX_DEFAULT_IOTC_SESSION_NUMBER				16
#endif // #ifdef IOTC_ARC_HOPE312

#if defined (__linux__) || defined (__APPLE__) || defined(__pnacl__)
#define MAX_DEFAULT_IOTC_SESSION_NUMBER				128
#endif // #ifdef __linux__  __APPLE__  __pnacl__

#ifdef IOTC_ASIX8051
#define MAX_DEFAULT_IOTC_SESSION_NUMBER				4
#endif // #ifdef IOTC_ASIX8051

#ifdef IOTC_UCOSII
#define MAX_DEFAULT_IOTC_SESSION_NUMBER				8
extern u32  TutkLoginTaskPri;
extern u32  TutkRoutineTaskPri;
#endif // #ifdef IOTC_UCOSII

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ============================================================================
 * Generic Macro Definition
 * ============================================================================
 */
/** The maximum size, in byte, of the buffer used in IOTC_Session_Read(),
 * IOTC_Session_Write() and IOTC_Session_Read_Check_Lost(). */
#define IOTC_MAX_PACKET_SIZE						1388

/** The maximum size, in byte, of the buffer used in IOTC_Session_Write_Reliable(). */
#define RELIABLE_MAX_PACKET_SIZE				    1384

/** The maximum number of IOTC channels for each IOTC session */
#define MAX_CHANNEL_NUMBER							32

/** The timeout, in unit of second, for keeping an IOTC session alive since
 * the last time remote site has response */
#define IOTC_SESSION_ALIVE_TIMEOUT					60

/* ============================================================================
 * Error Code Declaration
 * ============================================================================
 */
/** The function is performed successfully. */
#define	IOTC_ER_NoERROR								0

/** IOTC servers have no response, probably caused by many types of Internet connection issues. */
// See [Troubleshooting](..\Troubleshooting\index.htm#IOTC_ER_SERVER_NOT_RESPONSE)
#define	IOTC_ER_SERVER_NOT_RESPONSE					-1

/** IOTC masters cannot be resolved their domain name, probably caused
 * by network connection or DNS setting issues. */
#define	IOTC_ER_FAIL_RESOLVE_HOSTNAME				-2

/** IOTC module is already initialized. It is not necessary to re-initialize. */
#define IOTC_ER_ALREADY_INITIALIZED                 -3

/** IOTC module fails to create Mutexs when doing initialization. Please
 * check if OS has sufficient Mutexs for IOTC platform. */
#define IOTC_ER_FAIL_CREATE_MUTEX                   -4

/** IOTC module fails to create threads. Please check if OS has ability
 * to create threads for IOTC module. */
#define IOTC_ER_FAIL_CREATE_THREAD                  -5

/** IOTC module fails to create sockets. Please check if OS supports socket service */
#define IOTC_ER_FAIL_CREATE_SOCKET                  -6

/** IOTC module fails to set up socket options. */
#define IOTC_ER_FAIL_SOCKET_OPT                     -7

/** IOTC module fails to bind sockets */
#define IOTC_ER_FAIL_SOCKET_BIND                    -8

/** The specified UID is not licensed or expired. */
#define IOTC_ER_UNLICENSE                           -10

/** The device is already under login process currently
 * so it is prohibited to invoke login again at this moment. */
#define IOTC_ER_LOGIN_ALREADY_CALLED                -11

/** IOTC module is not initialized yet. Please use IOTC_Initialize() or
 * IOTC_Initialize2() for initialization. */
#define IOTC_ER_NOT_INITIALIZED                     -12

/** The specified timeout has expired during the execution of some IOTC
 * module service. For most cases, it is caused by slow response of remote
 * site or network connection issues */
#define IOTC_ER_TIMEOUT                             -13

/** The specified IOTC session ID is not valid */
#define IOTC_ER_INVALID_SID                         -14

/** The specified device's name is unknown to the IOTC servers */
#define IOTC_ER_UNKNOWN_DEVICE                      -15

/** IOTC module fails to get the local IP address */
#define IOTC_ER_FAIL_GET_LOCAL_IP                   -16

/** The device already start to listen for connections from clients. It is
 * not necessary to listen again. */
#define IOTC_ER_LISTEN_ALREADY_CALLED               -17

/** The number of IOTC sessions has reached maximum.
 * To increase the max number of IOTC sessions, please use IOTC_Set_Max_Session_Number()
 * before initializing IOTC module. */
#define IOTC_ER_EXCEED_MAX_SESSION                  -18

/** IOTC servers cannot locate the specified device, probably caused by
 * disconnection from the device or that device does not login yet. */
#define IOTC_ER_CAN_NOT_FIND_DEVICE                 -19

/** The client is already connecting to a device currently
 * so it is prohibited to invoke connection again at this moment. */
#define IOTC_ER_CONNECT_IS_CALLING                  -20

/** The remote site already closes this IOTC session.
 * Please call IOTC_Session_Close() to release IOTC session resource in locate site. */
#define IOTC_ER_SESSION_CLOSE_BY_REMOTE             -22

/** This IOTC session is disconnected because remote site has no any response
 * after a specified timeout expires, i.e. #IOTC_SESSION_ALIVE_TIMEOUT */
#define IOTC_ER_REMOTE_TIMEOUT_DISCONNECT           -23

/** The client fails to connect to a device because the device is not listening for connections. */
#define IOTC_ER_DEVICE_NOT_LISTENING                -24

/** The IOTC channel of specified channel ID is not turned on before transferring data. */
#define IOTC_ER_CH_NOT_ON                           -26

/** A client stops connecting to a device by calling IOTC_Connect_Stop() */
#define IOTC_ER_FAIL_CONNECT_SEARCH                 -27

/** Too few masters are specified when initializing IOTC module.
 * Two masters are required for initialization at minimum. */
#define IOTC_ER_MASTER_TOO_FEW                      -28

/** A client fails to pass certification of a device due to incorrect key. */
#define IOTC_ER_AES_CERTIFY_FAIL                    -29

/** The number of IOTC channels for a IOTC session has reached maximum, say, #MAX_CHANNEL_NUMBER */
#define IOTC_ER_SESSION_NO_FREE_CHANNEL             -31

/** Cannot connect to masters neither UDP mode nor TCP mode by IP or host name ways */
#define IOTC_ER_TCP_TRAVEL_FAILED					-32

/** Cannot connect to IOTC servers in TCP */
#define IOTC_ER_TCP_CONNECT_TO_SERVER_FAILED        -33

/** A client wants to connect to a device in non-secure mode while that device
 * supports secure mode only. */
#define IOTC_ER_CLIENT_NOT_SECURE_MODE              -34

/** A client wants to connect to a device in secure mode while that device does
 * not support secure mode. */
#define IOTC_ER_CLIENT_SECURE_MODE					-35

/** A device does not support connection in secure mode */
#define IOTC_ER_DEVICE_NOT_SECURE_MODE              -36

/** A device does not support connection in non-secure mode */
#define IOTC_ER_DEVICE_SECURE_MODE					-37

/** The IOTC session mode specified in IOTC_Listen2(), IOTC_Connect_ByUID2()
 * is not valid. Please see #IOTCSessionMode for possible modes. */
#define IOTC_ER_INVALID_MODE                        -38

/** A device stops listening for connections from clients. */
#define IOTC_ER_EXIT_LISTEN                         -39

/** The specified device does not support advance function
 *(TCP relay and P2PTunnel module) */
#define IOTC_ER_NO_PERMISSION                  		-40

/** Network is unreachable, please check the network settings */
#define	IOTC_ER_NETWORK_UNREACHABLE     			-41

/** A client fails to connect to a device via relay mode */
#define IOTC_ER_FAIL_SETUP_RELAY					-42

/** A client fails to use UDP relay mode to connect to a device
 * because UDP relay mode is not supported for that device by IOTC servers */
#define IOTC_ER_NOT_SUPPORT_RELAY					-43

/** No IOTC server information while device login or client connect
 * because no IOTC server is running or not add IOTC server list */
#define IOTC_ER_NO_SERVER_LIST						-44

/** The connecting device duplicated loggin and may unconnectable. */
#define IOTC_ER_DEVICE_MULTI_LOGIN					-45

/** The arguments passed to a function is invalid. */
#define IOTC_ER_INVALID_ARG                         -46

/** The remote device not support partial encoding. */
#define IOTC_ER_NOT_SUPPORT_PE			            -47

/** The remote device no more free session can be connected. */
#define IOTC_ER_DEVICE_EXCEED_MAX_SESSION			-48


/** The function call is a blocking call and was called by other thread. */
#define IOTC_ER_BLOCKED_CALL									-49

/** The session was closed. */
#define IOTC_ER_SESSION_CLOSED								-50

/** Remote doesn't support this function. */
#define IOTC_ER_REMOTE_NOT_SUPPORTED					-51

/** The function is aborted by related function. */
#define IOTC_ER_ABORTED											-52

/** The buffer size exceed maximum packet size. */
#define IOTC_ER_EXCEED_MAX_PACKET_SIZE				-53

/** Server does not support this feature. */
#define IOTC_ER_SERVER_NOT_SUPPORT						-54

/** Cannot find a path to write data*/
#define IOTC_ER_NO_PATH_TO_WRITE_DATA				-55

/** Start function is not called */
#define IOTC_ER_SERVICE_IS_NOT_STARTED  			-56

/** Already in processing*/
#define IOTC_ER_STILL_IN_PROCESSING                    -57

/** Out of memory*/
#define IOTC_ER_NOT_ENOUGH_MEMORY                    -58

/** The device is banned and locked*/
#define IOTC_ER_DEVICE_IS_BANNED						-59

/** IOTC master servers have no response, probably caused by many types of Internet connection issues. */
#define IOTC_ER_MASTER_NOT_RESPONSE					-60

/** All Server response can not find device */
#define IOTC_ER_DEVICE_OFFLINE						-90 // 2015.05.08  Eddy Add


/* ============================================================================
 * Enumeration Declaration
 * ============================================================================
 */

/**
 * \details IOTC session mode, used in IOTC_Listen2(), IOTC_Connect_ByUID2()
 *			to specify what kinds of IOTC session
 *			that devices will listen or clients will connect.
 */
typedef enum
{
	/// IOTC session will be established in non-secure mode. <br>
	/// - For devices, the behavior is the same as IOTC_Listen(). <br>
	/// - For clients, the behavior is the same as IOTC_Connect_ByUID()
	IOTC_NON_SECURE_MODE = 0,

	/// IOTC session will be established in secure mode. <br>
	/// - For devices, it means only secure connection will be accepted. <br>
	/// - For clients, it means only secure connection will be performed
	IOTC_SECURE_MODE = 1,

	/// IOTC session will be established in either non-secure or secure mode,
	/// depending on remote site's request. Can use IOTC_Session_Check()
	/// to check what mode is actually used. <br>
	/// - For devices, it means both non-secure or secure modes are accepted. <br>
	/// - For clients, it means either non-secure or secure modes can be performed.
	IOTC_ARBITRARY_MODE = 2
} IOTCSessionMode;


/* ============================================================================
 * Structure Definition
 * ============================================================================
 */

/**
 * \details IOTC session info, containing all the information when a IOTC session is
 *  established between a device and a client. Users can use IOTC_Session_Check()
 *  to get IOTC session information.
 */
struct st_SInfo
{
	unsigned char Mode; //!< 0: P2P mode, 1: Relay mode, 2: LAN mode
	char CorD; //!< 0: As a Client, 1: As a Device
	char UID[21]; //!< The UID of the device
	char RemoteIP[17]; //!< The IP address of remote site used during this IOTC session
	unsigned short RemotePort; //!< The port number of remote site used during this IOTC session
	unsigned int TX_Packetcount; //!< The total packets sent from the device and the client during this IOTC session
	unsigned int RX_Packetcount; //!< The total packets received in the device and the client during this IOTC session
	unsigned int IOTCVersion; //!< The IOTC version
	unsigned short VID; //!< The Vendor ID, part of VPG mechanism
	unsigned short PID; //!< The Product ID, part of VPG mechanism
	unsigned short GID; //!< The Group ID, part of VPG mechanism
	unsigned char NatType; //!< The remote NAT type
	unsigned char isSecure; //!< 0: The IOTC session is in non-secure mode, 1: The IOTC session is in secure mode
};

/**
 * \details IOTC session info, containing all the information when a IOTC session is
 *  established between a device and a client. Users can use IOTC_Session_Check_Ex()
 *  to get IOTC session information.
 */
struct st_SInfoEx
{
    unsigned int size; //size of the structure
	unsigned char Mode; //!< 0: P2P mode, 1: Relay mode, 2: LAN mode
	char CorD; //!< 0: As a Client, 1: As a Device
	char UID[21]; //!< The UID of the device
	char RemoteIP[17]; //!< The IP address of remote site used during this IOTC session
	unsigned short RemotePort; //!< The port number of remote site used during this IOTC session
	unsigned int TX_Packetcount; //!< The total packets sent from the device and the client during this IOTC session
	unsigned int RX_Packetcount; //!< The total packets received in the device and the client during this IOTC session
	unsigned int IOTCVersion; //!< The IOTC version
	unsigned short VID; //!< The Vendor ID, part of VPG mechanism
	unsigned short PID; //!< The Product ID, part of VPG mechanism
	unsigned short GID; //!< The Group ID, part of VPG mechanism
	unsigned char isSecure; //!< 0: The IOTC session is in non-secure mode, 1: The IOTC session is in secure mode
	unsigned char LocalNatType; //!< The local NAT type, 0: Unknown type, 1: Type 1, 2: Type 2, 3: Type 3, 10: TCP only
	unsigned char RemoteNatType; //!< The remote NAT type, 0: Unknown type, 1: Type 1, 2: Type 2, 3: Type 3, 10: TCP only
	unsigned char RelayType; //!< 0: Not Relay, 1: UDP Relay, 2: TCP Relay
	unsigned int NetState; //!<If no UDP packet is ever received, the first bit of value is 1, otherwise 0
};

/**
 * \details Device search info, containing all the information
 * when client searches devices in LAN.
 */
struct st_LanSearchInfo
{
	char UID[21]; //!< The UID of discovered device
	char IP[16]; //!< The IP address of discovered device
	unsigned short port; //!< The port number of discovered device used for IOTC session connection
	char Reserved; //!< Reserved, no use
};

/**
 * \details Device search info, containing all the information and device name
 * when client searches devices in LAN.
 */
struct st_LanSearchInfo2
{
	char UID[21]; //!< The UID of discovered device
	char IP[16]; //!< The IP address of discovered device
	unsigned short port; //!< The port number of discovered device used for IOTC session connection
	char DeviceName[132]; //!< The Name of discovered device
	char Reserved; //!< Reserved, no use
};

/**
 * \details Device search info, containing all the information and device name
 * when client searches devices in LAN.
 */
struct st_SearchDeviceInfo
{
 char UID[21]; //!< The UID of discovered device
 char IP[16]; //!< The IP address of discovered device
 unsigned short port; //!< The port number of discovered device used for IOTC session connection
 char DeviceName[132]; //!< The Name of discovered device
 char Reserved; //!< Reserved, no use
 int  nNew; //!< New device
};

/**
 * \details Connect Option, containing all options of connection setup when client connects to
 * device by P2P or relay mode.
 */
struct st_ConnectOption
{
    char IsParallel; //!< 0: Turn off parallel connection. 1: Turn on parallel connection.
};

/* ============================================================================
 * Type Definition
 * ============================================================================
 */

/**
 * \details The prototype of getting login info function, used by a device
 *			to be notified if it is still kept login with IOTC servers or is
 *			disconnected with IOTC servers.
 *
 * \param pnLoginInfo [out] The login info with meanings of following bits
 *			- bit 0: the device is ready for connection by client from LAN if this bit is 1
 *			- bit 1: the device is ready for connection by client from Internet if this bit is 1
 *			- bit 2: if this bit is 1, it means the device has received login
 *						response from IOTC servers since IOTC_Get_Login_Info()
 *						is called last time.
 *
 */
typedef void (__stdcall *loginInfoCB)(unsigned int nLoginInfo);

/**
 * \details The prototype of getting session status function, used by a device
 *			or client to be notified if session is disconnected.
 *
 * \param nIOTCSessionID [out] The session ID of the session being disconnected
 * \param nErrorCode [out]
 *			- #IOTC_ER_SESSION_CLOSE_BY_REMOTE The IOTC session of specified
 *				session ID has been closed by remote site
 *			- #IOTC_ER_REMOTE_TIMEOUT_DISCONNECT The timeout defined by #IOTC_SESSION_ALIVE_TIMEOUT
 *				expires because	remote site has no response
 *
 */
typedef void (__stdcall *sessionStatusCB)(int nIOTCSessionID, int nErrorCode);


typedef int (__stdcall *IOTC_SearchDeviceResultCB)(struct st_SearchDeviceInfo *psSearchDeviceInfo, int nArrayLength);

/* ============================================================================
 * Function Declaration
 * ============================================================================
 */

/**
 * \brief Get the version of IOTC module
 *
 * \details This function returns the version of IOTC module.
 *
 * \param pnVersion [out] The version of IOTC module. It contains
 *			the version from high byte to low byte, for example, 0x01020304
 *			means the version is 1.2.3.4
 *
 * \see RDT_GetRDTApiVer(), avGetAVApiVer()
 */
P2PAPI_API void IOTC_Get_Version(unsigned int *pnVersion);


/**
 * \brief Set the max number of IOTC sessions of IOTC module
 *
 * \details This function set the max number of allowable IOTC sessions in IOTC
 *			module. The max number of IOTC session limits the max number of
 *			connected clients in device side, while it limits the max number
 *			connected devices in client	side. A device or a client could use
 *			this function to reduce the number of IOTC sessions in order to save
 *			some memory usage.
 *
 * \param nMaxSessionNum [in] The max number of IOTC sessions
 *
 * \attention	(1) This function is optional if users do not want to change the
 *				default max number of IOTC sessions, i.e. MAX_DEFAULT_IOTC_SESSION_NUMBER.
 *				However, if users really wants to change it, this function
 *				shall be called before IOTC platform is initialized.<br><br>
 *				(2) The maximum IOTC session number is platform dependent.
 *				See the definition of MAX_DEFAULT_IOTC_SESSION_NUMBER for each
 *				platform.
 */
P2PAPI_API void IOTC_Set_Max_Session_Number(unsigned int nMaxSessionNum);


/**
 * \brief Initialize IOTC module
 *
 * \details This function is used by devices or clients to initialize IOTC
 *			module and shall be called before any IOTC module related
 *			function is invoked except for IOTC_Set_Max_Session_Number().
 *
 * \param nUDPPort [in] Specify a UDP port. Random UDP port is used if it is specified as 0.
 * \param cszP2PHostNamePrimary [in] Specify the host name or IP address of
 *			the primary master. Cannot be NULL. See attention below for more detail.
 * \param cszP2PHostNameSecondary [in] Specify the host name or IP address of
 *			the secondary master. Cannot be NULL. See attention below for more detail.
 * \param cszP2PHostNameThird [in] Specify the host name or IP address of
 *			the third master. Can be NULL if only two masters are required.
 *			See attention below for more detail.
 * \param cszP2PHostNameFourth [in] Specify the host name or IP address of
 *			the fourth master. Can be NULL if only two masters are required.
 *			See attention below for more detail.
 *
 * \return #IOTC_ER_NoERROR if initializing successfully
 * \return Error code if return value < 0
 *			- #IOTC_ER_FAIL_RESOLVE_HOSTNAME Cannot resolve masters' host name
 *			- #IOTC_ER_ALREADY_INITIALIZED IOTC module is already initialized
 *			- #IOTC_ER_MASTER_TOO_FEW Two masters are required as parameters at minimum
 *			- #IOTC_ER_FAIL_CREATE_MUTEX Fails to create Mutexs
 *			- #IOTC_ER_FAIL_CREATE_THREAD Fails to create threads
 *
 * \see IOTC_Initialize2(), IOTC_DeInitialize()
 *
 * \attention   (1) This function is the key entry to whole IOTC platform, including
 *				RDT module and AV module. That means, if you want tso use
 *				RDT module, users shall still use this function to initialize IOTC
 *				module before calling RDT_Initialize(). <br><br>
 *				(2) Usually, host name is suggested to be used to specify a master.
 *				because that will ensure devices and clients can still connect
 *				to masters even the network address configuration of masters
 *				changes in the future. However, in rare case, the host name
 *				of masters can not be resolved due to network issue and it is
 *				necessary to specify IP address of masters in this function
 *				in order for successful connection. The host name and IP address
 *				of each master is listed as below:
 *				- Master #1: m1.iotcplatform.com => 50.19.254.134
 *				- Master #2: m2.iotcplatform.com => 122.248.234.207
 *				- Master #3: m3.iotcplatform.com => 46.137.188.54
 *				- Master #4: m4.iotcplatform.com => 122.226.84.253
 *				- Master #5: m5.iotcplatform.com => 61.188.37.216
 *				- Master #6: m6.iotcplatform.com => 120.24.59.150
 *				- Master #7: m7.iotcplatform.com => 114.215.137.159
 *				- Master #8: m8.iotcplatform.com => 104.199.156.58
 */
P2PAPI_API int IOTC_Initialize(unsigned short nUDPPort, const char* cszP2PHostNamePrimary,
								const char* cszP2PHostNameSecondary, const char* cszP2PHostNameThird,
								const char* cszP2PHostNameFourth);


/**
 * \brief Initialize IOTC module
 *
 * \details This function is used by devices or clients to initialize IOTC
 *			module and shall be called before	any IOTC module related
 *			function is invoked except for IOTC_Set_Max_Session_Number().
 *			<br>
 *			The different between this function and IOTC_Initialize() is this
 *			function uses following steps to connect masters (1) IP addresses
 *			of master (2) if fails to connect in step 1, resolve predefined
 *			domain name of masters (3) try to connect again with the resolved
 *			IP address of step 2 if IP is resolved successfully.
 *
 * \param nUDPPort [in] Specify a UDP port. Random UDP port is used if it is specified as 0.
 *
 * \return #IOTC_ER_NoERROR if initializing successfully
 * \return Error code if return value < 0
 *			- #IOTC_ER_ALREADY_INITIALIZED IOTC module is already initialized
 *			- #IOTC_ER_FAIL_CREATE_MUTEX Fails to create Mutexs
 *			- #IOTC_ER_FAIL_CREATE_THREAD Fails to create threads
 *
 * \see IOTC_Initialize(), IOTC_DeInitialize()
 *
 * \attention This function is the key entry to whole IOTC platform, including
 *				RDT module and AV module. That means, if you want to use
 *				RDT module, users shall still use this function to initialize IOTC
 *				module before calling RDT_Initialize().
 */
P2PAPI_API int IOTC_Initialize2(unsigned short nUDPPort);


/**
 * \brief Deinitialize IOTC module
 *
 * \details This function will deinitialize IOTC module.
 *
 * \return #IOTC_ER_NoERROR if deinitialize successfully
 * \return Error code if return value < 0
 *			- #IOTC_ER_NOT_INITIALIZED the IOTC module is not initialized yet.
 *
 * \see IOTC_Initialize(), IOTC_Initialize2()
 *
 * \attention IOTC_DeInitialize() will automatically close all IOTC sessions
 *				in local site while the remote site will find sessions have
 *				been closed after certain period of time. Therefore, it is
 *				suggested to close all sessions before invoking this function
 *				to ensure the remote site and real-time session status.
 */
P2PAPI_API int IOTC_DeInitialize(void);


/**
 * \brief Used by a device to login to IOTC servers
 *
 * \details This function will let a device login to IOTC servers. UID is required
 *			when login to IOTC servers. The device name and password are applicable
 *			only in LAN mode when the device cannot login to IOTC servers
 *			due to network issue.
 *
 * \param cszUID [in] The UID of that device login to IOTC servers
 * \param cszDeviceName [in] The name of that device, used in LAN mode for clients
 *			to connect
 * \param cszDevicePWD [in] The password of that device, used in LAN mode for
 *			clients to connect
 *
 * \return #IOTC_ER_NoERROR if device had obtained server list and sent login packets successfully
 * \return Error code if return value < 0
 *			- #IOTC_ER_UNLICENSE The UID is not licensed or expired
 *			- #IOTC_ER_NOT_INITIALIZED The IOTC module is not initialized yet
 *			- #IOTC_ER_LOGIN_ALREADY_CALLED The device is already under login process currently
 *			- #IOTC_ER_FAIL_CREATE_SOCKET Fails to create sockets
 *			- #IOTC_ER_FAIL_SOCKET_OPT Fails to set up socket options
 *			- #IOTC_ER_FAIL_SOCKET_BIND Fails to bind sockets
 *			- #IOTC_ER_FAIL_CREATE_THREAD Fails to create threads
 *			- #IOTC_ER_FAIL_RESOLVE_HOSTNAME Cannot resolve masters' host name
 *			- #IOTC_ER_TCP_TRAVEL_FAILED Cannot connect to masters in neither UDP nor TCP
 *			- #IOTC_ER_TCP_CONNECT_TO_SERVER_FAILED Cannot connect to IOTC servers in TCP
 *			- #IOTC_ER_NO_PERMISSION The device does not support TCP relay
 *			- #IOTC_ER_SERVER_NOT_RESPONSE IOTC servers have no response
 *			- #IOTC_ER_FAIL_GET_LOCAL_IP Fails to get the device's local IP address
 *			- #IOTC_ER_NETWORK_UNREACHABLE Network is unreachable
 *			- #IOTC_ER_NO_SERVER_LIST No IOTC server information while device login
 *
 * \attention (1) This function is a block process. That means this function will return until
 *				the device can login into IOTC servers successfully or some error happens
 *				during the process. It is suggested to use another thread to perform
 *				login process so that sequential instructions will not blocked. <br><br>
 *				(2) Please use IOTC_Get_Login_Info() to check if device really logins
 * 				into server successfully.
 */
P2PAPI_API int IOTC_Device_Login(const char *cszUID, const char *cszDeviceName, const char *cszDevicePWD);


/**
 * \brief Used by a device to get the login information
 *
 * \details This function gets the login information of a device to IOTC servers.
 *
 * \param pnLoginInfo [out] The login info with meanings of following bits
 *			- bit 0: the device is ready for connection by client from LAN if this bit is 1
 *			- bit 1: the device is ready for connection by client from Internet if this bit is 1
 *			- bit 2: if this bit is 1, it means the device has received login
 *						response from IOTC servers since IOTC_Get_Login_Info()
 *						is called last time.
 *
 * \return The number of fails to login to IOTC servers.
 *
 */
P2PAPI_API int IOTC_Get_Login_Info(unsigned int *pnLoginInfo);


/**
 * \brief Used by a device to get the login information
 *
 * \details This function gets the login information of a device to IOTC servers. <br>
 *			The difference of this function and IOTC_Get_Login_Info() is
 *			this function will set callback function inside IOTC module and
 *			that callback function will be invoked whenever the login status
 *			of that device is updated from IOTC servers, for example, IOTC
 *			servers response login message to that device or the connection
 *			between IOTC servers and that device has been lost.
 *
 * \param pfxLoginInfoFn [in] The function pointer to getting login info function
 *
 */
P2PAPI_API void IOTC_Get_Login_Info_ByCallBackFn(loginInfoCB pfxLoginInfoFn);

/**
 * \brief Used by a device to listen connections from clients
 *
 * \details This function is for a device to listen any connection from clients.
 *			If connection is established with the help of IOTC servers, the
 *			IOTC session ID will be returned in this function and then device and
 *			client can communicate for the other later by using this IOTC session ID.
 *
 * \param nTimeout [in] The timeout for this function in unit of millisecond, give 0 means block forever
 *
 * \return IOTC session ID if return value >= 0
 * \return Error code if return value < 0
 *			- #IOTC_ER_NOT_INITIALIZED The IOTC module is not initialized yet
 *			- #IOTC_ER_EXCEED_MAX_SESSION The number of IOTC sessions has reached maximum in device side
 *			- #IOTC_ER_LISTEN_ALREADY_CALLED The device is already in listen process
 *			- #IOTC_ER_TIMEOUT No connection is established from clients before timeout expires
 *			- #IOTC_ER_EXIT_LISTEN The device stops listening for connections from clients.
 *
 * \attention (1) This function is a block process, waiting for following two
 *				conditions happens before executing	sequential instructions
 *				(a) any connection from clients is established (b) timeout expires.<br><br>
 *				(2) nTimeout has no effect in 8051 platform.
 */
P2PAPI_API int  IOTC_Listen(unsigned int nTimeout);


/**
 * \brief Used by a device to exit listen process
 *
 * \details Since listen process is a block process and that means a device
 *			will wait for following two conditions happens before executing
 *			sequential instructions (1) any connection from clients is established
 *			(2) timeout expires. In some cases, users may want the device to
 *			exit listen immediately by this function in another thread before
 *			the two conditions above happens.
 */
P2PAPI_API void IOTC_Listen_Exit(void);


#if defined(_WIN32) || defined(IOTC_ARC_HOPE312) || defined(__linux__) || defined(__APPLE__) || defined(__pnacl__)
/**
 * \brief Used by a device to listen connections from clients
 *
 * \details This function is for a device to listen any connection from clients.
 *			If connection is established with the help of IOTC servers, the
 *			IOTC session ID will be returned in this function and then device and
 *			client can communicate for the other later by using this IOTC session ID.
 *			<br> <br>
 *			The difference between this function and IOTC_Listen() is that
 *			this function supports IOTC session established in secure mode. Also,
 *			by specifying IOTC_ARBITRARY_MODE as IOTC session mode, this function can
 *			let devices establish IOTC session in either non-secure mode and secure
 *			mode according to clients' request.
 *
 * \param nTimeout [in] The timeout for this function in unit of millisecond, give 0 means block forever
 * \param cszAESKey [in] The AES key for certification. Specify it as NULL will make
 *			IOTC module use predefined AES key.
 * \param nSessionMode [in] The IOTC session mode that a device want to connect.
 *			Please refer to #IOTCSessionMode for more detail
 *
 * \return IOTC session ID if return value >= 0
 * \return Error code if return value < 0
 *			- #IOTC_ER_NOT_INITIALIZED The IOTC module is not initialized yet
 *			- #IOTC_ER_EXCEED_MAX_SESSION The number of IOTC sessions has reached maximum in device side
 *			- #IOTC_ER_LISTEN_ALREADY_CALLED The device is already in listen process
 *			- #IOTC_ER_TIMEOUT No connection is established from clients before timeout expires
 *			- #IOTC_ER_EXIT_LISTEN The device stops listening for connections from clients.
 *			- #IOTC_ER_INVALID_MODE The IOTC session mode is not valid. Please refer to #IOTCSessionMode
 *			- #IOTC_ER_CLIENT_NOT_SECURE_MODE A client wants to connect to a device in
 *					non-secure mode while that device supports secure mode only.
 *			- #IOTC_ER_CLIENT_SECURE_MODE A client wants to connect to a device
 *					in secure mode while that device does not support secure mode.
 *			- #IOTC_ER_AES_CERTIFY_FAIL The AES certification fails
 *
 * \attention (1) This function is available on Win32, Linux, Android, iOS and ARC platforms.<br><br>
 *				(2) The AES key shall be matched between a device and a client
 *				in order to establish connection successfully.
 */
P2PAPI_API int  IOTC_Listen2(unsigned int nTimeout, const char *cszAESKey, IOTCSessionMode nSessionMode);


/**
 * \brief Used by a client to connect a device
 *
 * \details This function is for a client to connect a device by specifying
 *			the UID of that device. If connection is established with the
 *			help of IOTC servers, the IOTC session ID will be returned in this
 *			function and then device and client can communicate for the other
 *			later by using this IOTC session ID.
 *
 * \param cszUID [in] The UID of a device that client wants to connect
 *
 * \return IOTC session ID if return value >= 0
 * \return Error code if return value < 0
 *			- #IOTC_ER_NOT_INITIALIZED The IOTC module is not initialized yet
 *			- #IOTC_ER_CONNECT_IS_CALLING The client is already connecting to a device
 *			- #IOTC_ER_UNLICENSE The specified UID of that device is not licensed or expired
 *			- #IOTC_ER_EXCEED_MAX_SESSION The number of IOTC sessions has reached maximum in client side
 *			- #IOTC_ER_DEVICE_NOT_LISTENING The device is not listening for connection now
 *			- #IOTC_ER_FAIL_CONNECT_SEARCH The client stop connecting to the device
 *			- #IOTC_ER_FAIL_RESOLVE_HOSTNAME Cannot resolve masters' host name
 *			- #IOTC_ER_FAIL_CREATE_THREAD Fails to create threads
 *			- #IOTC_ER_TCP_TRAVEL_FAILED Cannot connect to masters in neither UDP nor TCP
 *			- #IOTC_ER_TCP_CONNECT_TO_SERVER_FAILED Cannot connect to IOTC servers in TCP
 *			- #IOTC_ER_CAN_NOT_FIND_DEVICE IOTC servers cannot locate the specified device
 *			- #IOTC_ER_NO_PERMISSION The specified device does not support TCP relay
 *			- #IOTC_ER_SERVER_NOT_RESPONSE IOTC servers have no response
 *			- #IOTC_ER_FAIL_GET_LOCAL_IP Fails to get the client's local IP address
 *			- #IOTC_ER_FAIL_SETUP_RELAY Fails to connect the device via relay mode
 *			- #IOTC_ER_FAIL_CREATE_SOCKET Fails to create sockets
 *			- #IOTC_ER_FAIL_SOCKET_OPT Fails to set up socket options
 *			- #IOTC_ER_FAIL_SOCKET_BIND Fails to bind sockets
 *			- #IOTC_ER_NOT_SUPPORT_RELAY Not support relay connection by IOTC servers
 *			- #IOTC_ER_NO_SERVER_LIST No IOTC server information while client connect
 *			- #IOTC_ER_DEVICE_MULTI_LOGIN The connecting device duplicated loggin and may unconnectable
 *
 * \attention This process is a block process.
 *
 */
P2PAPI_API int  IOTC_Connect_ByUID(const char *cszUID);

/**
* \brief Used by a client to get a tutk_platform_free session ID.
*
* \details This function is for a client to get a tutk_platform_free session ID used for a parameter of
*          IOTC_Connect_ByUID_Parallel().
*
* \return IOTC session ID if return value >= 0
* \return Error code if return value < 0
*			- #IOTC_ER_EXCEED_MAX_SESSION The number of IOTC sessions has reached maximum in client side
*
* \attention (1) If you already get a session ID via this function must call IOTC_Connect_ByUID_Parallel() and then
*                 it will release session ID resource automatically when connection failed.<br><br>
*            (2) If you already get a session ID via this function and want to close it please use IOTC_Session_Close().
*
*/
P2PAPI_API int  IOTC_Get_SessionID(void);

/**
 * \brief Used by a client to connect a device and bind to a specified session ID.
 *
 * \details This function is for a client to connect a device by specifying
 *			the UID of that device, and bind to a tutk_platform_free session ID from IOTC_Get_SessionID().
 * 			If connection is established with the help of IOTC servers,
 *			the #IOTC_ER_NoERROR will be returned in this function and then device and
 *			client can communicate for the other later by using this IOTC session ID.
 *			If this function is called by multiple threads, the connections will be
 *			processed concurrently.
 *
 * \param cszUID [in] The UID of a device that client wants to connect
 * \param SID [in] The Session ID got from IOTC_Get_SessionID() the connection should bind to.
 *
 * \return IOTC session ID if return value >= 0 and equal to the input parameter SID.
 * \return Error code if return value < 0
 *			- #IOTC_ER_NOT_INITIALIZED The IOTC module is not initialized yet
 *			- #IOTC_ER_UNLICENSE The specified UID of that device is not licensed or expired
 *			- #IOTC_ER_EXCEED_MAX_SESSION The number of IOTC sessions has reached maximum in client side
 *			- #IOTC_ER_DEVICE_NOT_LISTENING The device is not listening for connection now
 *			- #IOTC_ER_FAIL_CONNECT_SEARCH The client stop connecting to the device
 *			- #IOTC_ER_FAIL_RESOLVE_HOSTNAME Cannot resolve masters' host name
 *			- #IOTC_ER_FAIL_CREATE_THREAD Fails to create threads
 *			- #IOTC_ER_TCP_TRAVEL_FAILED Cannot connect to masters in neither UDP nor TCP
 *			- #IOTC_ER_TCP_CONNECT_TO_SERVER_FAILED Cannot connect to IOTC servers in TCP
 *			- #IOTC_ER_CAN_NOT_FIND_DEVICE IOTC servers cannot locate the specified device
 *			- #IOTC_ER_NO_PERMISSION The specified device does not support TCP relay
 *			- #IOTC_ER_SERVER_NOT_RESPONSE IOTC servers have no response
 *			- #IOTC_ER_FAIL_GET_LOCAL_IP Fails to get the client's local IP address
 *			- #IOTC_ER_FAIL_SETUP_RELAY Fails to connect the device via relay mode
 *			- #IOTC_ER_FAIL_CREATE_SOCKET Fails to create sockets
 *			- #IOTC_ER_FAIL_SOCKET_OPT Fails to set up socket options
 *			- #IOTC_ER_FAIL_SOCKET_BIND Fails to bind sockets
 *			- #IOTC_ER_NOT_SUPPORT_RELAY Not support relay connection by IOTC servers
 *			- #IOTC_ER_NO_SERVER_LIST No IOTC server information while client connect
 *			- #IOTC_ER_DEVICE_MULTI_LOGIN The connecting device duplicated loggin and may unconnectable
 *			- #IOTC_ER_INVALID_SID The specified IOTC session ID is not valid
 *
 * \attention If you call IOTC_Connect_Stop_BySID() and this function not return yet, and then use the same
 *              session ID to call again will cause session ID in wrong status.
 *
 */
P2PAPI_API int  IOTC_Connect_ByUID_Parallel(const char *cszUID, int SID);


/**
 * \brief Used by a client to connect a device
 *
 * \details This function is for a client to connect a device by specifying
 *			the UID of that device. If connection is established with the
 *			help of IOTC servers, the IOTC session ID will be returned in this
 *			function and then device and client can communicate for the other
 *			later by using this IOTC session ID.
 *			<br> <br>
 *			The different between this function and IOTC_Connect_ByUID() is
 *			that this function supports IOTC session established in secure mode.
 *			Also, by specifying IOTC_ARBITRARY_MODE as IOTC session mode, this
 *			function can let clients establish IOTC session in either non-secure
 *			mode and secure	mode according to devices' secure settings.
 *
 * \param cszUID [in] The UID of a device that client wants to connect
 * \param cszAESKey [in] The AES key for certification. Specify it as NULL will make
 *			IOTC module use predefined AES key.
 * \param nSessionMode [in] The IOTC session mode that a client want to connect.
 *			Please refer to #IOTCSessionMode for more detail
 *
 * \return IOTC session ID if return value >= 0
 * \return Error code if return value < 0
 *			- #IOTC_ER_NOT_INITIALIZED The IOTC module is not initialized yet
 *			- #IOTC_ER_CONNECT_IS_CALLING The client is already connecting to a device
 *			- #IOTC_ER_UNLICENSE The specified UID of that device is not licensed or expired
 *			- #IOTC_ER_EXCEED_MAX_SESSION The number of IOTC sessions has reached maximum in client side
 *			- #IOTC_ER_DEVICE_NOT_LISTENING The device is not listening for connection now
 *			- #IOTC_ER_FAIL_CONNECT_SEARCH The client stop connecting to the device
 *			- #IOTC_ER_FAIL_RESOLVE_HOSTNAME Cannot resolve masters' host name
 *			- #IOTC_ER_FAIL_CREATE_THREAD Fails to create threads
 *			- #IOTC_ER_TCP_TRAVEL_FAILED Cannot connect to masters in neither UDP nor TCP
 *			- #IOTC_ER_TCP_CONNECT_TO_SERVER_FAILED Cannot connect to IOTC servers in TCP
 *			- #IOTC_ER_CAN_NOT_FIND_DEVICE IOTC servers cannot locate the specified device
 *			- #IOTC_ER_NO_PERMISSION The specified device does not support TCP relay
 *			- #IOTC_ER_SERVER_NOT_RESPONSE IOTC servers have no response
 *			- #IOTC_ER_FAIL_GET_LOCAL_IP Fails to get the client's local IP address
 *			- #IOTC_ER_FAIL_SETUP_RELAY Fails to connect the device via relay mode
 *			- #IOTC_ER_INVALID_MODE The IOTC session mode is not valid. Please refer to #IOTCSessionMode
 *			- #IOTC_ER_DEVICE_NOT_SECURE_MODE A client wants to connect to a device in
 *					secure mode while that device supports non-secure mode only.
 *			- #IOTC_ER_DEVICE_SECURE_MODE A client wants to connect to a device
 *					in non-secure mode while that device supports secure mode only.
 *			- #IOTC_ER_AES_CERTIFY_FAIL The AES certification fails
 *			- #IOTC_ER_FAIL_CREATE_SOCKET Fails to create sockets
 *			- #IOTC_ER_FAIL_SOCKET_OPT Fails to set up socket options
 *			- #IOTC_ER_FAIL_SOCKET_BIND Fails to bind sockets
 *			- #IOTC_ER_NOT_SUPPORT_RELAY Not support relay connection by IOTC servers
 *			- #IOTC_ER_NO_SERVER_LIST No IOTC server information while client connect
 *			- #IOTC_ER_DEVICE_MULTI_LOGIN The connecting device duplicated loggin and may unconnectable
 *
 * \attention (1) This process is a block process.<br><br>
 *				(2) The AES key shall be matched between a device and a client in
 *				order to establish connection successfully.<br><br>
 *				(3) This function is available on Win32, Linux, Android, iOS and ARC platforms.
 *
 */
P2PAPI_API int  IOTC_Connect_ByUID2(const char *cszUID, const char *cszAESKey, IOTCSessionMode nSessionMode);

/**
 * \brief Used by a client to stop connecting a device
 *
 * \details This function is for a client to stop connecting a device. Since
 *			IOTC_Connect_ByUID(), IOTC_Connect_ByUID2() are all block processes, that means
 *			the client will have to wait for the return of these functions before
 *			executing sequential instructions. In some cases, users may want
 *			the client to stop connecting immediately by this function in
 *			another thread before the return of connection process.
 *
 * \attention Only use to stop IOTC_Connect_ByUID() and 2, NOT use to stop IOTC_Connect_ByUID_Parallel().
 */
P2PAPI_API void IOTC_Connect_Stop(void);

/**
 * \brief Used by a client to stop a specific session connecting a device
 *
 * \details This function is for a client to stop connecting a device. Since
 *			IOTC_Connect_ByUID_Parallel() is a block processes, that means
 *			the client will have to wait for the return of these functions before
 *			executing sequential instructions. In some cases, users may want
 *			the client to stop connecting immediately by this function in
 *			another thread before the return of connection process.
 *
 * \param SID [in] The Session ID of a connection which will be stop.
 *
 * \return #IOTC_ER_NoERROR
 * \return Error code if return value < 0
 *			- #IOTC_ER_NOT_INITIALIZED The IOTC module is not initialized yet
 *			- #IOTC_ER_INVALID_SID The specified IOTC session ID is not valid
 *			- #IOTC_ER_SESSION_CLOSE_BY_REMOTE The IOTC session of specified
 *				session ID has been closed by remote site
 *			- #IOTC_ER_REMOTE_TIMEOUT_DISCONNECT The timeout defined by #IOTC_SESSION_ALIVE_TIMEOUT
 *				expires because	remote site has no response
 */
P2PAPI_API int IOTC_Connect_Stop_BySID(int SID);

/**
 * \brief Used by a device or a client to read data from the other
 *
 * \details A device or a client uses this function to read data through
 *			a specific IOTC channel in a IOTC session. <br>
 *			The difference between this function and IOTC_Session_Read() is
 *			this function provides packet lost information. Users may use
 *			this to check how many packets, if any, have been lost since the last
 *			time reading from this session.
 *
 * \param nIOTCSessionID [in] The session ID of the IOTC session to read data
 * \param abBuf [out] The array of byte buffer to receive read result
 * \param nMaxBufSize [in] The maximum length of the byte buffer
 * \param nTimeout [in] The timeout for this function in unit of millisecond, give 0 means return immediately
 * \param pnPacketSN [out] The serial number of the packet that is read successfully
 *							this time. Could be NULL.
 * \param pbFlagLost [out] A boolean value to indicate if there are some packets
 *							lost between this time and previous successful read.
 *							Could be NULL.
 * \param nIOTCChannelID [in] The IOTC channel ID in this IOTC session to read data
 *
 * \return The actual length of read result stored in abBuf if read successfully
 * \return Error code if return value < 0
 *			- #IOTC_ER_NOT_INITIALIZED The IOTC module is not initialized yet
 *			- #IOTC_ER_INVALID_SID The specified IOTC session ID is not valid
 *			- #IOTC_ER_CH_NOT_ON The IOTC channel of specified channel ID is not turned on
 *			- #IOTC_ER_SESSION_CLOSE_BY_REMOTE The IOTC session of specified
 *				session ID has been closed by remote site
 *			- #IOTC_ER_REMOTE_TIMEOUT_DISCONNECT The timeout defined by #IOTC_SESSION_ALIVE_TIMEOUT
 *				expires because	remote site has no response
 *			- #IOTC_ER_TIMEOUT The timeout specified by nTimeout expires before
 *				read process is performed completely
 *
 * \attention The IOTC channel of ID 0 is enabled by default when a IOTC session is established.
 *				That means nIOTCChannelID can be specified as 0 if only one IOTC channel
 *				is needed by the user. If more IOTC channels are required, users
 *				should use IOTC_Session_Get_Free_Channel() and IOTC_Session_Channel_ON()
 *				to get more IOTC channel IDs and then specifying those IOTC channel IDs
 *				in this function according to the purpose defined by the user.
 */
P2PAPI_API int  IOTC_Session_Read_Check_Lost(int nIOTCSessionID, char *abBuf, int nMaxBufSize,unsigned int nTimeout, unsigned short *pnPacketSN,char *pbFlagLost, unsigned char nIOTCChannelID);
#endif // defined(_WIN32) || defined(IOTC_ARC_HOPE312) || defined(__linux__)


/**
 * \brief Used by a device or a client to check the IOTC session info
 *
 * \details A device or a client may use this function to check if the IOTC session
 *			is still alive as well as getting the IOTC session info.
 *
 * \param nIOTCSessionID [in] The session ID of the IOTC session to be checked
 * \param psSessionInfo [out] The session info of specified IOTC session
 *
 * \return #IOTC_ER_NoERROR if getting the IOTC session info successfully
 * \return Error code if return value < 0
 *			- #IOTC_ER_NOT_INITIALIZED The IOTC module is not initialized yet
 *			- #IOTC_ER_INVALID_SID The specified IOTC session ID is not valid
 *			- #IOTC_ER_SESSION_CLOSE_BY_REMOTE The IOTC session of specified
 *				session ID has been closed by remote site
 *			- #IOTC_ER_REMOTE_TIMEOUT_DISCONNECT The timeout defined by #IOTC_SESSION_ALIVE_TIMEOUT
 *				expires because	remote site has no response
 */
P2PAPI_API int  IOTC_Session_Check(int nIOTCSessionID, struct st_SInfo *psSessionInfo);

/**
 * \brief Used by a device or a client to check the IOTC session info
 *
 * \details A device or a client may use this function to check if the IOTC session
 *			is still alive as well as getting the IOTC session info. The structure
 *          size must be filled. For example "info.size=sizeof(struct st_SInfoEx);"
 *
 * \param nIOTCSessionID [in] The session ID of the IOTC session to be checked
 * \param psSessionInfo [out] The session info of specified IOTC session
 *
 * \return #IOTC_ER_NoERROR if getting the IOTC session info successfully
 * \return Error code if return value < 0
 *			- #IOTC_ER_NOT_INITIALIZED The IOTC module is not initialized yet
 *			- #IOTC_ER_INVALID_SID The specified IOTC session ID is not valid
 *			- #IOTC_ER_SESSION_CLOSE_BY_REMOTE The IOTC session of specified
 *				session ID has been closed by remote site
 *			- #IOTC_ER_REMOTE_TIMEOUT_DISCONNECT The timeout defined by #IOTC_SESSION_ALIVE_TIMEOUT
 *				expires because	remote site has no response
 *          - #IOTC_ER_INVALID_ARG The size of structure is not filled
 */
P2PAPI_API int  IOTC_Session_Check_Ex(int nIOTCSessionID, struct st_SInfoEx *psSessionInfo);

/**
 * \brief Used by a device or a client to get the session status
 *
 * \details This function gets the session status between device and client. <br>
 *			The difference of this function and IOTC_Session_Check() is
 *			this function will set callback function inside IOTC module and
 *			that callback function will be invoked whenever the session status
 *			between device and client disconnected, for example, IOTC
 *			alive timeout or one side call IOTC_Session_Close() to close
 *			this session.

 * \param nIOTCSessionID [in] The session ID of the IOTC session to check status
 * \param pfxSessionStatusFn [in] The function pointer to getting session status function
 *
 * \return #IOTC_ER_NoERROR if getting the IOTC session info successfully
 * \return Error code if return value < 0
 *			- #IOTC_ER_NOT_INITIALIZED The IOTC module is not initialized yet
 *			- #IOTC_ER_INVALID_SID The specified IOTC session ID is not valid
 *			- #IOTC_ER_SESSION_CLOSE_BY_REMOTE The IOTC session of specified
 *				session ID has been closed by remote site
 *			- #IOTC_ER_REMOTE_TIMEOUT_DISCONNECT The timeout defined by #IOTC_SESSION_ALIVE_TIMEOUT
 *				expires because	remote site has no response
 */
P2PAPI_API int IOTC_Session_Check_ByCallBackFn(int nIOTCSessionID, sessionStatusCB pfxSessionStatusFn);

/**
 * \brief Used by a device or a client to read data from the other
 *
 * \details A device or a client uses this function to read data through
 *			a specific IOTC channel in a IOTC session.
 *
 * \param nIOTCSessionID [in] The session ID of the IOTC session to read data
 * \param abBuf [out] The array of byte buffer to receive read result
 * \param nMaxBufSize [in] The maximum length of the byte buffer
 * \param nTimeout [in] The timeout for this function in unit of millisecond, give 0 means return immediately
 * \param nIOTCChannelID [in] The IOTC channel ID in this IOTC session to read data
 *
 * \return The actual length of read result stored in abBuf if read successfully
 * \return Error code if return value < 0
 *			- #IOTC_ER_NOT_INITIALIZED The IOTC module is not initialized yet
 *			- #IOTC_ER_INVALID_SID The specified IOTC session ID is not valid
 *			- #IOTC_ER_CH_NOT_ON The IOTC channel of specified channel ID is not turned on
 *			- #IOTC_ER_SESSION_CLOSE_BY_REMOTE The IOTC session of specified
 *				session ID has been closed by remote site
 *			- #IOTC_ER_REMOTE_TIMEOUT_DISCONNECT The timeout defined by #IOTC_SESSION_ALIVE_TIMEOUT
 *				expires because	remote site has no response
 *			- #IOTC_ER_TIMEOUT The timeout specified by nTimeout expires before
 *				read process is performed completely
 *
 * \attention (1) The IOTC channel of ID 0 is enabled by default when a IOTC session is established.
 *				That means nIOTCChannelID can be specified as 0 if only one IOTC channel
 *				is needed by the user. If more IOTC channels are required, users
 *				should use IOTC_Session_Get_Free_Channel() and IOTC_Session_Channel_ON()
 *				to get more IOTC channel IDs and then specifying those IOTC channel IDs
 *				in this function according to the purpose defined by the user.<br><br>
 *				(2) If the size of abBuf, i.e. defined by nMaxBufSize, is less than
 *				the size of data to be read, then this function will only read
 *				the data up to nMaxBufSize and the remaining part will be truncated without
 *				error code returned. Therefore, it is suggested to allocate the size
 *				of abBuf as #IOTC_MAX_PACKET_SIZE for ensure complete reading.
 *				(3) This function will need 192 bytes stack size.
 *
 */
P2PAPI_API int  IOTC_Session_Read(int nIOTCSessionID, char *abBuf, int nMaxBufSize, unsigned int nTimeout, unsigned char nIOTCChannelID);


/**
 * \brief Used by a device or a client to write data to the other
 *
 * \details A device or a client uses this function to write data through
 *			a specific IOTC channel in a IOTC session.
 *
 * \param nIOTCSessionID [in] The session ID of the IOTC session to write data
 * \param cabBuf [in] The array of byte buffer containing the data to write.
 *			Its size cannot be larger than #IOTC_MAX_PACKET_SIZE
 * \param nBufSize [in] The length of the byte buffer. It cannot be larger than
 *			#IOTC_MAX_PACKET_SIZE
 * \param nIOTCChannelID [in] The IOTC channel ID in this IOTC session to write data
 *
 * \return The actual length of buffer to be written if write successfully. In non-blocking
 *         mode, the length with zero usually means the socket buffer is full and unable to
 *         write into.
 * \return Error code if return value < 0
 *			- #IOTC_ER_NOT_INITIALIZED The IOTC module is not initialized yet
 *			- #IOTC_ER_INVALID_SID The specified IOTC session ID is not valid
 *			- #IOTC_ER_CH_NOT_ON The IOTC channel of specified channel ID is not turned on
 *			- #IOTC_ER_SESSION_CLOSE_BY_REMOTE The IOTC session of specified
 *				session ID has been closed by remote site
 *			- #IOTC_ER_REMOTE_TIMEOUT_DISCONNECT The timeout defined by #IOTC_SESSION_ALIVE_TIMEOUT
 *				expires because	remote site has no response
 *
 * \attention (1) The IOTC channel of ID 0 is enabled by default when a IOTC session is established.
 *				That means nIOTCChannelID can be specified as 0 if only one IOTC channel
 *				is needed by the user. If more IOTC channels are required, users
 *				should use IOTC_Session_Get_Free_Channel() and IOTC_Session_Channel_ON()
 *				to get more IOTC channel IDs and then specifying those IOTC channel IDs
 *				in this function according to the purpose defined by the user. <br><br>
 *				(2) This function will block when session is connected via TCP and socket buffer is full.
 *				(3) This function will need 4976 bytes stack size.
 *
 */
P2PAPI_API int IOTC_Session_Write(int nIOTCSessionID, const char *cabBuf, int nBufSize, unsigned char nIOTCChannelID);


/**
 * \brief Used by a device or a client to close a IOTC session
 *
 * \details A device or a client uses this function to close a IOTC session
 *			specified by its session ID if this IOTC session is no longer
 *			required.
 *
 * \param nIOTCSessionID [in] The session ID of the IOTC session to be closed
 *
 */
P2PAPI_API void IOTC_Session_Close(int nIOTCSessionID);


/**
 * \brief Used by a device or a client to get a tutk_platform_free IOTC channel
 *
 * \details A device or a client uses this function to get a tutk_platform_free IOTC channel
 *			in a specified IOTC session. By default, IOTC channel of ID 0 is turned on
 *			once a IOTC session is established. If more IOTC channels are required
 *			by users, this function can always return a tutk_platform_free IOTC channel until
 *			maximum IOTC channels are reached.
 *
 * \param nIOTCSessionID [in] The session ID of the IOTC session to get tutk_platform_free IOTC channel
 *
 * \return The IOTC channel ID of a tutk_platform_free IOTC channel if successfully
 * \return Error code if return value < 0
 *			- #IOTC_ER_NOT_INITIALIZED The IOTC module is not initialized yet
 *			- #IOTC_ER_INVALID_SID The specified IOTC session ID is not valid
 *			- #IOTC_ER_SESSION_CLOSE_BY_REMOTE The IOTC session of specified
 *				session ID has been closed by remote site
 *			- #IOTC_ER_REMOTE_TIMEOUT_DISCONNECT The timeout defined by #IOTC_SESSION_ALIVE_TIMEOUT
 *				expires because	remote site has no response
 *			- #IOTC_ER_SESSION_NO_FREE_CHANNEL Already reach the maximum
 *				number of IOTC channels, no more tutk_platform_free IOTC channel is available
 *
 * \attention (1) The IOTC channel returned by this function is already turned on.<br><br>
 *				(2) The IOTC channel is only turned on in the local site
 *				calling	this function. That means, the remote site shall use
 *				IOTC_Session_Channel_ON() to turn on the same IOTC channel at its
 *				side before communication.
 */
P2PAPI_API int IOTC_Session_Get_Free_Channel(int nIOTCSessionID);


/**
 * \brief Used by a device or a client to turn on a IOTC channel
 *
 * \details A device or a client uses this function to turn on a IOTC channel
 *			before sending or receiving data through this IOTC channel.
 *
 * \param nIOTCSessionID [in] The session ID of the IOTC session containing the
 *			IOTC channel to be turned on
 * \param nIOTCChannelID [in] The channel ID of the IOTC channel to be turned on
 *
 * \return IOTC_ER_NoERROR if turning on the IOTC channel successfully
 * \return Error code if return value < 0
 *			- #IOTC_ER_NOT_INITIALIZED The IOTC module is not initialized yet
 *			- #IOTC_ER_INVALID_SID The specified IOTC session ID is not valid
 *			- #IOTC_ER_SESSION_CLOSE_BY_REMOTE The IOTC session of specified
 *				session ID has been closed by remote site
 *			- #IOTC_ER_REMOTE_TIMEOUT_DISCONNECT The timeout defined by #IOTC_SESSION_ALIVE_TIMEOUT
 *				expires because	remote site has no response
 *			- #IOTC_ER_CH_NOT_ON The specified IOTC channel ID is not valid
 *
 * \attention The IOTC channel is only turned on in the local site calling
 *				this function. That means, the remote site shall also use
 *				IOTC_Session_Channel_ON() to turn on the same IOTC channel
 *				at its side	before communication.
 */
P2PAPI_API int IOTC_Session_Channel_ON(int nIOTCSessionID, unsigned char nIOTCChannelID);


/**
 * \brief Used by a device or a client to turn off a IOTC channel
 *
 * \details A device or a client uses this function to turn off a IOTC channel
 *			when this IOTC channel is no longer needed for communication.
 *
 * \param nIOTCSessionID [in] The session ID of the IOTC session containing the
 *			IOTC channel to be turned off
 * \param nIOTCChannelID [in] The channel ID of the IOTC channel to be turned off
 *
 * \return IOTC_ER_NoERROR if turning off the IOTC channel successfully
 * \return Error code if return value < 0
 *			- #IOTC_ER_NOT_INITIALIZED The IOTC module is not initialized yet
 *			- #IOTC_ER_INVALID_SID The specified IOTC session ID is not valid
 *			- #IOTC_ER_SESSION_CLOSE_BY_REMOTE The IOTC session of specified
 *				session ID has been closed by remote site
 *			- #IOTC_ER_REMOTE_TIMEOUT_DISCONNECT The timeout defined by #IOTC_SESSION_ALIVE_TIMEOUT
 *				expires because	remote site has no response
 *			- #IOTC_ER_CH_NOT_ON The specified IOTC channel ID is not valid
 *
 * \attention (1) IOTC Channel 0 cannot be turned off because it is a default channel.<br><br>
 *				(2) Turn off a IOTC channel will also make the data remaining
 *				in receiving queue of this channel be deleted.
 */
P2PAPI_API int IOTC_Session_Channel_OFF(int nIOTCSessionID, unsigned char nIOTCChannelID);


/**
 * \brief Used by search devices in LAN.
 *
 * \details When clients and devices are stay in a LAN environment, client can call this function
 *			to discovery devices and connect it directly.
 *
 * \param psLanSearchInfo [in] The array of struct st_LanSearchInfo to store search result
 *
 * \param nArrayLen [in] The size of the psLanSearchInfo array
 *
 * \param nWaitTimeMs [in] The timeout in milliseconds before discovery process end.
 *
 * \return IOTC_ER_NoERROR if search devices in LAN successfully
 * \return Error code if return value < 0
 *			- #IOTC_ER_INVALID_ARG The arguments passed in to this function is invalid.
 *			- #IOTC_ER_FAIL_CREATE_SOCKET Fails to create sockets
 *			- #IOTC_ER_FAIL_SOCKET_OPT Fails to set up socket options
 *			- #IOTC_ER_FAIL_SOCKET_BIND Fails to bind sockets
 */
P2PAPI_API int IOTC_Lan_Search(struct st_LanSearchInfo *psLanSearchInfo, int nArrayLen, int nWaitTimeMs);

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
P2PAPI_API	void IOTC_Set_Log_Path(char *path, int nMaxSize);

/**
 * \brief Set partial encode On/Off
 *
 * \details Set partial encode On/Off
 *
 * \param nIOTCSessionID [in] The session ID of the IOTC session to be encrypted.
 *
 * \param bPartialEncryption [in] 1: Enable partial encode, 0: Disable partial encode
 *
 * \return Error code if return value < 0
 *			- #IOTC_ER_NOT_INITIALIZED The IOTC module is not initialized yet
 *			- #IOTC_ER_INVALID_SID The specified IOTC session ID is not valid
 *			- #IOTC_ER_SESSION_CLOSE_BY_REMOTE The IOTC session of specified
 *				session ID has been closed by remote site
 *			- #IOTC_ER_REMOTE_TIMEOUT_DISCONNECT The timeout defined by #IOTC_SESSION_ALIVE_TIMEOUT
 *				expires because	remote site has no response
 *			- #IOTC_ER_NOT_SUPPORT_PE The remote device don't support partial encryption
 *
 */
P2PAPI_API	int IOTC_Set_Partial_Encryption(int nIOTCSessionID, unsigned char bPartialEncryption);

/**
 * \brief Set device name.
 *
 * \details Device can let client know its name when client call IOTC_Lan_Search2().
 *          The maximum size of device name is 131 Byte. We filled in 0 at the 132th Byte.
 *
 * \param cszDeviceName  [in] This is user-defined device name. Clients will get it by calling IOTC_Lan_Search2().
 *
 */
P2PAPI_API  void IOTC_Set_Device_Name(const char *cszDeviceName);

/**
 * \brief Used for searching devices in LAN.
 *
 * \details When client and devices are in LAN, client can search devices and their name
 *			by calling this function.
 *
 * \param psLanSearchInfo2 [in] The array of struct st_LanSearchInfo2 store the search result and Device name.
 *
 * \param nArrayLen [in] The size of psLanSearchInfo2 array
 *
 * \param nWaitTimeMs [in] Period (or timeout) of searching LAN. (milliseconds)
 *
 * \return IOTC_ER_NoERROR if search devices in LAN successfully
 * \return Error code if return value < 0
 *			- #IOTC_ER_INVALID_ARG The arguments passed in to this function is invalid.
 *			- #IOTC_ER_FAIL_CREATE_SOCKET Fails to create sockets
 *			- #IOTC_ER_FAIL_SOCKET_OPT Fails to set up socket options
 *			- #IOTC_ER_FAIL_SOCKET_BIND Fails to bind sockets
 */
P2PAPI_API  int IOTC_Lan_Search2(struct st_LanSearchInfo2 *psLanSearchInfo2, int nArrayLen, int nWaitTimeMs);

/**
 * \brief Used for searching devices in LAN.
 *
 * \details When client and devices are in LAN, client can search devices and their name
 *			by calling this function.
 *
 * \param psLanSearchInfo2 [in] The array of struct st_LanSearchInfo2 store the search result and Device name.
 * \param nArrayLen [in] The size of psLanSearchInfo2 array
 * \param nWaitTimeMs [in] Period (or timeout) of searching LAN. (milliseconds)
 * \param nSendIntervalMs [in] Interval of sending broadcast for searching device in LAN. (milliseconds)
 *
 * \return IOTC_ER_NoERROR if search devices in LAN successfully
 * \return Error code if return value < 0
 *			- #IOTC_ER_INVALID_ARG The arguments passed in to this function is invalid.
 *			- #IOTC_ER_FAIL_CREATE_SOCKET Fails to create sockets
 *			- #IOTC_ER_FAIL_SOCKET_OPT Fails to set up socket options
 *			- #IOTC_ER_FAIL_SOCKET_BIND Fails to bind sockets
 */
P2PAPI_API int IOTC_Lan_Search2_Ex(struct st_LanSearchInfo2 *psLanSearchInfo2, int nArrayLen, int nWaitTimeMs, int nSendIntervalMs);

/**
* \brief Start to search devices in LAN.
*
* \details When client and devices are in LAN, client can search devices and their name
*			and the result can be polled by function IOTC_Search_Device_Result
*
* \param nWaitTimeMs [in] Period (or timeout) of searching LAN. (milliseconds)
* \param nSendIntervalMs [in] Interval of sending broadcast for searching device in LAN. (milliseconds)
*
* \return IOTC_ER_NoERROR if search devices in LAN successfully
* \return Error code if return value < 0
*			- #IOTC_ER_INVALID_ARG The arguments passed in to this function is invalid.
*			- #IOTC_ER_FAIL_CREATE_SOCKET Fails to create sockets
*			- #IOTC_ER_FAIL_SOCKET_OPT Fails to set up socket options
*			- #IOTC_ER_FAIL_SOCKET_BIND Fails to bind sockets
*			- #IOTC_ER_STILL_IN_PROCESSING The function is called more then once before it stopped
*			- #IOTC_ER_NOT_ENOUGH_MEMORY not enough memory
*/
P2PAPI_API int IOTC_Search_Device_Start(int nWaitTimeMs, int nSendIntervalMs);

/**
* \brief Poll the results of searched device in LAN.
*
* \details Use the function to poll the result of device search in LAN, the IOTC_Search_Device_Start should be called
*			before use the function
*
* \param st_SearchDeviceInfo [in] The array of struct st_SearchDeviceInfo store the search result and Device name.
* \param nArrayLen [in] The length of array
* \param nGetAll [in] 0: get new queried device 1: get all queried devices
*
* \return IOTC_ER_NoERROR if search devices in LAN successfully
* \return Error code if return value < 0
*			- #IOTC_ER_INVALID_ARG The arguments passed in to this function is invalid.
*			- #IOTC_ER_SERVICE_IS_NOT_STARTED The start function is not called
*/
P2PAPI_API int IOTC_Search_Device_Result(struct st_SearchDeviceInfo *psSearchDeviceInfo, int nArrayLen, int nGetAll);

/**
* \brief Stop to search devices in LAN.
*
* \details Stop to do device search in LAN, the IOTC_Search_Device_Start should be called
*			before use the function
*
* \param st_SearchDeviceInfo [in] The array of struct st_SearchDeviceInfo store the search result and Device name.
* \param nArrayLen [in] The length of array
* \param nGetAll [in] 0: get new queried device 1: get all queried devices
*
* \return IOTC_ER_NoERROR if search devices in LAN successfully
* \return Error code if return value < 0
*			- #IOTC_ER_SERVICE_IS_NOT_STARTED The start function is not called
*/
P2PAPI_API int IOTC_Search_Device_Stop();

/**
 * \brief Turn on only TCP relay mode function.
 *
 * \details When device call this will login via TCP and only can be connected via LAN or TCP relay mode.
 *			When client call this will connect to device only via LAN or TCP relay mode.
 *
 * \attention	Can be called anywhere.
 *
 */
P2PAPI_API void IOTC_TCPRelayOnly_TurnOn(void);

/**
 * \brief Setup LAN search and LAN connection timeout
 *
 * \details Only client can call this, it can determine how many time to try LAN search and LAN connection.
 *			Once it called the timeout value is effective forever until IOTC_DeInitialize() be called.
 *
 * \param nTimeout [in] The timeout for this function in unit of millisecond, give 0 means skip LAN flow
 *
 * \attention	Mast be called before start connection. Minimum is 100 millisecond.
 *
 */
P2PAPI_API void IOTC_Setup_LANConnection_Timeout(unsigned int nTimeout);

/**
* \brief Setup P2P connection timeout
*
* \details Only client can call this, it can determine how many time to try P2P connection.
*			Once it called the timeout value is effective forever until IOTC_DeInitialize() be called.
*
* \param nTimeout [in] The timeout for this function in unit of millisecond, give 0 means skip P2P flow
*
* \attention	Mast be called before start connection. Minimum is 100 millisecond.
*
*/
P2PAPI_API void IOTC_Setup_P2PConnection_Timeout(unsigned int nTimeout);

/**
 * \brief ReInitialize IOTC module socket
 *
 * \details This function will reopen P2P UDP socket of IOTC module.
 *			It is useful for iOS APP from backgroud to foreground will cause socket failed.
 *
 * \param nUDPPort [in] Specify a UDP port. Random UDP port is used if it is specified as 0.
 *
 * \return #IOTC_ER_NoERROR if initializing successfully
 * \return Error code if return value < 0
 *			- #IOTC_ER_FAIL_CREATE_SOCKET Fails to create sockets
 *			- #IOTC_ER_FAIL_SOCKET_OPT Fails to set up socket options
 *			- #IOTC_ER_FAIL_SOCKET_BIND Fails to bind sockets
 *
 *
 * \attention   Must call it before any connection process.
 *
 */
P2PAPI_API int IOTC_ReInitSocket(unsigned short nUDPPort);

/**
 * \brief Setup detect network reachable timeout
 *
 * \details Both device and client can call this, it can determine how many time to detect internet reachable.
 *			Once it called the timeout value is effective forever until IOTC_DeInitialize() be called.
 *
 * \param nTimeout [in] The timeout for this function in unit of millisecond
 *
 * \attention	Mast be called before start login or connection.
 *
 */
P2PAPI_API void IOTC_Setup_DetectNetwork_Timeout(unsigned int nTimeout);

/**
* \brief Used by a device or a client to write data to the other in reliable way
*
* \details A device or a client uses this function to write data through
*			a specific IOTC channel in a IOTC session in a reliable way.
*
* \param nIOTCSessionID [in] The session ID of the IOTC session to write data
* \param cabBuf [in] The array of byte buffer containing the data to write.
*			Its size cannot be larger than #RELIABLE_MAX_PACKET_SIZE
* \param nBufSize [in] The length of the byte buffer. It cannot be larger than
*			#RELIABLE_MAX_PACKET_SIZE
* \param nIOTCChannelID [in] The IOTC channel ID in this IOTC session to write data
* \param nTimeout [in] The timeout for this function in unit of millisecond, give 0 means it'll block until success or error occured.
*
* \return Return 0 if send successfully.
* \return Error code if return value < 0
*			- #IOTC_ER_NOT_INITIALIZED The IOTC module is not initialized yet
*			- #IOTC_ER_INVALID_SID The specified IOTC session ID is not valid
*			- #IOTC_ER_CH_NOT_ON The IOTC channel of specified channel ID is not turned on
*			- #IOTC_ER_SESSION_CLOSE_BY_REMOTE The IOTC session of specified
*				session ID has been closed by remote site
*			- #IOTC_ER_REMOTE_TIMEOUT_DISCONNECT The timeout defined by #IOTC_SESSION_ALIVE_TIMEOUT
*				expires because	remote site has no response
*			- #IOTC_ER_EXCEED_MAX_PACKET_SIZE The packet size is larger than RELIABLE_MAX_PACKET_SIZE
*			- #IOTC_ER_ABORTED Was aborted by IOTC_Session_Write_Reliable_Abort().
*			- #IOTC_ER_BLOCKED_CALL IOTC_Session_Write_Reliable() with same channel and session ID was called in other place.
*			- #IOTC_ER_SESSION_CLOSED The specified session is off or closed.
*			- #IOTC_ER_SERVER_NOT_SUPPORT Server does not support this feature.
*
* \attention (1) The IOTC channel of ID 0 is enabled by default when a IOTC session is established.
*				That means nIOTCChannelID can be specified as 0 if only one IOTC channel
*				is needed by the user. If more IOTC channels are required, users
*				should use IOTC_Session_Get_Free_Channel() and IOTC_Session_Channel_ON()
*				to get more IOTC channel IDs and then specifying those IOTC channel IDs
*				in this function according to the purpose defined by the user. <br><br>
*				(2) This function will block when session is connected via TCP and socket buffer is full.
*				(3) This function will need 4976 bytes stack size.
*
*/
P2PAPI_API int IOTC_Session_Write_Reliable(int nIOTCSessionID, const char *cabBuf, int nBufSize, unsigned char nIOTCChannelID, unsigned int nTimeout);

/**
* \brief Used by a device or a client to abort specific IOTC_Session_Write_Reliable() immediately.
*
* \details A device or a client uses this function to write data through
*			a specific IOTC channel in a IOTC session in a reliable way.
*
* \param nIOTCSessionID [in] The session ID of the IOTC session to write data
* \param nIOTCChannelID [in] The IOTC channel ID in this IOTC session to write data
*
* \return Return 0 if IOTC_Session_Write_Reliable_Abort() call successfully.
* \return Error code if return value < 0
*			- #IOTC_ER_NOT_INITIALIZED The IOTC module is not initialized yet
*			- #IOTC_ER_INVALID_SID The specified IOTC session ID is not valid
*			- #IOTC_ER_CH_NOT_ON The IOTC channel of specified channel ID is not turned on
* \attention The IOTC channel of ID 0 is enabled by default when a IOTC session is established.
*				That means nIOTCChannelID can be specified as 0 if only one IOTC channel
*				is needed by the user. If more IOTC channels are required, users
*				should use IOTC_Session_Get_Free_Channel() and IOTC_Session_Channel_ON()
*				to get more IOTC channel IDs and then specifying those IOTC channel IDs
*				in this function according to the purpose defined by the user. <br><br>
*
**/
P2PAPI_API int IOTC_Session_Write_Reliable_Abort(int nIOTCSessionID, unsigned char nIOTCChannelID);


/**
* \brief Setup session alive timeout value
*
* \details the default alive timeout value is 60 seconds since version 1.14.20.0,
*			this function could help you to change the alive timeout value
*
* \param nTimeout [in] The timeout for this function in unit of second
*
* \attention Must be set on client and device with the same value. Please use the function before
*				IOTC_initialize or IOTC_initialize2. The minimum value is 1.
*
*/
P2PAPI_API void IOTC_Setup_Session_Alive_Timeout(unsigned int nTimeout);


/**
* \brief Setup connect option when client connects to device.
*
* \details Client uses this function to set the Option of Connection.
*
* \param nIOTCSessionID [in] The session ID of the IOTC session to be checked.
* \param S_ConnectOption [in] the connect option that contained the option to be set.
**/
P2PAPI_API int IOTC_Set_Connection_Option(struct st_ConnectOption *S_ConnectOption);

/**
* \brief Switch of Uploading Error code to Server.
*
* \details Use this function to enable or disable error code uploading.
*           The default setup is disable.
*
* \param nEnable [in] Set nEnable to 0 to disable error code uploading.
*           Set nEnable to 1 to enable error code uploading.
**/
P2PAPI_API void IOTC_Setup_ErrorUpload(int bEnable);

#ifdef IOTC_UCOSII
void *malloc(unsigned int size);
void tutk_platform_free(void *addr);
#endif

#ifdef IOTC_ASIX8051
void IOTC_PeriodicRun();  //--call it in main loop
#endif


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* _IOTCAPIs_H_ */
