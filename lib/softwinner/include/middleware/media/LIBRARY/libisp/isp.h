
/*
 ******************************************************************************
 *
 * isp.h
 *
 * Hawkview ISP - isp.h module
 *
 * Copyright (c) 2016 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Version		  Author         Date		    Description
 *
 *   3.0		  Yang Feng   	2016/05/27	VIDEO INPUT
 *
 *****************************************************************************
 */
#ifndef _ISP_H_
#define _ISP_H_
#include "include/isp_type.h"
#include "include/isp_manage.h"
#include "include/isp_tuning.h"
#include "isp_version.h"
int media_dev_init(void);
void media_dev_exit(void);
int isp_reset(int dev_id);
int isp_init(int dev_id);
int isp_update(int dev_id);
int isp_stop(int dev_id);
int isp_exit(int dev_id);
int isp_run(int dev_id);
HW_S32 isp_pthread_join(int dev_id);
HW_S32 isp_get_cfg(int dev_id, HW_U8 group_id, HW_U32 cfg_ids, void *cfg_data);
HW_S32 isp_set_cfg(int dev_id, HW_U8 group_id, HW_U32 cfg_ids, void *cfg_data);
HW_S32 isp_stats_req(int dev_id, struct isp_stats_context *stats_ctx);

HW_S32 isp_set_fps(int dev_id, int sensor_fps);
HW_S32 isp_set_attr_cfg(int dev_id, HW_U32 ctrl_id, void *value);
HW_S32 isp_get_attr_cfg(int dev_id, HW_U32 ctrl_id, void *value);
HW_S32 isp_set_saved_ctx(int dev_id);
HW_S32 isp_get_sensor_info(int dev_id, struct sensor_config *cfg);

HW_S32 isp_get_version(char* version);

/*******************isp for video buffer*********************/
HW_S32 isp_get_lv(int dev_id);

#endif /*_ISP_H_*/



