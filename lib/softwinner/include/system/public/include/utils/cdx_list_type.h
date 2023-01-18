/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : plat_type.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/03/15
  Last Modified :
  Description   : The common data type defination, used in platform scope.
  Function List :
  History       :
******************************************************************************/
#ifndef _CDX_LIST_TYPE_H_
#define _CDX_LIST_TYPE_H_

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

struct list_head {
	struct list_head *next, *prev;
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* _CDX_LIST_TYPE_H_ */

