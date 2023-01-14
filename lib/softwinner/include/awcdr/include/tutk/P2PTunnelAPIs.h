/*! \file P2PTunnelAPIs.h
This file describes all the APIs of the P2PTunnel module in IOTC platform.
P2PTunnel module is a kind of medium module to map local service to remote can access
like port mapping among devices and clients. ex: Web, ssh etc.

* \attention P2PTunnel module can't be used with IOTC and RDT module at the same process.

\copyright Copyright (c) 2012 by Throughtek Co., Ltd. All Rights Reserved.

Revision Table

Version     | Name             |Date           |Description
------------|------------------|---------------|-------------------
0.9.8.0  	|Kevin	  		   |2013-02-25	   |Alpha Version
1.2.0.0     |Harry             |2013-03-07     |+ All new P2PTUNNEL module, NO backward compatible<br> + Support authentication
1.2.1.0  	|Kevin, Harry      |2013-03-28     |+ Tunnel agent add SO_REUSEADDR option except win32.<br> o Modify IOTC_Device_Login() stop retry if login failed.<br> o Use IOTC_Initialize2() instead of IOTC_Initialize().<br> o Check pointer is not NULL before access.<br>
1.2.2.0     |Hanlin, Harry     |2013-04-03     |+ Add P2PTunnelAgent_Set_Log_Path() to set log file name, path and maximum size.<br> + Log error codes in specified file.
1.2.3.0     |Hanlin, Harry     |2013-05-07     |o Fix bugs of buffer handling at low bandwidth condition.<br> o Fix file pointer leak in log function.
1.2.4.0     |Hanlin, Harry     |2013-05-15     |o Fix local socket does not disconnect correctly when remotes side is disconnected.
1.2.5.0		|Han-Lin		   |2013-06-25     |+ Add P2PTunnel_LastIOTime() to get the last IO time of SID.<br> o Fix timer bug which causes incorrect waiting interval.
1.2.6.0     |Harry             |2013-07-26     |o Fix CPU busy issue.<br> o Fix large transmission of the same packages.
1.2.7.0     |Harry, Victor     |2013-08-28     |o Fix TCP socket error code handling to improve P2PTunnel stability.<br> o Fix P2PTunnel control command may not be sent correctly to prevent channel is kept after disconnected.<br> o Fix Big Endian platform will make P2PTunnelAgent_Connect returns -30008 error code.
1.2.7.3     |Han-Lin, Victor   |2013-10-28     |o Fix flow control mechanism.<br> o Fix error detection in Win32.<br> o Fix big endian bug.<br> + P2PTunnel_SetBufSize() API to set tunnel buffer size to improve performance.<br> o Fix buffer control for RTSP/TCP application.
1.2.8.0     |TUTK RD TEAM      |2013-11-19     |o Fix race condition and deadlock bug.<br> o Fix disconnection bug.<br> o Fix variables conflict error.<br> + Support Lan mode environment.
1.2.9.0     |Jelly, Alex       |2014-02-17     |o Fix restart bug.<br> o Improve transmission machanism.<br> o Fix LAN search issue under LAN in P2PTunnelServer_Start().<br> o Fix re-bind error.<br> Fix Server restart error.
1.3.0.0     |TUTK RD TEAM      |2014-08-28     |o Improve TCP relay mode performance.<br> - Remove include platform_config.h
1.4.0.0     |TUTK RD team      |2014-09-22     |o Fix when do duplicate call P2PTunnelAgent_StopPortMapping() will crash.<br> o Improve LAN data transmission performance has greatly increased.<br> + Add 64-bit library.
1.4.2.0     |TUTK RD team      |2014-10-17     |o Fix process crash with LAN mode on Win32.<br> + Add P2PTunnelAgent_Connect_Stop() for exit connection process.
*/

#ifndef _P2PTunnelAPIs_H_
#define _P2PTunnelAPIs_H_


/* ============================================================================
 * Platform Dependant Macro Definition
 * ============================================================================
 */


