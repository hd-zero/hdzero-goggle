
/*
 ******************************************************************************
 *
 * isp_type.h
 *
 * Hawkview ISP - isp_type.h module
 *
 * Copyright (c) 2016 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Version		  Author         Date		    Description
 *
 *   3.0		  Yang Feng   	2016/03/24	VIDEO INPUT
 *
 *****************************************************************************
 */
#ifndef _HW_TYPE_H_
#define _HW_TYPE_H_

typedef unsigned char		HW_U8;
typedef unsigned short		HW_U16;
typedef unsigned int		HW_U32;

typedef signed char             HW_S8;
typedef short                   HW_S16;
typedef int                     HW_S32;

typedef unsigned char           HW_U0Q8;
typedef unsigned char           HW_U1Q7;
typedef unsigned char           HW_U5Q3;

typedef unsigned short          HW_U0Q16;
typedef unsigned short          HW_U4Q12;
typedef unsigned short          HW_U6Q10;
typedef unsigned short          HW_U8Q8;
typedef unsigned short          HW_U12Q4;
typedef unsigned short          HW_U14Q2;

typedef short                   HW_S8Q8;
typedef short                   HW_S9Q7;
typedef short                   HW_S14Q2;
typedef short                   HW_S1Q15;

typedef unsigned int            HW_U22Q10;
typedef unsigned int            HW_U25Q7;

typedef int                     HW_S25Q7;
typedef int                     HW_S16Q16;
typedef unsigned short          HW_U8Q4F4;
typedef float			HW_FLOAT;
typedef double                  HW_DOUBLE;
typedef unsigned long long	HW_U64;
typedef long long		HW_S64;

typedef char                    HW_CHAR;
#define HW_VOID                 void

typedef enum {
    HW_FALSE = 0,
    HW_TRUE  = 1,
} HW_BOOL;

#ifndef NULL
    #define NULL    0L
#endif

#define HW_NULL     0L
#define HW_SUCCESS  0
#define HW_FAILURE  (-1)


#endif /*_HW_TYPE_H_*/

