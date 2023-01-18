#ifndef __COMMON_DEFINE_H__
#define __COMMON_DEFINE_H__

/*
 * enable global struct __GLOBAL_INFO__ to save dev info
 */
//#define ENABLE_GLOBAL_VAR


/*
 * enable flash part read/write log record in part.log
 */
//#define DEBUG_PART

/*
 * enable 4G debug info 
 */
//#define DEBUG_4G

/*
 * enable lcd show debug info 
 */
//#define LCD_SHOW_DEBUG

/*
 * enable OTA debug info 
 */
#define DEBUG_OTG

/*
 * enable sqlit Debug info
 */

#define DEBUG_DATABASE

/*
 * enable Minigui Messge Debug info 
 */

//#define DEBUG_MINI_GUI_MESSAGE

/*
*  enable create log file
*/

#define DEBUG_LOG_CREATE


/*
* enable save adas log
*/
//#define SAVE_ADAS_LOG

/***************************************************/
/***************************************************/

/*
 * flash part read/write log record queue max size
 */
#define LOG_RECORD_MAX      (30)

/*
 * flash key define
 */
#define FKEY_BINDFLAG    "BindFlag"
#define FKEY_UPGRADEINFO "UpgradeInfo"
#define FKEY_MAX         "Max"

/*
 * define update type, net/tfcard...
 */
#define TYPE_UPDATE_NET     (0)
#define TYPE_UPDATE_CARD    (1)

#endif
