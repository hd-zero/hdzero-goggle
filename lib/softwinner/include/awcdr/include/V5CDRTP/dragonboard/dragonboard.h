#ifndef _DragonBoard_H_
#define _DragonBoard_H_

#include <linux/rtc.h>
#include <cutils/log.h>

struct fifo_param {
    char type;                  // A: autotester, H: handtester
    char name[10];              // tester-name
    char result;                // F: Fail, P: Pass, W: waiting
    union {
        float acc[3];           // acc[0-2]: x,y,z
        struct rtc_time rtc;        // rtc[0-5]: Y:M:D:H:M:S
        int MemTotal;           // DRAM capacity
    }val;
};
typedef struct fifo_param fifo_param_t;

/* define device which refer to fifo_param.type/name/result */
//#define autotester 'A'
//#define handtester 'H'
#define ACC "acc"   // do NOT over 9 chars!
#define RTC "rtc"
//#define MIC  "mic"    // if tester contains data, such as acc and rtc, we use fifo_param_t var
//#define SPK  "spk"    // to tansmit data, but these MIC, SPK... do not contains device-data
//#define TF   "tf" // we directly transmit string through fifo
//#define NOR  "nor"
//#define DDR  "ddr"
//#define WIFI "wifi"

#endif /* _DragonBoard_H_ */


#if 0 
#ifndef DB_LOG_LEVEL
#define DB_LOG_LEVEL 4          // TODO: set it to debug your code
#endif

#if DB_LOG_LEVEL == 0
#define DB_ERROR
#elif DB_LOG_LEVEL == 1
#define DE_ERROR
#define DB_WARN
#elif DB_LOG_LEVEL == 2
#define DB_ERROR
#define DB_WARN
#define DB_MSG
#elif DB_LOG_LEVEL == 3
#define DB_ERROR
#define DB_WARN
#define DB_MSG
#define DB_DEBUG
#elif DB_LOG_LEVEL == 4
#define DB_ERROR
#define DB_WARN
#define DB_MSG
#define DB_DEBUG
#define DB_VERB
#endif  /* DB_LOG_LEVEL == 0 */
#endif

#ifndef TAG             // TODO: define TAG before dragonborad.h if U use it
#define TAG __FILE__
#endif

#if 0
#ifdef DB_ERROR             // level 0
#define db_error(fmt, ...)  \
    do { fprintf(stdout, "[DragonBoard](error): (%s)", TAG); fprintf(stdout, fmt, ##__VA_ARGS__);} while(0)
#else
#define db_error(fmt, ...)
#endif

#ifdef DB_WARN              // level 1
#define db_warn(fmt, ...)   \
    do { fprintf(stdout, "[DragonBoard](warn): (%s)", TAG); fprintf(stdout, fmt, ##__VA_ARGS__);} while(0)
#else
#define db_warn(fmt, ...)
#endif

#ifdef DB_MSG               // level 2
#define db_msg(fmt, ...)    \
    do { fprintf(stdout, "[DragonBoard](msg): (%s)", TAG); fprintf(stdout, fmt, ##__VA_ARGS__);} while(0)
#else
#define db_msg(fmt, ...)
#endif

#ifdef DB_DEBUG             // level 3
#define db_debug(fmt, ...)  \
    do { fprintf(stdout, "[DragonBoard](debug): (%s)", TAG); fprintf(stdout, fmt, ##__VA_ARGS__);} while(0)
#else
#define db_debug(fmt, ...)
#endif

#ifdef DB_VERB              // level 4
#define db_verb(fmt, ...)   \
    do { fprintf(stdout, "[DragonBoard](verb): (%s)", TAG); fprintf(stdout, fmt, ##__VA_ARGS__);} while(0)
#else
#define db_verb(fmt, ...)
#endif

#else

#ifdef DB_ERROR
#define db_error(...) ALOGE("[DragonBoard]("TAG")"__VA_ARGS__)
#else
#define db_error(...)
#endif

#ifdef DB_WARN
#define db_warn(...) ALOGW("[DragonBoard]("TAG")"__VA_ARGS__)
#else
#define db_warn(...)
#endif

#ifdef DB_MSG
#define db_msg(...) ALOGI("[DragonBoard]("TAG")"__VA_ARGS__)
#else
#define db_msg(...)
#endif


#ifdef DB_DEBUG
#define db_debug(...) ALOGD("[DragonBoard]("TAG")"__VA_ARGS__)
#else
#define db_degbug(...)
#endif

#ifdef DB_VERB
#define db_verb(...) ALOGV("[DragonBoard]("TAG")"__VA_ARGS__)
#else
#define db_verb(...)
#endif

#endif