#ifdef _WIN32
/** @cond */
#ifdef IOTC_STATIC_LIB
#define TUNNEL_API
#elif defined P2PAPI_EXPORTS
#define TUNNEL_API __declspec(dllexport)
#else
#define TUNNEL_API __declspec(dllimport)
#endif // #ifdef P2PAPI_EXPORTS
/** @endcond */
#endif // #ifdef _WIN32


#if defined(__linux__) || defined (__APPLE__)
#define TUNNEL_API
#define __stdcall
#endif // #ifdef __linux__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ============================================================================
 * Generic Macro Definition
 * ============================================================================
 */

/** The maximum number of port mapping service in P2PTunnel module */
#define 	TUNNEL_MAX_PORT_MAPPING_SERVICE_NUMBER 			256

/** The maximum size, in byte, of the authentication data used in P2PTunnelAgent_Connect() */
#define		MAX_AUTH_DATA_LEN		1000

/* ============================================================================
 * Error Code Declaration
 * ============================================================================
 */

/** The function is performed successfully. */
#define		TUNNEL_ER_NoERROR								0

/** P2PTunnel module is not initialized yet. Please use P2PTunnelServerInitialize() or
 * P2PTunnelAgentInitialize() for initialization. */
#define		TUNNEL_ER_NOT_INITIALIZED					-30000

/** The number of P2PTunnel port mapping service has reached maximum.
 * The maximum number of P2PTunnel port mapping service is determined by
 * #TUNNEL_MAX_PORT_MAPPING_SERVICE_NUMBER */
#define		TUNNEL_ER_EXCEED_MAX_SERVICE				-30001

/** Failed to start port mapping when binding because local port had been used by
 * the other service in agent side */
#define		TUNNEL_ER_BIND_LOCAL_SERVICE				-30002

/** Failed to start port mapping when listening because local port has been used
 * by the other service in agent side */
#define		TUNNEL_ER_LISTEN_LOCAL_SERVICE				-30003

/** P2PTunnel module fails to create threads. Please check if OS has ability
 * to create threads for P2PTunnel module. */
#define		TUNNEL_ER_FAIL_CREATE_THREAD				-30004

/** Tunnel agent has already connected to a tunnel server, therefore, it failed
 * connect to another tunnel server */
#define		TUNNEL_ER_ALREADY_CONNECTED					-30005

/** The tunnel between tunnel agent and tunnel server has been disconnected.
 * Used in tunnel status callback function for notifying tunnel connection status */
#define		TUNNEL_ER_DISCONNECTED				  		-30006

/** The P2PTunnel module has been initialized in a tunnel server or a tunnel agent */
#define		TUNNEL_ER_ALREADY_INITIALIZED				-30007

/** Failed to connect to tunnel server because authentication data is wrong. */
#define		TUNNEL_ER_AUTH_FAILED						-30008

/** Authentication data length exceed #MAX_AUTH_DATA_LEN. */
#define		TUNNEL_ER_EXCEED_MAX_LEN					-30009

/** The specified P2PTunnel session ID is not valid */
#define		TUNNEL_ER_INVALID_SID						-30010

/** This UID is illegal. */
#define		TUNNEL_ER_UID_UNLICENSE						-30011

/** The specified device does not support advance function
 *(TCP relay and P2PTunnel module) */
#define		TUNNEL_ER_UID_NO_PERMISSION					-30012

/** This UID can't setup connection through relay. */
#define		TUNNEL_ER_UID_NOT_SUPPORT_RELAY				-30013

/** Tunnel server not login to IOTC server. */
#define		TUNNEL_ER_DEVICE_NOT_ONLINE					-30014

/** Tunner server is not listening for connections. */
#define		TUNNEL_ER_DEVICE_NOT_LISTENING				-30015

/** Internet not available or firewall block. */
#define		TUNNEL_ER_NETWORK_UNREACHABLE				-30016

