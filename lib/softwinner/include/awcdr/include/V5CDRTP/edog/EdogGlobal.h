
#ifndef _EDOGGLOBAL_H_
#define _EDOGGLOBAL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifdef __cplusplus
    extern   "C"
    {
#endif



//============================================================================
//      Type Definitions
//============================================================================  
#define BOOL   				unsigned char			// 1 Byte
#define UCHAR				unsigned char			// 1 Byte
#define UINT				unsigned int			// 2 Bytes
#define USHORT				unsigned short			// 2 Bytes
#define BYTE   				unsigned char			// 1 Byte
#define WORD				unsigned short			// 2 Bytes
#define DWORD  				unsigned long			// 4 Bytes

typedef unsigned char           BOOLEAN;
typedef unsigned char           INT8U;
typedef signed   char           INT8S;
typedef unsigned short          INT16U;
typedef signed   short          INT16S;
typedef unsigned int            INT32U;
typedef signed   int            INT32S;
typedef float        			FP32;
//typedef	long long		        INT64S;
//typedef	unsigned long long	    INT64U;
typedef double                  FP64;
/***************************************************************************************************/
//MACRO DEFINE
/**************************************************************************************************/

#define PUBLIC
#define LOCAL   static

#define TRUE                    1
#define FALSE                   0
//#define NULL                    0

#define GPS_MAX_CHAN  12

//#define TIME_TONE_ADJUST


/***************************************************************************************************/
//ENUM AND STRUCT TYPE DEFINE
/**************************************************************************************************/

typedef enum error_a 
{
  EDOG_NO_ERROR = 0,
  EDOG_ERROR
} error_e;


typedef	struct dir_f
{
	INT8S east;
	INT8S west;
	INT8S north;
	INT8S south;
}dir_flag;

typedef struct _GPSRADAR_GPSINFO
{
	INT8U      valid;                // 1 = Data valid, 0 = navigation rx warning
	INT8U      hour;                 //
	INT8U      minute;               //
	INT8U      second;               //
	INT8U      day;                   //
	INT8U      month;                 //
	INT16U     year;
	INT16U     speed;               // speed over ground, knots
	INT16U     course;              //
	INT32S     latitude;                  // current latitude  (度数* 10000)  wsk精确到0.1秒
	INT32S     longitude;                 // current longitude 度数* 10000
	INT8U 	   validSatNum;
	INT8U 	   testmode;
	INT8U SatInfo[3*GPS_MAX_CHAN];
	INT8U SatsInSolution[GPS_MAX_CHAN];
	
	dir_flag   direction_flag;
}ST_GPSRADAR_GPSINFO,*PST_GPSRADAR_GPSINFO;



/***************************************************************************************************/
//extern fun
/***************************************************************************************************/
extern void edog_resource_rd_cb_t(INT8U *buffer,INT32U offset,INT32U count);
extern void edog_notify_cb_t(INT8U alarm,INT32U event,INT32U speed,INT16U distance);
extern void edog_location_notify_cb_t(PST_GPSRADAR_GPSINFO gpsinfo);
extern int GPS_ComRead(INT8U *buf, INT32U len);
extern int GPS_ComWrite(INT8U *buf, INT32U len);

/***************************************************************************************************/
//PUBLIC FUN
/***************************************************************************************************/
//EDOG 
PUBLIC INT8U Edog_Init(void);
PUBLIC void  Edog_Run(void);			//200ms once
PUBLIC INT16S GPSRadar_GetTimeZone(void);
PUBLIC INT32U GPSRadar_GetBaseDatabase_Ver(void);
PUBLIC void GPSRadar_EnableTestMode(INT8U mode);


#ifdef   __cplusplus
    }
#endif


#endif

