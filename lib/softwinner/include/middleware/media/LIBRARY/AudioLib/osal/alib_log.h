#ifndef ALIB_LOG_H
#define ALIB_LOG_H

/*
* Register Ur Platform
* __OS_LINUX
* __OS_ANDROID
* __OS_DS5
* __OS_UCOS
* __OS_WIN
* Coming here...
*/

#define VERSION "ver : 2017-8-4-18-00"
#define LOG_TAG "AllwinnerAlibs"


#define ALIB_DEBUG 0

#ifdef __OS_ANDROID//Use android sdk
    #include <utils/Log.h>
    #ifdef  LOG_NDEBUG
    #undef  LOG_NDEBUG
    #define LOG_NDEBUG 0
    #endif
    #define alib_loge(fmt, arg...) ALOGE("%s "fmt"" ,VERSION, ##arg)
    #define alib_logd(fmt, arg...) if(ALIB_DEBUG > 0) ALOGD("%s "fmt"" ,VERSION, ##arg)
    #define alib_logw(fmt, arg...) if(ALIB_DEBUG > 1) ALOGD("%s "fmt"" ,VERSION, ##arg)
    #define alib_logv(fmt, arg...) if(ALIB_DEBUG > 2) ALOGD("%s "fmt"" ,VERSION, ##arg)
#elif (defined __OS_LINUX) || (defined __OS_DS5) || (defined __OS_WIN)

    #include <stdlib.h>
    #include <stdio.h>
    #include <string.h>

    #ifndef __ANDROID__ 
        #define alib_loge(fmt, arg...) printf("(%s),line(%d) : "fmt"\n", LOG_TAG, __LINE__, ##arg)
        #define alib_logd(fmt, arg...) if(ALIB_DEBUG > 0) printf("(%s),line(%d) : "fmt"\n", LOG_TAG, __LINE__, ##arg)
        #define alib_logw(fmt, arg...) if(ALIB_DEBUG > 1) printf("(%s),line(%d) : "fmt"\n", LOG_TAG, __LINE__, ##arg)
        #define alib_logv(fmt, arg...) if(ALIB_DEBUG > 2) printf("(%s),line(%d) : "fmt"\n", LOG_TAG, __LINE__, ##arg)

    #else// Use android ndk...
        #include <android/log.h>
        #define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
        #define ALOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

        #define alib_loge(fmt, arg...) ALOGE("%s "fmt"" ,VERSION, ##arg)
        #define alib_logd(fmt, arg...) if(ALIB_DEBUG > 0) ALOGD("%s "fmt"" ,VERSION, ##arg)
        #define alib_logw(fmt, arg...) if(ALIB_DEBUG > 1) ALOGD("%s "fmt"" ,VERSION, ##arg)
        #define alib_logv(fmt, arg...) if(ALIB_DEBUG > 2) ALOGD("%s "fmt"" ,VERSION, ##arg)
    #endif

#elif (defined __OS_UCOS)
    #define alib_loge(fmt, arg...) eLIBs_printf("(%s),line(%d) : "fmt"\n", LOG_TAG, __LINE__, ##arg)
    #define alib_logd(fmt, arg...) if(ALIB_DEBUG > 0) eLIBs_printf("(%s),line(%d) : "fmt"\n", LOG_TAG, __LINE__, ##arg)
    #define alib_logw(fmt, arg...) if(ALIB_DEBUG > 1) eLIBs_printf("(%s),line(%d) : "fmt"\n", LOG_TAG, __LINE__, ##arg)
    #define alib_logv(fmt, arg...) if(ALIB_DEBUG > 2) eLIBs_printf("(%s),line(%d) : "fmt"\n", LOG_TAG, __LINE__, ##arg)
#else
    #error "No specific platform..."
#endif

#endif