/** Tunnel agent failed to connect to tunnel server maybe network unstable. */
#define		TUNNEL_ER_FAILED_SETUP_CONNECTION			-30017

/** Tunnel server failed to login to IOTC Server but still can be connected by tunnel agent on LAN. */
#define		TUNNEL_ER_LOGIN_FAILED						-30018

/** Notify tunnel server session connection alreay reached maximum through tunnelStatusCB,
* can't be connected anymore until anyone session release. */
#define		TUNNEL_ER_EXCEED_MAX_SESSION				-30019

/** Tunnel agent can't call P2PTunnelServer_GetSessionInfo() */
#define		TUNNEL_ER_AGENT_NOT_SUPPORT					-30020

/** The arguments passed to a function is invalid. */
#define		TUNNEL_ER_INVALID_ARG                       -30021

/** OS resource not enough to malloc memory or open socket. */
#define		TUNNEL_ER_OS_RESOURCE_LACK                  -30022

/** Tunnel Agent isn't connecting. */
#define		TUNNEL_ER_AGENT_NOT_CONNECTING              -30023

/** Unable to get free session.*/
#define         TUNNEL_ER_NO_FREE_SESSION               -30024

/** Connection cancelled*/
#define         TUNNEL_ER_CONNECTION_CANCELLED          -30025

/* ============================================================================
 * Structure Definition
 * ============================================================================
 */

/**
 * \details P2PTunnel session info, containing all the information when a P2PTunnel session is
 *  established between a tunnel server and a tunnel agent. Users can get this information
 *  through use P2PTunnelServer_GetSessionInfo() to register a call back function tunnelSessionInfoCB
 */
typedef struct st_P2PTunnelSessionInfo
{
	int nSID; //!< P2PTunnel session index
	unsigned char nMode; //!< 0: P2P mode, 1: Relay mode, 2: LAN mode
	unsigned char nNatType; //!< The remote NAT type
	char szRemoteIP[16]; //!< The IP address of remote site used during this P2PTunnel session
	unsigned short nRemotePort; //!< The port number of remote site used during this P2PTunnel session
	unsigned int nVersion; //!< The remote P2PTunnel API version.
	int nAuthDataLen; //!< How many byte of the authentication data.
	void *pAuthData; //!< The authentication data buffer, it will be freed by API automatically.
} sP2PTunnelSessionInfo;


/* ============================================================================
 * Type Definition
 * ============================================================================
 */

/**
 * \details The prototype of getting tunnel status function, used by a tunnel server
 *			or tunnel agent to be notified if tunnel status is changed.
 *
 * \param nErrorCode [out]
 *			- #TUNNEL_ER_DISCONNECTED The tunnel between tunnel agent and tunnel server
 *				has been disconnected.
 * \param nSessionID [in] Which session ID of the P2PTunnel session disconnected
 * \param pArg [out] Tunnel server or agent pass user data
 *
 */
typedef void (__stdcall *tunnelStatusCB)(int nErrorCode, int nSessionID, void *pArg);


/**
 * \details The prototype of getting tunnel session information function,
 * 			used by a tunnel server to be notified after tunnel session is established.
 *
 * \param sSessionInfo [out] About remote session information
 * \param pArg [out] Tunnel server pass user data
 *
 *
 * \return User define value.
 *			- if value == 0, it means a tunnel server passes tunnel agent authentication
 *			- if value < 0, it means a tunnel server rejects tunnel agent authentication
 *				and P2PTunnel module will send the value back to tunnel agent
 *				for tell the reason why this tunnel server rejects connection.
 *
 */
typedef int (__stdcall *tunnelSessionInfoCB)(sP2PTunnelSessionInfo *sSessionInfo, void *pArg);


/* ============================================================================
 * Function Declaration
 * ============================================================================
 */

/**
 * \brief Get the version of P2PTunnel module
 *
 * \details This function returns the version of P2PTunnel module.
 *
 * \return The version of P2PTunnel module from high byte to low byte, for example,
 *			0x01020304 means the version is 1.2.3.4
 */
