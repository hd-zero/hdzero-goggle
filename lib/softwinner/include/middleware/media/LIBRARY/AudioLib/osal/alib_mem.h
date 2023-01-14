#ifndef ALIB_MEM_H
#define ALIB_MEM_H

/*
* Register Ur Platform
* __OS_LINUX
* __OS_ANDROID
* __OS_DS5
* __OS_UCOS
* __OS_WIN
* Coming here...
*/

#if (defined __OS_LINUX) || (defined __OS_ANDROID) || (defined __OS_DS5) || (defined __OS_WIN)

    #include <stdlib.h>
	#include <stdio.h>
	#include <string.h>

	#define alib_malloc  malloc
	#define alib_realloc realloc
    #define alib_memset  memset
    #define alib_free    free
    #define alib_memcpy  memcpy
    #define alib_memcmp  memcmp

    #define alib_strlen  strlen
	#define alib_strcpy  strcpy
	#define alib_strcat  strcat
	#define alib_strncpy strncpy
	#define alib_strcmp  strcmp
	#define alib_strncmp strncmp
	#define alib_strchr  strchr
	#define alib_strstr  strstr

#elif (defined __OS_UCOS)

	extern void *CEDAR_malloc(int size);
	extern int  CEDAR_free(void *pbuf);
	extern void *eLIBs_realloc(void *ptr, unsigned int size);
	extern void     eLIBs_memset(void * pmem,   unsigned char data_val, unsigned int size);
	extern void   * eLIBs_memcpy    (void * pdest,  const void   * psrc, unsigned int size);
	extern int      eLIBs_memcmp    (const void * p1_mem, const void * p2_mem, unsigned int size);
	extern unsigned int eLIBs_strlen    (const char * pstr);
	extern char   * eLIBs_strcpy    (char * pdest, const char * psrc);
	extern char   * eLIBs_strncpy   (char * pdest, const char * psrc, unsigned int len_max);
	extern short    eLIBs_strcmp    (const char * p1_str, const char * p2_str);
	extern short    eLIBs_strncmp   (const char * p1_str, const char * p2_str,   unsigned int len_max);
	extern char   * eLIBs_strchr    (char * pstr,   char     srch_char);
	extern char   * eLIBs_strstr    (char * pstr,   char   * srch_str);
    extern char   * eLIBs_strcat    (char * pdest, const char * pstr_cat);

    extern void qsort ( void * base, unsigned int num, unsigned int size, int ( * comparator ) ( const void *, const void * ) );

	#define alib_malloc  CEDAR_malloc
	#define alib_realloc  eLIBs_realloc
    #define alib_memset  eLIBs_memset
    #define alib_free    CEDAR_free
    #define alib_memcpy  eLIBs_memcpy
    #define alib_memcmp  eLIBs_memcmp
    #define alib_strlen  eLIBs_strlen
	#define alib_strcpy  eLIBs_strcpy
	#define alib_strcat  eLIBs_strcat
	#define alib_strncpy eLIBs_strncpy
	#define alib_strcmp  eLIBs_strcmp
	#define alib_strncmp eLIBs_strncmp
	#define alib_strchr  eLIBs_strchr
	#define alib_strstr  eLIBs_strstr

#else
    #error "No specific platform..."
#endif

#endif
