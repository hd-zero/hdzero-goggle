
/*
 ******************************************************************************
 *
 * isp_3a_md.h
 *
 * Hawkview ISP - isp_3a_md.h module
 *
 * Copyright (c) 2016 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Version		  Author         Date		    Description
 *
 *   3.0		  Yang Feng   	2016/03/31	VIDEO INPUT
 *
 *****************************************************************************
 */

#ifndef _ISP_3A_MD_H_
#define _ISP_3A_MD_H_

#define ISP_LIB_USE_MD		0

enum direction {
	MD_LEFT = 0,
	MD_RIGHT,
	MD_UP,
	MD_DOWN,
	MD_BACKWARD,
	MD_FORWARD,
	MD_NO_MOTION,
};

typedef enum isp_md_param_type {
	ISP_MD_PARAM_TYPE_MAX,
} md_param_type_t;


typedef struct md_test_config {
	HW_S32 isp_test_mode;
} md_test_config_t;

typedef struct isp_md_param {
	md_param_type_t type;
	HW_S32 isp_platform_id;
	HW_S32 md_frame_id;
	HW_S32 af_scene_motion_th;
	isp_sensor_info_t md_sensor_info;
	md_test_config_t test_cfg;
} md_param_t;

typedef struct isp_md_stats {
	struct isp_md_stats_s *md_stats;

} md_stats_t;

typedef struct isp_md_result {
	HW_S32 motion_flag;
	enum direction motion_dir;
} md_result_t;

typedef struct isp_md_core_ops {
	HW_S32 (*isp_md_set_params)(void *md_core_obj, md_param_t *param, md_result_t *result);
	HW_S32 (*isp_md_get_params)(void *md_core_obj, md_param_t **param);
	HW_S32 (*isp_md_run)(void *md_core_obj, md_stats_t *stats, md_result_t *result);
} isp_md_core_ops_t;

void* md_init(isp_md_core_ops_t **md_core_ops);
void  md_exit(void *md_core_obj);


#endif /*_ISP_3A_MD_H_*/