TUNNEL_API int P2PTunnel_Version(void);


/**
 * \brief Initialize P2PTunnel module in a tunnel server
 *
 * \details This function is used by a tunnel server to initialize P2PTunnel
 *			module and shall be called before any P2PTunnel module related
 *			function is invoked.
 *
 * \param nMaxAgentConnection [in] Specify max number of tunnel agents that
 *				can connect to this tunnel server
 *
 * \return #TUNNEL_ER_NoERROR if initializing successfully
 * \return Error code if return value < 0
 *			- #TUNNEL_ER_ALREADY_INITIALIZED P2PTunnel module is already initialized
 *			- #IOTC_ER_FAIL_RESOLVE_HOSTNAME Cannot resolve masters' host name
 *			- #IOTC_ER_ALREADY_INITIALIZED IOTC module is already initialized
 *			- #IOTC_ER_MASTER_TOO_FEW Two masters are required as parameters at minimum
 *			- #IOTC_ER_FAIL_CREATE_MUTEX Fails to create Mutexs
 *			- #IOTC_ER_FAIL_CREATE_THREAD Fails to create threads
 *			- #RDT_ER_ALREADY_INITIALIZED RDT module is already initialized
 *			- #RDT_ER_FAIL_CREATE_MUTEX Fails to create Mutexs
 *
 * \see P2PTunnelServerDeInitialize()
 */
TUNNEL_API int P2PTunnelServerInitialize(unsigned int nMaxAgentConnection);


/**
 * \brief Deinitialize P2PTunnel module in a tunnel server
 *
 * \details This function will deinitialize P2PTunnel module in a tunnel server
 *
 * \return #TUNNEL_ER_NoERROR if deinitialize successfully
 * \return Error code if return value < 0
 *			- #TUNNEL_ER_NOT_INITIALIZED the P2PTunnel module is not initialized yet.
 *
 * \see P2PTunnelServerInitialize()
 *
 * \attention Must call P2PTunnelServer_Stop() before calling this function,
 *	 		otherwise resource in P2PTunnel module will not be freed.
 */
TUNNEL_API int P2PTunnelServerDeInitialize(void);


/**
 * \brief Start a tunnel server
 *
 * \details This function will start a tunnel server with given UID
 *
 * \param cszUID [in] The UID of that tunnel server
 *
 * \return #TUNNEL_ER_NoERROR if start successfully
 * \return Error code if return value < 0
 *			- #TUNNEL_ER_NOT_INITIALIZED P2PModule has not been initialized in that tunnel server
 *			- #TUNNEL_ER_FAIL_CREATE_THREAD Failed to create thread
 *			- #TUNNEL_ER_UID_NO_PERMISSION This UID not support P2PModule and TCP relay function
 *			- #TUNNEL_ER_UID_UNLICENSE This UID is not licensed or expired
 */
TUNNEL_API int P2PTunnelServer_Start(const char *cszUID);


/**
 * \brief Stop a tunnel server
 *
 * \details This function will stop a tunnel server
 *
 */
TUNNEL_API void P2PTunnelServer_Stop(void);


/**
 * \brief Used by a tunnel server to get the tunnel status
 *
 * \details This function will set a callback function pointer in P2PTunnel
 *			module for a tunnel server to get the tunnel status from given callback
 *			function. Whenever tunnel status changes, P2PTunnel module will invoke
 *			the given callback function to notify tunnel server with corresponding
 *			status.
 *
 * \param pfxTunnelStatusFn [in] The function pointer to get tunnel status function
 * \param pArg [in] User can give data pointer to pass to pfxSessionInfoFn when this
 *			call back function is triggered. Can be NULL.
 */
TUNNEL_API void P2PTunnelServer_GetStatus(tunnelStatusCB pfxTunnelStatusFn, void *pArg);


