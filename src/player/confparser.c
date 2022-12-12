/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : confparser
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/11/4
  Last Modified :
  Description   : confparser module
  Function List :
  History       :
******************************************************************************/

//#define LOG_NDEBUG 0
#define LOG_TAG "CfgParser"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "plat_log.h"
#include "confparser.h"

#define INI_INVALID_KEY     ((char*)-1)

/*
NOTICE: parse the config file, get the specific parameter string or val,
        according to defined name str.
        config file content format as following
        [never use "\r\n" as change line, just "\n"].

[parameter]
src_file = "/mnt/extsd/1080p.yuv"
src_size = 1080
dst_file = "/mnt/extsd/1080p.mov"
dst_size = 1080
encoder = "H.264"
dst_framerate = 25
dst_bitrate = 7000000
*/


int createConfParser(const char *conf_path, CONFPARSER_S *pCfg)
{
    if (pCfg==NULL || conf_path==NULL)
    {
        aloge("cfgParserInit para error");
        return -1;
    }

    pCfg->pDict = iniparser_load(conf_path);
    if (pCfg->pDict == NULL)
    {
        aloge("cannot parse file: %s\n", conf_path);
        return -1 ;
    }
    else
    {
        alogd("load conf file %s ok!\n", conf_path);
    }

    return 0;
}

void destroyConfParser(CONFPARSER_S *pCfg)
{
    if (pCfg!=NULL && pCfg->pDict!=NULL)
    {
        iniparser_freedict(pCfg->pDict);
        pCfg->pDict = NULL;
    }
}


int GetConfParaInt(CONFPARSER_S *pCfg, const char * key,int notfound)
{
    char parameterkey[128];

    if (pCfg==NULL || pCfg->pDict==NULL)
    {
        aloge("cfg pointer error");
        return 0;
    }
    else
    {
        sprintf(parameterkey,"parameter:%s",key);
        //sprintf(parameterkey,"%s",key);
        return iniparser_getint(pCfg->pDict, parameterkey, notfound);
    }
}

unsigned int GetConfParaUInt(CONFPARSER_S *pCfg, const char * key,int notfound)
{
    char parameterkey[128];

    if (pCfg==NULL || pCfg->pDict==NULL)
    {
        aloge("cfg pointer error");
        return 0;
    }
    else
    {
        sprintf(parameterkey,"parameter:%s",key);
        const char *str = iniparser_getstring(pCfg->pDict, parameterkey, INI_INVALID_KEY);
        if(str==INI_INVALID_KEY)
        {
            return notfound;
        }
        return (unsigned int)strtoul(str, NULL, 0);
    }
}

const char *GetConfParaString(CONFPARSER_S *pCfg, const char *key, const char *def)
{
    char parameterkey[128];

    if (pCfg==NULL || pCfg->pDict==NULL)
    {
        aloge("cfg pointer error");
        return NULL;
    }
    else
    {
        sprintf(parameterkey,"parameter:%s",key);
        return iniparser_getstring(pCfg->pDict, parameterkey, def);
    }
}

double GetConfParaDouble(CONFPARSER_S *pCfg, const char *key, double notfound)
{
    char parameterkey[128];

    if (pCfg==NULL || pCfg->pDict==NULL)
    {
        aloge("cfg pointer error");
        return (double)0.0;
    }
    else
    {
        sprintf(parameterkey,"parameter:%s",key);
        return iniparser_getdouble(pCfg->pDict, parameterkey, notfound);
    }
}

BOOL GetConfParaBoolean(CONFPARSER_S *pCfg, const char *key, int notfound)
{
    char parameterkey[128];

    if (pCfg==NULL || pCfg->pDict==NULL)
    {
        aloge("cfg pointer error");
        return FALSE;
    }
    else
    {
        sprintf(parameterkey,"parameter:%s",key);
        return iniparser_getboolean(pCfg->pDict, parameterkey, notfound);
    }
}

