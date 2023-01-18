/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : mpp_version.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2017/07/25
  Last Modified :
  Description   : mpi functions declaration
  Function List :
  History       :
******************************************************************************/

#ifndef _MPP_VERSION_H_
#define _MPP_VERSION_H_

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define REPO_MODULE_NAME "MPP-Platform"
#define REPO_TAG "V1.0 Release"
#define REPO_BRANCH "v5-dev"
#define REPO_COMMIT "4eed413532ad87725895a8cb1ba75e22fc328233"
#define REPO_DATE "20170725"
#define RELEASE_AUTHOR "jenkins"

static inline void MPPLogVersionInfo(void)
{
    printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>> Media Process Platform<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");
    printf("modName : %s\n", REPO_MODULE_NAME);
    printf("tag   : %s\n", REPO_TAG);
    printf("branch: %s\n", REPO_BRANCH);
    printf("commit: %s\n", REPO_COMMIT);
    printf("date  : %s\n", REPO_DATE);
    printf("author: %s\n", RELEASE_AUTHOR);
}

#ifdef __cplusplus
}
#endif

#endif  /* _MPP_VERSION_H_ */