/**
 * \brief Used by a tunnel server to get the tunnel session information
 *
 * \details This function will set a callback function pointer in P2PTunnel
 *			module for a tunnel server to get the tunnel session information from
 *			given callback function. Whenever tunnel agent try to establish session with tunnel server
 *			successfully, P2PTunnel module will invoke the given callback function to notify
 *			tunnel server and pass information through sP2PTunnelSessionInfo data struct.
 *
 * \param pfxSessionInfoFn [in] The function pointer to get tunnel session information function
 * \param pArg [in] User can give data pointer to pass to pfxSessionInfoFn when this
 *			call back function is triggered. Can be NULL.
 *
 * \return #TUNNEL_ER_NoERROR if register successfully
 * \return Error code if return value < 0
 *			- #TUNNEL_ER_AGENT_NOT_SUPPORT Tunnel agent not support this function call
 *
 * \attention Must call P2PTunnelServer_Start() before calling this function,
 *				otherwise it will return TUNNEL_ER_AGENT_NOT_SUPPORT.
 */
TUNNEL_API int P2PTunnelServer_GetSessionInfo(tunnelSessionInfoCB pfxSessionInfoFn, void *pArg);


/**
 * \brief Used by a tunnel server to disconnect a tunnel session
 *
 * \details This function will disconnect a tunnel session anytime after tunnel session
 *			established and it call be called anywhere.
 *
 * \param nSessionID [in] Which session ID of the P2PTunnel session established
 *
 * \return #TUNNEL_ER_NoERROR if disconnect P2PTunnel session successfully
 * \return Error code if return value < 0
 *			- #TUNNEL_ER_AGENT_NOT_SUPPORT Tunnel agent not support this function call
 *			- #TUNNEL_ER_INVALID_SID The session ID already disconnected or not yet established
 *			- #TUNNEL_ER_INVALID_ARG The session ID is not in valid range.
 */
TUNNEL_API int P2PTunnelServer_Disconnect(int nSessionID);


/**
 * \brief Initialize P2PTunnel module in a tunnel agent
 *
 * \details This function is used by a tunnel agent to initialize P2PTunnel
 *			module and shall be called before any P2PTunnel module related
 *			function is invoked.
 *
 * \param nMaxConnection [in] Specify max number of tunnel agents that
 *				can connect to tunnel servers
 *
 * \return #TUNNEL_ER_NoERROR if initializing successfully
 * \return Error code if return value < 0
 *			- #TUNNEL_ER_ALREADY_INITIALIZED P2PTunnel module is already initialized
 *			- #TUNNEL_ER_FAIL_CREATE_THREAD Failed to create thread
 *			- #IOTC_ER_FAIL_RESOLVE_HOSTNAME Cannot resolve masters' host name
 *			- #IOTC_ER_ALREADY_INITIALIZED IOTC module is already initialized
 *			- #IOTC_ER_MASTER_TOO_FEW Two masters are required as parameters at minimum
 *			- #IOTC_ER_FAIL_CREATE_MUTEX Fails to create Mutexs
 *			- #IOTC_ER_FAIL_CREATE_THREAD Fails to create threads
 *			- #RDT_ER_ALREADY_INITIALIZED RDT module is already initialized
 *			- #RDT_ER_FAIL_CREATE_MUTEX Fails to create Mutexs
 *
 * \see P2PTunnelAgentDeInitialize()
 */
TUNNEL_API int P2PTunnelAgentInitialize(unsigned int nMaxConnection);


/**
 * \brief Deinitialize P2PTunnel module in a tunnel agent
 *
 * \details This function will deinitialize P2PTunnel module in a tunnel agent
 *
 * \return #TUNNEL_ER_NoERROR if deinitialize successfully
 * \return Error code if return value < 0
 *			- #TUNNEL_ER_NOT_INITIALIZED the P2PTunnel module is not initialized yet.
 *
 * \see P2PTunnelAgentInitialize()
 *
 * \attention Must call P2PTunnelAgent_Disconnect() before calling this function,
 *				otherwise resource in P2PTunnel module will not be freed and connection
 *				will not be closed (the remote site will have connection timeout event
 *				eventually).
 */
