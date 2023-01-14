
/*
 ******************************************************************************
 *
 * isp_rolloff.h
 *
 * Hawkview ISP - isp_rolloff.h module
 *
 * Copyright (c) 2016 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Version		  Author         Date		    Description
 *   3.0		  Yang Feng   	2015/10/22	ISP Tuning Tools Support
 *   3.1		  Yang Feng   	2016/03/29	VIDEO INPUT
 *
 *****************************************************************************
 */

#ifndef _ISP_3A_ROLLOFF_H_
#define _ISP_3A_ROLLOFF_H_

#define ISP_LIB_USE_ROLLOFF	0

typedef enum isp_rolloff_param_type {
	ISP_ROLLOFF_INI_DATA,
	ISP_ROLLOFF_PARAM_TYPE_MAX,
} rolloff_param_type_t;
typedef struct isp_rolloff_ini_cfg {
	int rolloff_ratio;
	unsigned short lens_table_ini[ISP_LENS_TBL_SIZE*3];
} rolloff_ini_cfg_t;

typedef struct isp_rolloff_param {
	rolloff_param_type_t type;
	int isp_platform_id;
	int rolloff_frame_id;
	isp_sensor_info_t rolloff_sensor_info;
	struct isp_lsc_config lsc_cfg;
	rolloff_ini_cfg_t rolloff_ini;
} rolloff_param_t;

typedef struct isp_rolloff_stats {
	struct isp_stats_s *rolloff_stats;
} rolloff_stats_t;

typedef struct isp_rolloff_result {
	unsigned short lens_table_output[ISP_LENS_TBL_SIZE*3];
} rolloff_result_t;

typedef struct isp_rolloff_core_ops {
	HW_S32 (*isp_rolloff_set_params)(void *rolloff_core_obj, rolloff_param_t *param, rolloff_result_t *result);
	HW_S32 (*isp_rolloff_get_params)(void *rolloff_core_obj, rolloff_param_t **param);
	HW_S32 (*isp_rolloff_run)(void *rolloff_core_obj, rolloff_stats_t *stats, rolloff_result_t *result);
} isp_rolloff_core_ops_t;

void* rolloff_init(isp_rolloff_core_ops_t **rolloff_core_ops);
void  rolloff_exit(void *rolloff_core_obj);


#endif /*_ISP_3A_ROLLOFF_H_*/



