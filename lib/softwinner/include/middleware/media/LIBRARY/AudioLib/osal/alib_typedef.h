#ifndef ALIB_TYPEDEF_H
#define ALIB_TYPEDEF_H

/*
* Register Ur Platform
* __OS_LINUX
* __OS_ANDROID
* __OS_DS5
* __OS_UCOS
* __OS_WIN
* Coming here...
*/

typedef signed char     alib_int8;
typedef unsigned char   alib_uint8;
typedef short           alib_int16;
typedef unsigned short  alib_uint16;
typedef int             alib_int32;
typedef unsigned int    alib_uint32;

typedef void*           alib_handle;
typedef float           alib_f32;
typedef double          alib_lf64;

typedef long int          alib_intptr;
typedef unsigned long int alib_uintptr;

typedef void            alib_void;

typedef unsigned int    alib_size;

#ifndef false
#define false 0
#endif

#ifndef true
#define true  1
#endif


#if (defined __OS_LINUX) || (defined __OS_ANDROID) || (defined __OS_DS5)

#include <stdint.h>
typedef int64_t         alib_int64;
typedef uint64_t        alib_uint64;

#ifndef	TCHAR
typedef char TCHAR;
#endif

#elif (defined __OS_UCOS)

typedef signed long long    alib_int64;
typedef unsigned long long  alib_uint64;

#ifndef	TCHAR
typedef char TCHAR;
#endif

#elif (defined __OS_WIN)

typedef __int64           alib_int64;
typedef unsigned __int64  alib_uint64;
#include <tchar.h>

#else
#error "No specific platform..."
#endif

#endif
