
/*
 ******************************************************************************
 *
 * isp_3a_awb.h
 *
 * Hawkview ISP - isp_3a_awb.h module
 *
 * Copyright (c) 2016 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Version		  Author         Date		    Description
 *
 *   3.0		  Yang Feng   	2016/03/31	VIDEO INPUT
 *
 *****************************************************************************
 */

#ifndef _ISP_3A_AWB_H_
#define _ISP_3A_AWB_H_
#define ISP_CFA_DIR_TH	2047//500
#define AWB_SAT_DEF_LIM	255
#define ISP_MAX_AWB_GRAY_CAL_NUM	(1<<12)
#define ISP_MAX_AWB_AVG_SPEC	(1<<24)
#define ISP_AWB_GRAY_CNT_SHIFT	6

enum white_balance_mode {
	WB_MANUAL        = 0,
	WB_AUTO          = 1,
	WB_INCANDESCENT  = 2,
	WB_FLUORESCENT   = 3,
	WB_FLUORESCENT_H = 4,
	WB_HORIZON       = 5,
	WB_DAYLIGHT      = 6,
	WB_FLASH         = 7,
	WB_CLOUDY        = 8,
	WB_SHADE         = 9,
	WB_TUNGSTEN      = 10,
};

typedef struct isp_awb_setting {
	enum white_balance_mode wb_mode;
	HW_S32 wb_temperature;
	bool white_balance_lock;
	struct isp_wb_gain wb_gain_manual;
	struct isp_h3a_coor_win awb_coor;
} isp_awb_setting_t;


typedef struct isp_awb_ini_cfg {
	HW_S32 awb_interval;
	HW_S32 awb_speed;
	HW_S32 awb_color_temper_low;
	HW_S32 awb_color_temper_high;
	HW_S32 awb_base_temper;

	HW_S32 awb_green_zone_dist;
	HW_S32 awb_blue_sky_dist;

	HW_S32 awb_light_num;
	HW_S32 awb_ext_light_num;
	HW_S32 awb_skin_color_num;
	HW_S32 awb_special_color_num;
	HW_S32 awb_light_info[320];
	HW_S32 awb_ext_light_info[320];
	HW_S32 awb_skin_color_info[160];
	HW_S32 awb_special_color_info[320];
	HW_S32 awb_preset_gain[22];
	HW_S32 awb_rgain_favor;
	HW_S32 awb_bgain_favor;

} awb_ini_cfg_t;

typedef enum isp_awb_param_type {
	ISP_AWB_INI_DATA,
	ISP_AWB_PARAM_TYPE_MAX,
} awb_param_type_t;

typedef struct awb_test_config {
	HW_S32 isp_test_mode;
	HW_S32 isp_color_temp;
	HW_S32 awb_en;
} awb_test_config_t;

typedef struct isp_awb_param {
	awb_param_type_t type;
	HW_S32 isp_platform_id;
	HW_S32 awb_frame_id;
	isp_awb_setting_t awb_ctrl;
	awb_ini_cfg_t awb_ini;
	isp_sensor_info_t awb_sensor_info;
	awb_test_config_t test_cfg;
} awb_param_t;

typedef struct isp_awb_stats {
	struct isp_awb_stats_s *awb_stats;
} awb_stats_t;

typedef struct isp_awb_result {
	struct isp_wb_gain wb_gain_output;
	HW_S32 color_temp_output;
} awb_result_t;

typedef struct isp_awb_core_ops {
	HW_S32 (*isp_awb_set_params)(void *awb_core_obj, awb_param_t *param, awb_result_t *result);
	HW_S32 (*isp_awb_get_params)(void *awb_core_obj, awb_param_t **param);
	HW_S32 (*isp_awb_run)(void *awb_core_obj, awb_stats_t *stats, awb_result_t *result);
} isp_awb_core_ops_t;

void* awb_init(isp_awb_core_ops_t **awb_core_ops);
void  awb_exit(void *awb_core_obj);

#endif /*_ISP_3A_AWB_H_*/