TUNNEL_API int P2PTunnelAgentDeInitialize(void);

/**
 * \brief Used by an agent to get a free session ID.
 *
 * \datails This function is for an agent to get a free session ID used for a parameter of P2PTunnelAgent_Connect_Parallel().
 *
 * \return Tunnel Session ID if return value >= 0
 * \return Error code if return value < 0
 *                      - #TUNNEL_ER_NO_FREE_SESSION Current tunnel session are full.
 *
 */
TUNNEL_API int P2PTunnelAgent_GetFreeSession(void);

/**
 * \brief Connect to a tunnel server
 *
 * \details This function used by a tunnel agent to connect the tunnel server
 *			with specified UID
 *
 * \param cszUID [in] The UID of that tunnel server to be connected
 * \param cpAuthData [in] Send authentication data to tunnel server for connection.
 * \param nAuthDataLength [in] The length of the byte buffer. It cannot be larger than
 *			#MAX_AUTH_DATA_LEN
 * \param pnErrFromDeviceCB [out] The reason why tunnel server rejects this connection.
 *			It's returned from tunnelSessionInfoCB of the tunnel server.
 *
  * \return Tunnel Session ID if return value >= 0
 * \return #TUNNEL_ER_NoERROR if connect successfully
 * \return Error code if return value < 0
 *			- #TUNNEL_ER_NOT_INITIALIZED P2PModule has not been initialized in that tunnel agent
 *			- #TUNNEL_ER_AUTH_FAILED The tunnel agent failed to connect to tunnel server
 *					because authentication data is illegal.
 *			- #TUNNEL_ER_UID_UNLICENSE This UID is illegal or does not support P2PTunnel function
 *			- #TUNNEL_ER_UID_NO_PERMISSION This UID not support P2PModule and TCP relay function
 *			- #TUNNEL_ER_UID_NOT_SUPPORT_RELAY This UID can't setup connection through relay
 *			- #TUNNEL_ER_DEVICE_NOT_ONLINE The specified tunnel server does not login to IOTC server yet
 *			- #TUNNEL_ER_DEVICE_NOT_LISTENING The specified tunnel server is not listening for connection,
 *					it maybe busy at establishing connection with other tunnel agent
 *			- #TUNNEL_ER_NETWORK_UNREACHABLE Internet is not available or firewall blocks connection
 *			- #TUNNEL_ER_FAILED_SETUP_CONNECTION Can't connect to the tunnel server although it is online
 *					and listening for connection, it maybe caused by internet unstable situation
 *
 */
TUNNEL_API int P2PTunnelAgent_Connect(const char *cszUID, const void *cpAuthData, int nAuthDataLength, int *pnErrFromDeviceCB);

