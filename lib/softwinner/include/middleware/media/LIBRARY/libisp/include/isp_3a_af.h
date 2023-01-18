
/*
 ******************************************************************************
 *
 * isp_3a_af.h
 *
 * Hawkview ISP - isp_3a_af.h module
 *
 * Copyright (c) 2016 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Version		  Author         Date		    Description
 *
 *   3.0		  Yang Feng   	2016/03/31	VIDEO INPUT
 *
 *****************************************************************************
 */

#ifndef _ISP_3A_AF_LIB_H_
#define _ISP_3A_AF_LIB_H_

/* focus range 0~1024 */
#define ISP_AF_DIR_TH	24
#define ISP_AF_DIR_TH_MAX	96

#define FOCUS_MIN	0
#define FOCUS_MAX	1024

#define ISP_LIB_USE_AF		0

typedef struct isp_af_ini_cfg {
	HW_S32 af_use_otp;
	HW_S32 vcm_min_code;
	HW_S32 vcm_max_code;
	HW_S32 af_interval_time;
	HW_S32 af_speed_ind;
	HW_S32 af_auto_fine_en;
	HW_S32 af_single_fine_en;
	HW_S32 af_fine_step;
	HW_S32 af_move_cnt;
	HW_S32 af_still_cnt;
	HW_S32 af_move_monitor_cnt;
	HW_S32 af_still_monitor_cnt;
	HW_S32 af_stable_min;
	HW_S32 af_stable_max;
	HW_S32 af_low_light_lv;
	HW_S32 af_near_tolerance;
	HW_S32 af_far_tolerance;
	HW_S32 af_tolerance_off;
	HW_S32 af_peak_th;
	HW_S32 af_dir_th;
	HW_S32 af_change_ratio;
	HW_S32 af_move_minus;
	HW_S32 af_still_minus;
	HW_S32 af_scene_motion_th;
	HW_S32 af_tolerance_tbl_len;
	HW_S32 af_std_code_tbl[20];
	HW_S32 af_tolerance_value_tbl[20];
} af_ini_cfg_t;

enum auto_focus_run_mode {
	AUTO_FOCUS_MANUAL	= 0,
	AUTO_FOCUS_CONTINUEOUS	= 1,
	AUTO_FOCUS_TOUCH	= 2,
	AUTO_FOCUS_SNAP		= 3,
};

enum auto_focus_metering_mode {
	AUTO_FOCUS_METERING_AVERAGE		= 0,
	AUTO_FOCUS_METERING_CENTER_WEIGHTED	= 1,
	AUTO_FOCUS_METERING_SPOT		= 2,
	AUTO_FOCUS_METERING_MATRIX		= 3,
};

enum auto_focus_range_new {
	AUTO_FOCUS_RANGE_AUTO		= 0,
	AUTO_FOCUS_RANGE_NORMAL		= 1,
	AUTO_FOCUS_RANGE_MACRO		= 2,
	AUTO_FOCUS_RANGE_INFINITY	= 3,
};

enum auto_focus_status {
	AUTO_FOCUS_STATUS_IDLE		= 0,
	AUTO_FOCUS_STATUS_BUSY		= 1,
	AUTO_FOCUS_STATUS_REACHED	= 2,
	AUTO_FOCUS_STATUS_APPROCH	= 3,
	AUTO_FOCUS_STATUS_REFOCUS	= 4,
	AUTO_FOCUS_STATUS_FAILED	= 5,
};

typedef struct isp_af_test_config {
	HW_S32 isp_test_mode;
	HW_S32 isp_test_focus;

	HW_S32 focus_start;
	HW_S32 focus_step;
	HW_S32 focus_end;
	HW_S32 focus_change_interval;
	HW_S32 af_en;

} af_test_config_t;

typedef enum isp_af_param_type {
	ISP_AF_INI_DATA,
	ISP_AF_TRIGGER,

	ISP_AF_PARAM_TYPE_MAX,
} af_param_type_t;

struct vcm_para {
	HW_S32 vcm_max_code;
	HW_S32 vcm_min_code;
};

typedef struct isp_af_param {
	af_param_type_t type;
	HW_S32 isp_platform_id;
	HW_S32 af_frame_id;
	af_ini_cfg_t af_ini;
	HW_S32 focus_absolute;
	HW_S32 focus_relative;
	enum auto_focus_run_mode af_run_mode;
	enum auto_focus_metering_mode af_metering_mode;
	enum auto_focus_range_new af_range;
	struct vcm_para vcm;
	bool focus_lock;
	isp_sensor_info_t sensor_info;
	af_test_config_t test_cfg;
	HW_S32 auto_focus_trigger;
} af_param_t;

typedef struct isp_af_stats {
	struct isp_af_stats_s *af_stats;
} af_stats_t;

typedef struct isp_af_result {
	enum auto_focus_status af_status_output;
	HW_U32 last_code_output;
	HW_U32 real_code_output;
	HW_U32 std_code_output;
	HW_U16 af_sap_lim_output;
	HW_U32 af_sharp_output;
} af_result_t;

typedef struct isp_af_core_ops {
	HW_S32 (*isp_af_set_params)(void *af_core_obj, af_param_t *param, af_result_t *result);
	HW_S32 (*isp_af_get_params)(void *af_core_obj, af_param_t **param);
	HW_S32 (*isp_af_run)(void *af_core_obj, af_stats_t *stats, af_result_t *result);
} isp_af_core_ops_t;

void* af_init(isp_af_core_ops_t **af_core_ops);
void  af_exit(void *af_core_obj);

#endif /*_ISP_3A_AF_LIB_H_*/

