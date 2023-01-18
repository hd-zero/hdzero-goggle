/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : plat_errors.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/03/15
  Last Modified :
  Description   : define the format of error code, used in platform scope.
  Function List :
  History       :
******************************************************************************/
#ifndef __IPCLINUX_PLAT_ERRNO_H__
#define __IPCLINUX_PLAT_ERRNO_H__

#ifdef __cplusplus
extern "C"{
#endif /* End of #ifdef __cplusplus */


/* 1010 0000b
 * use APPID based on 32
 */
#define ERR_APPID  (0x80000000L + 0x20000000L)

typedef enum ERR_LEVEL_E
{
    EN_ERR_LEVEL_DEBUG = 0,  /* debug-level                                  */
    EN_ERR_LEVEL_INFO,       /* informational                                */
    EN_ERR_LEVEL_NOTICE,     /* normal but significant condition             */
    EN_ERR_LEVEL_WARNING,    /* warning conditions                           */
    EN_ERR_LEVEL_ERROR,      /* error conditions                             */
    EN_ERR_LEVEL_CRIT,       /* critical conditions                          */
    EN_ERR_LEVEL_ALERT,      /* action must be taken immediately             */
    EN_ERR_LEVEL_FATAL,      /* just for compatibility with previous version */
    EN_ERR_LEVEL_BUTT
}ERR_LEVEL_E;


/******************************************************************************
|----------------------------------------------------------------|
| 1 |   APP_ID   |   MOD_ID    | ERR_LEVEL |   ERR_ID            |
|----------------------------------------------------------------|
|<--><--7bits----><----8bits---><--3bits---><------13bits------->|
******************************************************************************/

#define DEF_ERR( module, level, errid) \
    ((int)( (ERR_APPID) | ((module) << 16 ) | ((level)<<13) | (errid) ))

/* NOTE! the following defined all common error code,
** all module must reserved 0~63 for their common error code
*/
typedef enum EN_ERR_CODE_E
{
    EN_ERR_INVALID_DEVID = 1, /* invlalid device ID                           */
    EN_ERR_INVALID_CHNID = 2, /* invlalid channel ID                          */
    EN_ERR_ILLEGAL_PARAM = 3, /* at lease one parameter is illagal
                               * eg, an illegal enumeration value             */
    EN_ERR_EXIST         = 4, /* resource exists                              */
    EN_ERR_UNEXIST       = 5, /* resource unexists                            */
    
    EN_ERR_NULL_PTR      = 6, /* using a NULL point                           */
    
    EN_ERR_NOT_CONFIG    = 7, /* try to enable or initialize system, device
                              ** or channel, before configing attribute       */

    EN_ERR_NOT_SUPPORT   = 8, /* operation or type is not supported by NOW    */
    EN_ERR_NOT_PERM      = 9, /* operation is not permitted
                              ** eg, try to change static attribute           */

    EN_ERR_NOMEM         = 12,/* failure caused by malloc memory              */
    EN_ERR_NOBUF         = 13,/* failure caused by malloc buffer              */

    EN_ERR_BUF_EMPTY     = 14,/* no data in buffer                            */
    EN_ERR_BUF_FULL      = 15,/* no buffer for new data                       */

    EN_ERR_SYS_NOTREADY  = 16,/* System is not ready,maybe not initialed or 
                              ** loaded. Returning the error code when opening
                              ** a device file failed.                        */

    EN_ERR_BADADDR       = 17,/* bad address, 
                              ** eg. used for copy_from_user & copy_to_user   */

    EN_ERR_BUSY          = 18,/* resource is busy, 
                              ** eg. destroy a venc chn without unregister it */
    EN_ERR_TIMEOUT       = 19, /* timeout */
    EN_ERR_SAMESTATE     = 20,/* component state is same as use wanted, no need to tranform */
    EN_ERR_INVALIDSTATE  = 21,/* component state is transit to invalid state */
    EN_ERR_INCORRECT_STATE_TRANSITION  = 22,/* component current state can't transit to destination state */
    EN_ERR_INCORRECT_STATE_OPERATION = 23, /* Attempting a command that is not allowed during the present state. */

    EN_ERR_FILE_EXCEPTION= 24, /* media file exception, size=0, etc.          */
    EN_ERR_EFUSE_ERROR = 25,    /* efuse check fail */

    EN_ERR_BUTT          = 63,/* maxium code, private error code of all modules
                              ** must be greater than it                      */
}EN_ERR_CODE_E;

#define MOD_ID_COMMON (0xFF)

#define ERR_COMMON_UNEXIST      DEF_ERR(MOD_ID_COMMON, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
#define ERR_COMMON_SYS_NOTREADY DEF_ERR(MOD_ID_COMMON, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)

/* 
** following is an example for defining error code of VDA module
** #define HI_ERR_MD_INVALID_CHNID HI_DEF_ERR(HI_ID_VDA, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
**
*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* __IPCLINUX_PLAT_ERRNO_H__ */