/**
 * \brief Connect to a tunnel server and bind to a specified session ID.
 *
 * \details This function is for a tunnel agent to connect a device by specifying the UID
 *                      of that device, and bind to a free session ID from P2PTunnelAgent_GetFreeSession().
 *                      If connection is established with the help of IOTC servers, the TUNNEL_ER_NoERROR will
 *                      be returned in this function and then device and client can communicate for the other
 *                      later by using this tunnel session ID. If this function is called by multiple threads,
 *                      the connections will be processed concurrently.
 *
 * \param cszUID [in] The UID of that tunnel server to be connected
 * \param SID [in] The SID got from P2PTunnelAgent_GetFreeSession() the connection should bind to.
 * \param cpAuthData [in] Send authentication data to tunnel server for connection.
 * \param nAuthDataLength [in] The length of the byte buffer. It cannot be larger than
 *                      #MAX_AUTH_DATA_LEN
 * \param pnErrFromDeviceCB [out] The reason why tunnel server rejects this connection.
 *                      It's returned from tunnelSessionInfoCB of the tunnel server.
 *
 * \return Tunnel session ID if return value >= 0 and equal to the input parameter SID.
 * \return #TUNNEL_ER_NoERROR if connect successfully
 * \return Error code if return value < 0
 *                      - #TUNNEL_ER_NOT_INITIALIZED P2PModule has not been initialized in that tunnel agent
 *                      - #TUNNEL_ER_AUTH_FAILED The tunnel agent failed to connect to tunnel server
 *                                      because authentication data is illegal.
 *                      - #TUNNEL_ER_UID_UNLICENSE This UID is illegal or does not support P2PTunnel function
 *                      - #TUNNEL_ER_UID_NO_PERMISSION This UID not support P2PModule and TCP relay function
 *                      - #TUNNEL_ER_UID_NOT_SUPPORT_RELAY This UID can't setup connection through relay
 *                      - #TUNNEL_ER_DEVICE_NOT_ONLINE The specified tunnel server does not login to IOTC server yet
 *                      - #TUNNEL_ER_DEVICE_NOT_LISTENING The specified tunnel server is not listening for connection,
 *                                      it maybe busy at establishing connection with other tunnel agent
 *                      - #TUNNEL_ER_NETWORK_UNREACHABLE Internet is not available or firewall blocks connection
 *                      - #TUNNEL_ER_FAILED_SETUP_CONNECTION Can't connect to the tunnel server although it is online
 *                                      and listening for connection, it maybe caused by internet unstable situation
 *
 */
TUNNEL_API int P2PTunnelAgent_Connect_Parallel(const char *cszUID, const int SID, const void *cpAuthData, int nAuthDataLength, int *pnErrFromDeviceCB);

/**
 * \brief Disconnect to a tunnel server
 *
 * \details This function used by a tunnel agent to disconnect to a tunnel server
 *
 * \param nSessionID [in] Which session ID of the P2PTunnel session to be disconnect
 *
 * \return - #TUNNEL_ER_NoERROR if connect successfully
 *			- #TUNNEL_ER_INVALID_ARG The session ID is not in valid range.
 */
TUNNEL_API int P2PTunnelAgent_Disconnect(int nSessionID);


/**
 * \brief Start port mapping service
 *
 * \details This function used by a tunnel agent to start port mapping service
 *			provided by P2PTunnel module. The tunnel agent specifies the local port
 *			in local host through which a tunnel is created with the remote port defined
 *			in the tunnel server.
 *
 * \param nSessionID [in] Which session ID of the P2PTunnel session to process the port mapping service
 * \param nLocalPort [in] The local port used to create a tunnel with the tunnel server
 * \param nRemotePort [in] The remote port defined in the tunnel server to create a tunnel
 *
 * \return The port mapping index if return value >= 0
 * \return Error code if return value < 0
 *			- #TUNNEL_ER_EXCEED_MAX_SERVICE Exceed the max number of port mapping service
 *			- #TUNNEL_ER_BIND_LOCAL_SERVICE Failed to bind local port since it is used by the other services
 *			- #TUNNEL_ER_LISTEN_LOCAL_SERVICE Failed to listen local port since it is used by the other services
 *			- #TUNNEL_ER_OS_RESOURCE_LACK Failed to malloc memory
 *			- #TUNNEL_ER_INVALID_ARG The session ID is not in valid range.
 */
TUNNEL_API int P2PTunnelAgent_PortMapping(int nSessionID, unsigned short nLocalPort, unsigned short nRemotePort);


/**
 * \brief Stop port mapping service
 *
 * \details This function used by a tunnel agent to stop port mapping service
 *			on a given port mapping index which is started by P2PTunnelAgent_PortMapping()
 *
 * \param nPortMappingIdx [in] The port mapping index started in P2PTunnelAgent_PortMapping()
 *
 * \attention If not call P2PTunnelAgent_StopPortMapping local port can't be used until OS release it.
 */
TUNNEL_API void P2PTunnelAgent_StopPortMapping(unsigned int nPortMappingIdx);


