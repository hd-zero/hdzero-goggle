#ifndef ALIB_UTILS
#define ALIB_UTILS
#include "alib_typedef.h"
/*
* Register Ur Platform
* __OS_LINUX
* __OS_ANDROID
* __OS_DS5
* __OS_UCOS
* __OS_WIN
* Coming here...
*/
/*
 * inline
 * __inline
 * __inline__
 *__forceinline
 *I just didn't know how to distinguish them
 * */

//add by khan, 1: return without data
//                   2: return for no data
#define RETURN_ACCORDING_TO_EXIT_MODE(a)   \
        if(a == 1) \
        { \
            return ERR_AUDIO_DEC_NO_BITSTREAM; \
        } \
        else \
        { \
            return ERR_AUDIO_DEC_ABSEND; \
        }

#define RETURN_FOR_DATA_LACKLESS(a)    \
        if(a == ERR_AUDIO_DEC_NO_BITSTREAM || a == ERR_AUDIO_DEC_ABSEND) \
        { \
            return a; \
        }

#define CHECK_EXTERN_RW_API(p)    \
		if(!p->FileReadInfo->FREAD_AC320 || !p->FileReadInfo->ShowAbsBits || \
  			!p->FileReadInfo->FlushAbsBits || !p->FileReadInfo->FSeek_AC320 || \
  			!p->FileReadInfo->FSeek_AC320_Origin || !p->FileReadInfo->BigLitEndianCheck) \
        { \
            alib_loge("extern read/write api hasn't be initialised..."); \
            return -1; \
        }

#ifndef NULL
#define NULL 0
#endif

#ifndef	ALIB_UINT_MAX
#define ALIB_UINT_MAX      0xffffffff    /* maximum unsigned int value */
#endif
#ifndef	ALIB_INT_MAX
#define ALIB_INT_MAX       2147483647    /* maximum (signed) int value */
#endif
#ifndef	ALIB_INT_MIN
#define ALIB_INT_MIN       (-2147483647 - 1)    /* maximum (signed) int value */
#endif

#ifndef	ALIB_SHRT_MAX
#define	ALIB_SHRT_MAX 32768
#endif
#ifndef	ALIB_SHRT_MIN
#define ALIB_SHRT_MIN    (-32768)
#endif

#define ALIB_ABS(a) ((a) >= 0 ? (a) : (-(a)))
#define ALIB_SIGN(a) ((a) > 0 ? 1 : -1)
#define ALIB_ALIGN(x, a) (((x)+(a)-1)&~((a)-1))
#define ALIB_MIN(a, b) (a>b?b:a)
#define ALIB_MAX(a, b) (a>b?a:b)

#if (defined __OS_LINUX) || (defined __OS_ANDROID)

#include <math.h>
#define alib_inline inline
#define alib_unuse __attribute__((unused))

#elif (defined __OS_DS5) || (defined __OS_WIN)

#include <math.h>
#define alib_inline __inline
#define alib_unuse __attribute__((unused))

static alib_inline long lrint(double x)
{
    return (long)rint(x);
}

static alib_inline long lrintf(float x)
{
    return (long)rintf(x);
}

#elif (defined __OS_UCOS)

extern void abort(void);
extern int abs(int);
extern double ceil(double);
extern double fabs(double);
extern double floor(double);
extern double sqrt(double);
extern double log(double);
extern double rint(double);
extern float  rintf(float);

/*
 * TODO:
 *     sin
 *     cos
 *     atan
 *     pow
 *     exp
 * */

#define alib_inline __inline
#define alib_unuse __attribute__((unused))

static alib_inline long lrint(double x)
{
    return (long)rint(x);
}

static alib_inline long lrintf(float x)
{
    return (long)rintf(x);
}

#else
#error "No specific platform..."
#endif

static alib_inline void* alib_memmove(void* dest, const void* src, alib_size count)
{
    char* tmp_dest = (char*)dest;
    const char* tmp_src = (const char*)src;

    if( dest == NULL || src == NULL || count == 0)
    {
        return NULL;
    }

    if( tmp_dest == tmp_src )
    {
        return dest;
    }
    else if( tmp_dest < tmp_src )
    {
        char* ptr = (char*)tmp_src;
        while(count)
        {
            *tmp_dest = *ptr;
            tmp_dest++;
            ptr++;
            count--;
        }
    }
    else
    {
        char* src_ptr_end  = (char*)(tmp_src + count - 1);
        char* dest_ptr_end = (char*)(tmp_dest + count - 1);
        while(count)
        {
            *dest_ptr_end = *src_ptr_end;
            dest_ptr_end--;
            src_ptr_end--;
            count--;
        }
    }
	return dest;
}

static alib_inline alib_int32 alib_toupper(alib_int32 c)
{
    if ((c >= 'a') && (c <= 'z'))
        return c + ('A' - 'a');
    return c;
}
#endif
