/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : plat_systrace.h
  Version       : Initial Draft
  Author        : Allwinner PDC-PD5 Team
  Created       : 2019/07/04
  Last Modified :
  Description   :
  Function List :
  History       :
******************************************************************************/
#ifndef _PLAT_SYSTRACE_H_
#define _PLAT_SYSTRACE_H_

#ifdef MPPCFG_SYSTRACE
#include <awctrace.h>
#define MPP_AtraceBegin(tag, name)  atrace_begin(tag, name)
#define MPP_AtraceEnd(tag)          atrace_end(tag)
#else
#define MPP_AtraceBegin(tag, name)
#define MPP_AtraceEnd(tag)
#endif

#endif  /* _PLAT_SYSTRACE_H_ */

