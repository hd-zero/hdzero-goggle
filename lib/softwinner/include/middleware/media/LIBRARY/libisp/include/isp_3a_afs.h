
/*
 ******************************************************************************
 *
 * isp_3a_afs.h
 *
 * Hawkview ISP - isp_3a_afs.h module
 *
 * Copyright (c) 2016 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Version		  Author         Date		    Description
 *   3.0		  Yang Feng   	2015/10/22	ISP Tuning Tools Support
 *   3.1		  Yang Feng   	2016/03/29	VIDEO INPUT
 *
 *****************************************************************************
 */

#ifndef _ISP_3A_AFS_H_
#define _ISP_3A_AFS_H_

enum power_line_frequency {
	FREQUENCY_DISABLED	= 0,
	FREQUENCY_50HZ		= 1,
	FREQUENCY_60HZ		= 2,
	FREQUENCY_AUTO		= 3,
};

enum detected_flicker_type
{
	FLICKER_NO	= 0,
	FLICKER_50HZ	= 1,
	FLICKER_60HZ	= 2,
};

typedef enum isp_afs_param_type {
	ISP_AFS_PARAM_TYPE_MAX,
} afs_param_type_t;

typedef struct afs_test_config {
	HW_S32 isp_test_mode;
	HW_S32 afs_en;
} afs_test_config_t;

typedef struct isp_afs_param {
	afs_param_type_t type;
	HW_S32 isp_platform_id;
	HW_S32 afs_frame_id;
	HW_S32 auto_afs_flag;
	HW_S32 flicker_ratio;
	HW_S32 flicker_type_ini;
	isp_sensor_info_t afs_sensor_info;
	afs_test_config_t test_cfg;
	enum power_line_frequency flicker_mode;
} afs_param_t;

typedef struct isp_afs_stats {
	struct isp_afs_stats_s *afs_stats;
} afs_stats_t;

typedef struct isp_afs_result {
	enum detected_flicker_type flicker_type_output;
} afs_result_t;

typedef struct isp_afs_core_ops {
	HW_S32 (*isp_afs_set_params)(void *afs_core_obj, afs_param_t *param, afs_result_t *result);
	HW_S32 (*isp_afs_get_params)(void *afs_core_obj, afs_param_t **param);
	HW_S32 (*isp_afs_run)(void *afs_core_obj, afs_stats_t *stats, afs_result_t *result);
} isp_afs_core_ops_t;

void* afs_init(isp_afs_core_ops_t **afs_core_ops);
void  afs_exit(void *afs_core_obj);


#endif /*_ISP_3A_AFS_H_*/



