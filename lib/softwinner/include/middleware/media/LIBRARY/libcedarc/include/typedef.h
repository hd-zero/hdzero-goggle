
/*
* Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
* All rights reserved.
*
* File : typedef.h
* Description :
* History :
*   Author  : xyliu <xyliu@allwinnertech.com>
*   Date    : 2016/04/13
*   Comment :
*
*
*/


#ifndef TYPEDEF_H
#define TYPEDEF_H
#include <stdint.h>

    typedef unsigned char u8;
    typedef unsigned short u16;
    typedef unsigned int u32;
#ifdef COMPILER_ARMCC
    typedef unsigned __int64 u64;
#else
    typedef unsigned long long u64;
#endif
    typedef signed char s8;
    typedef signed short s16;
    typedef signed int s32;
#ifdef COMPILER_ARMCC
    typedef signed __int64 s64;
#else
    typedef signed long long s64;
#endif
    typedef uintptr_t size_addr;

#endif