/**
 * \brief Stop port mapping service
 *
 * \details This function used by a tunnel agent to stop multiple port mapping service
 *			on an integer array set of given port mapping index which is started by P2PTunnelAgent_PortMapping()
 *
 * \param indexArray [in] An integer array of the port mapping index in P2PTunnelAgent_PortMapping()
 *        indexNum is the number of the port mapping index in indexArray
 *
 * \return Error code if return value < 0
 *          - #TUNNEL_ER_NoERROR if connect successfully
 *	        - #TUNNEL_ER_INVALID_ARG The session ID is not in valid range.
 *
 * \attention If not call P2PTunnelAgent_StopPortMapping local port can't be used until OS release it.
 */
TUNNEL_API int P2PTunnelAgent_StopPortMapping_byIndexArray(unsigned int *indexArray, unsigned int indexNum);


/**
 * \brief Used by a tunnel agent to get the tunnel status
 *
 * \details This function will set a callback function pointer in P2PTunnel
 *			module for a tunnel agent to get the tunnel status from given callback
 *			function. Whenever tunnel status changes, P2PTunnel module will invoke
 *			the given callback function to notify tunnel agent with corresponding
 *			status.
 *
 * \param pfxTunnelStatusFn [in] The function pointer to getting tunnel status function
 * \param pArg [in] User can give data pointer to pass to pfxSessionInfoFn when this
 *			call back function is triggered. Can be NULL.
 */
TUNNEL_API void P2PTunnelAgent_GetStatus(tunnelStatusCB pfxTunnelStatusFn, void *pArg);

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

TUNNEL_API void P2PTunnelAgent_Set_Log_Path(const char *path, int nMaxSize);

/**
 * \brief Get last IO time of SID
 *
 * \details Get last IO time of SID even if the connection is closed.
 *
 * \param nSessionID [in] Session ID
 *
 * \return POSIX time of last IO if return value > 0
 * \return 0 if no connection ever established on this SID
 * \return Error code if return value < 0
 *			- #TUNNEL_ER_INVALID_SID The session ID already disconnected or not yet established
 *			- #TUNNEL_ER_INVALID_ARG The session ID is not in valid range.
 *
 */
TUNNEL_API int P2PTunnel_LastIOTime(int nSessionID);

/**
 * \brief Set P2PTunnel buffer size
 *
 * \details Set P2PTunnel buffer size, larger buffer leads more maximum throughput
 *
 * \param nSessionID [in] Session ID
 * \param nSize [in] Buffer size in Bytes
 *
 * \return 0 if success
 * \return Error code if return value < 0
 *			- #TUNNEL_ER_INVALID_SID The session ID already disconnected or not yet established
 *			- #TUNNEL_ER_INVALID_ARG The session ID is not in valid range.
 *
 */
TUNNEL_API int P2PTunnel_SetBufSize(int nSessionID, int nSize);

/**
 * \brief It's to stop the progressing of connection.
 *
 * \details This API is for a client to stop connecting to a device.
 * 			We can use it to stop connecting when client blocks in P2PTunnelAgent_Connect().
 *
 * \param cszUID [in] The UID of that tunnel server
 *
 * \return 0 if success
 * \return Error code if return value < 0
 *			- #TUNNEL_ER_AGENT_NOT_CONNECTING Tunnel Agent isn't connecting.
 *
 */
TUNNEL_API int P2PTunnelAgent_Connect_Stop (const char* cszUID);


/**
 * \brief It's to stop the progressing of connection.
 *
 * \details This API is for a client to stop connecting to a device.
 *                      We can use it to stop connecting when client blocks in P2PTunnelAgent_Connect_Parallel().
 *
 * \param cszUID [in] The UID of that tunnel server
 * \param nSessionID [in] Session ID
 * \return 0 if success
 * \return Error code if return value < 0
 *                      - #TUNNEL_ER_AGENT_NOT_CONNECTING Tunnel Agent isn't connecting.
 *
 */
TUNNEL_API int P2PTunnelAgent_Connect_Stop_Parallel(const char* cszUID, const int nSessionID);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* _P2PTunnelAPIs_H_ */
