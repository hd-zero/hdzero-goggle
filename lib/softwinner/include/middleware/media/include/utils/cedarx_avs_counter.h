#ifndef CEDARX_AVS_COUNTER_H
#define CEDARX_AVS_COUNTER_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

//#include <CDX_Types.h>
#include <pthread.h>

typedef enum
{
    AvsCounter_StateInvalid = 0,
    AvsCounter_StateIdle,
    AvsCounter_StateExecuting,
    AvsCounter_StatePause,
}CedarxAvscounterState;

typedef struct CedarxAvscounterContext {
	long long sample_time;  //unit:us, custom system time, when the current time period begins. when adjust_ratio change, sample_time will be set again.
	long long base_time;    //unit:us, cedarx clock time, when the current time period begins. when adjust_ratio change, base_time will be set again.
	int adjust_ratio;   //!vps: +40~-100, +:slow down, -:speed up.
	long long system_base_time; //unit:us, android system time, when init or reset().
	long long mSystemPauseBaseTime; //unit:us, android system time when pause happen.
	long long mSystemPauseDuration; //unit:us, android system time pause duration.

    CedarxAvscounterState mStatus;

	pthread_mutex_t mutex;

	void (*reset)(struct CedarxAvscounterContext *context);
	void (*get_time)(struct CedarxAvscounterContext *context, long long *curr); //get cedarx clock time.
	void (*get_time_diff)(struct CedarxAvscounterContext *context, long long *diff);    //get (cedarx clock time duration) - (custom system clock time duration).
	void (*adjust)(struct CedarxAvscounterContext *context, int val);
    
    int (*start)(struct CedarxAvscounterContext *context);
    int (*pause)(struct CedarxAvscounterContext *context);
    
}CedarxAvscounterContext;

CedarxAvscounterContext *cedarx_avs_counter_request();
int cedarx_avs_counter_release(CedarxAvscounterContext *context);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
