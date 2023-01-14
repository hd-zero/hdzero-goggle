/*
 ******************************************************************************
 *
 * isp_tuning_adapter.h
 *
 * Hawkview ISP - isp_tuning_adapter.h module
 *
 * Copyright (c) 2016 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Version		  Author         Date		    Description
 *
 *   1.0		  clarkyy	2016/05/09	VIDEO INPUT
 *
 *****************************************************************************
 */

#ifndef _ISP_TUNING_CFG_H_
#define _ISP_TUNING_CFG_H_

#include "../include/isp_type.h"
#include "../include/isp_comm.h"

#define AW_ERR_VI_INVALID_PARA			-1
#define AW_ERR_VI_INVALID_DEVID			-2
#define AW_ERR_VI_INVALID_CHNID			-3
#define AW_ERR_VI_INVALID_NULL_PTR		-4
#define AW_ERR_VI_FAILED_NOTCONFIG		-5
#define AW_ERR_VI_SYS_NOTREADY			-6
#define AW_ERR_VI_BUF_EMPTY				-7
#define AW_ERR_VI_BUF_FULL				-8
#define AW_ERR_VI_NOMEM					-9
#define AW_ERR_VI_NOT_SUPPORT			-10
#define AW_ERR_VI_BUSY					-11
#define AW_ERR_VI_FAILED_NOTENABLE		-12
#define AW_ERR_VI_FAILED_NOTDISABLE		-13
#define AW_ERR_VI_CFG_TIMEOUT			-14
#define AW_ERR_VI_NORM_UNMATCH			-15
#define AW_ERR_VI_INVALID_PHYCHNID		-16
#define AW_ERR_VI_FAILED_NOTBIND		-17
#define AW_ERR_VI_FAILED_BINDED			-18



/*
 * id defination
 * 5 bytes total, first byte is the group id(current 4 groups total), left 4 bytes is the cfg id
 *   |group id|cfg id|cfg id|cfg id|cfg id|
 * group id : 0x1 ~ 0xFF, each number means a individual group id, total 255 group ids
 * cfg id: 0x1 ~ 0xFFFFFFFF, each bit means a individual cfg id, total 32 cfg ids
 */
typedef enum {
	HW_ISP_CFG_TEST						= 0x01,
	HW_ISP_CFG_3A						= 0x02,
	HW_ISP_CFG_TUNING					= 0x03,
	HW_ISP_CFG_TUNING_TABLES			= 0x04,
	HW_ISP_CFG_DYNAMIC					= 0x05,
	HW_ISP_CFG_GROUP_COUNT
} hw_isp_cfg_groups;

typedef enum {
	/* isp_test_param_cfg */
	HW_ISP_CFG_TEST_PUB					= 0x00000001,
	HW_ISP_CFG_TEST_EXPTIME				= 0x00000002,
	HW_ISP_CFG_TEST_GAIN				= 0x00000004,
	HW_ISP_CFG_TEST_FOCUS				= 0x00000008,
	HW_ISP_CFG_TEST_FORCED				= 0x00000010,
	HW_ISP_CFG_TEST_ENABLE				= 0x00000020,
} hw_isp_cfg_test_ids;

typedef enum {
	/* isp_3a_param_cfg */
	/* ae */
	HW_ISP_CFG_AE_PUB					= 0x00000001,
	HW_ISP_CFG_AE_PREVIEW_TBL			= 0x00000002,
	HW_ISP_CFG_AE_CAPTURE_TBL			= 0x00000004,
	HW_ISP_CFG_AE_VIDEO_TBL				= 0x00000008,
	HW_ISP_CFG_AE_WIN_WEIGHT			= 0x00000010,
	HW_ISP_CFG_AE_DELAY					= 0x00000020,
	/* awb */
	HW_ISP_CFG_AWB_PUB					= 0x00000040,
	HW_ISP_CFG_AWB_TEMP_RANGE			= 0x00000080,
	HW_ISP_CFG_AWB_DIST					= 0x00000100,
	HW_ISP_CFG_AWB_LIGHT_INFO			= 0x00000200,
	HW_ISP_CFG_AWB_EXT_LIGHT_INFO		= 0x00000400,
	HW_ISP_CFG_AWB_SKIN_INFO			= 0x00000800,
	HW_ISP_CFG_AWB_SPECIAL_INFO			= 0x00001000,
	HW_ISP_CFG_AWB_PRESET_GAIN			= 0x00002000,
	HW_ISP_CFG_AWB_FAVOR				= 0x00004000,
	/* af */
	HW_ISP_CFG_AF_VCM_CODE				= 0x00008000,
	HW_ISP_CFG_AF_OTP					= 0x00010000,
	HW_ISP_CFG_AF_SPEED					= 0x00020000,
	HW_ISP_CFG_AF_FINE_SEARCH			= 0x00040000,
	HW_ISP_CFG_AF_REFOCUS				= 0x00080000,
	HW_ISP_CFG_AF_TOLERANCE				= 0x00100000,
	HW_ISP_CFG_AF_SCENE					= 0x00200000,
} hw_isp_cfg_3a_ids;

typedef enum {
	/* isp_tuning_param_cfg */
	HW_ISP_CFG_TUNING_FLASH				= 0x00000001,
	HW_ISP_CFG_TUNING_FLICKER			= 0x00000002,
	HW_ISP_CFG_TUNING_VISUAL_ANGLE		= 0x00000004,
	HW_ISP_CFG_TUNING_GTM				= 0x00000008,
	HW_ISP_CFG_TUNING_CFA				= 0x00000010,
	HW_ISP_CFG_TUNING_CTC				= 0x00000020,
	HW_ISP_CFG_TUNING_DIGITAL_GAIN		= 0x00000040,
	HW_ISP_CFG_TUNING_CCM_LOW			= 0x00000080,
	HW_ISP_CFG_TUNING_CCM_MID			= 0x00000100,
	HW_ISP_CFG_TUNING_CCM_HIGH			= 0x00000200,
	HW_ISP_CFG_TUNING_PLTM				= 0x00000400,
} hw_isp_cfg_tuning_ids;

typedef enum {
	/* tuning tables */
	HW_ISP_CFG_TUNING_LSC				= 0x00000001,
	HW_ISP_CFG_TUNING_GAMMA				= 0x00000002,
	HW_ISP_CFG_TUNING_LINEARITY			= 0x00000004,
	HW_ISP_CFG_TUNING_DISTORTION		= 0x00000008,
	HW_ISP_CFG_TUNING_BDNF				= 0x00000010,
	HW_ISP_CFG_TUNING_TDNF				= 0x00000020,
	HW_ISP_CFG_TUNING_CONTRAST			= 0x00000040,
	HW_ISP_CFG_TUNING_SHARP				= 0x00000080,
	HW_ISP_CFG_TUNING_CEM				= 0x00000100,
	HW_ISP_CFG_TUNING_CEM_1				= 0x00000200,
	HW_ISP_CFG_TUNING_PLTM_TBL			= 0x00000400,
	HW_ISP_CFG_TUNING_WDR				= 0x00000800,
} hw_isp_cfg_tuning_table_ids;

typedef enum {
	/* isp_dynamic_cfg */
	HW_ISP_CFG_DYNAMIC_LUM_POINT		= 0x00000001,
	HW_ISP_CFG_DYNAMIC_GAIN_POINT		= 0x00000002,
	HW_ISP_CFG_DYNAMIC_SHARP			= 0x00000004,
	HW_ISP_CFG_DYNAMIC_CONTRAST			= 0x00000008,
	HW_ISP_CFG_DYNAMIC_DENOISE			= 0x00000010,
	HW_ISP_CFG_DYNAMIC_SENSOR_OFFSET	= 0x00000020,
	HW_ISP_CFG_DYNAMIC_BLACK_LV			= 0x00000040,
	HW_ISP_CFG_DYNAMIC_DPC				= 0x00000080,
	HW_ISP_CFG_DYNAMIC_PLTM				= 0x00000100,
	HW_ISP_CFG_DYNAMIC_DEFOG			= 0x00000200,
	HW_ISP_CFG_DYNAMIC_HISTOGRAM		= 0x00000400,
	HW_ISP_CFG_DYNAMIC_SATURATION		= 0x00000800,
	HW_ISP_CFG_DYNAMIC_CEM				= 0x00001000,
	HW_ISP_CFG_DYNAMIC_TDF				= 0x00002000,
	HW_ISP_CFG_DYNAMIC_AE				= 0x00004000,
	HW_ISP_CFG_DYNAMIC_GTM				= 0x00008000,
} hw_isp_cfg_dynamic_ids;

typedef enum {
	/*isp_ctrl*/
	ISP_CTRL_MODULE_EN = 0,
	ISP_CTRL_DIGITAL_GAIN,
	ISP_CTRL_PLTMWDR_STR,
	ISP_CTRL_DN_STR,
	ISP_CTRL_3DN_STR,
	ISP_CTRL_HIGH_LIGHT,
	ISP_CTRL_BACK_LIGHT,
	ISP_CTRL_WB_MGAIN,
	ISP_CTRL_AGAIN_DGAIN,
	ISP_CTRL_COLOR_EFFECT,
	ISP_CTRL_AE_ROI,
	ISP_CTRL_COLOR_TEMP,
	ISP_CTRL_EV_IDX,
} hw_isp_ctrl_cfg_ids;

struct isp_test_pub_cfg {
	HW_S32		test_mode;
	HW_S32		gain;
	HW_S32		exp_line;
	HW_S32		color_temp;
	HW_S32		log_param;
};

struct isp_test_item_cfg {
	HW_S32		enable;
	HW_S32		start;
	HW_S32		step;
	HW_S32		end;
	HW_S32		change_interval;
};

struct isp_test_forced_cfg {
	HW_S32		ae_enable;
	HW_S32		lum;
};

struct isp_test_enable_cfg {
	HW_S32		manual;
	HW_S32		afs;
	HW_S32		sharp;
	HW_S32		contrast;
	HW_S32		denoise;
	HW_S32		drc;
	HW_S32		cem;
	HW_S32		lsc;
	HW_S32		gamma;
	HW_S32		cm;
	HW_S32		ae;
	HW_S32		af;
	HW_S32		awb;
	HW_S32		hist;
	HW_S32		blc;
	HW_S32		so;
	HW_S32		wb;
	HW_S32		otf_dpc;
	HW_S32		cfa;
	HW_S32		tdf;
	HW_S32		cnr;
	HW_S32		saturation;
	HW_S32		defog;
	HW_S32		linearity;
	HW_S32		gtm;
	HW_S32		dig_gain;
	HW_S32		pltm;
	HW_S32		wdr;
	HW_S32		ctc;
};

/* isp_test_param cfg */
struct isp_test_param_cfg {
	struct isp_test_pub_cfg		test_pub;		/* id: 0x0100000001 */
	struct isp_test_item_cfg	test_exptime;	/* id: 0x0100000002 */
	struct isp_test_item_cfg	test_gain;		/* id: 0x0100000004 */
	struct isp_test_item_cfg	test_focus;		/* id: 0x0100000008 */
	struct isp_test_forced_cfg	test_forced;	/* id: 0x0100000010 */
	struct isp_test_enable_cfg	test_enable;	/* id: 0x0100000020 */
};

struct isp_ae_pub_cfg {
	HW_S32		define_table;
	HW_S32		max_lv;
	HW_S32		hist_mode_en;
	HW_S32		hist_sel;
	HW_S32		stat_sel;
	HW_S32		compensation_step;
	HW_S32		touch_dist_index;
	HW_S32		iso2gain_ratio;
	HW_S32		fno_table[16];
	HW_S32		ki;
	HW_S32		conv_data_index;
	HW_S32		blowout_pre_en;
	HW_S32		blowout_attr;
	HW_S32		wdr_cfg[3];
};

struct isp_ae_table_cfg {
	HW_S32		length;
	struct ae_table	value[7];
};

struct isp_ae_weight_cfg {
	HW_S32		weight[64];
};

struct isp_ae_delay_cfg {
	HW_S32		ae_frame;
	HW_S32		exp_frame;
	HW_S32		gain_frame;
};

struct isp_awb_pub_cfg {
	HW_S32		interval;
	HW_S32		speed;
	HW_S32		stat_sel;
};

struct isp_awb_temp_range_cfg {
	HW_S32		low;
	HW_S32		high;
	HW_S32		base;
};

struct isp_awb_dist_cfg {
	HW_S32		green_zone;
	HW_S32		blue_sky;
};

struct isp_awb_temp_info_cfg {
	HW_S32		number;
	HW_S32		value[320];
};

struct isp_awb_preset_gain_cfg {
	HW_S32		value[22];
};

struct isp_awb_favor_cfg {
	HW_S32		rgain;
	HW_S32		bgain;
};

struct isp_af_vcm_code_cfg {
	HW_S32		min;
	HW_S32		max;
};

struct isp_af_otp_cfg {
	HW_S32		use_otp;
};

struct isp_af_speed_cfg {
	HW_S32		interval_time;
	HW_S32		index;
};

struct isp_af_fine_search_cfg {
	HW_S32		auto_en;
	HW_S32		single_en;
	HW_S32		step;
};

struct isp_af_refocus_cfg {
	HW_S32		move_cnt;
	HW_S32		still_cnt;
	HW_S32		move_monitor_cnt;
	HW_S32		still_monitor_cnt;
};

struct isp_af_tolerance_cfg {
	HW_S32		near_distance;
	HW_S32		far_distance;
	HW_S32		offset;
	HW_S32		table_length;
	HW_S32		std_code_table[20];
	HW_S32		value[20];
};

struct isp_af_scene_cfg {
	HW_S32		stable_min;
	HW_S32		stable_max;
	HW_S32		low_light_lv;
	HW_S32		peak_thres;
	HW_S32		direction_thres;
	HW_S32		change_ratio;
	HW_S32		move_minus;
	HW_S32		still_minus;
	HW_S32		scene_motion_thres;
};

/* isp_3a_param cfg*/
struct isp_3a_param_cfg {
	/* ae */
	struct isp_ae_pub_cfg		ae_pub;					/* id: 0x0200000001 */
	struct isp_ae_table_cfg		ae_preview_tbl;			/* id: 0x0200000002 */
	struct isp_ae_table_cfg		ae_capture_tbl;			/* id: 0x0200000004 */
	struct isp_ae_table_cfg		ae_video_tbl;			/* id: 0x0200000008 */
	struct isp_ae_weight_cfg	ae_win_weight;			/* id: 0x0200000010 */
	struct isp_ae_delay_cfg		ae_delay;				/* id: 0x0200000020 */
	/* awb */
	struct isp_awb_pub_cfg			awb_pub;			/* id: 0x0200000040 */
	struct isp_awb_temp_range_cfg	awb_temp_range;		/* id: 0x0200000080 */
	struct isp_awb_dist_cfg			awb_dist;			/* id: 0x0200000100 */
	struct isp_awb_temp_info_cfg	awb_light_info;		/* id: 0x0200000200 */
	struct isp_awb_temp_info_cfg	awb_ext_light_info;	/* id: 0x0200000400 */
	struct isp_awb_temp_info_cfg	awb_skin_info;		/* id: 0x0200000800 */
	struct isp_awb_temp_info_cfg	awb_special_info;	/* id: 0x0200001000 */
	struct isp_awb_preset_gain_cfg	awb_preset_gain;	/* id: 0x0200002000 */
	struct isp_awb_favor_cfg		awb_favor;			/* id: 0x0200004000 */
	/* af */
	struct isp_af_vcm_code_cfg		af_vcm_code;		/* id: 0x0200008000 */
	struct isp_af_otp_cfg			af_otp;				/* id: 0x0200010000 */
	struct isp_af_speed_cfg			af_speed;			/* id: 0x0200020000 */
	struct isp_af_fine_search_cfg	af_fine_search;		/* id: 0x0200040000 */
	struct isp_af_refocus_cfg		af_refocus;			/* id: 0x0200080000 */
	struct isp_af_tolerance_cfg		af_tolerance;		/* id: 0x0200100000 */
	struct isp_af_scene_cfg			af_scene;			/* id: 0x0200200000 */
};

struct isp_tuning_flash_cfg {
	HW_S32		gain;
	HW_S32		delay_frame;
};

struct isp_tuning_flicker_cfg {
	HW_S32		type;
	HW_S32		ratio;
};

struct isp_tuning_visual_angle_cfg {
	HW_S32		horizontal;
	HW_S32		vertical;
	HW_S32		focus_length;
};

/* global tone mapping */
struct isp_tuning_gtm_cfg {
	HW_S32		type;
	HW_S32		gamma_type;
	HW_S32		auto_alpha_en;
	HW_S32		hist_pix_cnt;
	HW_S32		dark_minval;
	HW_S32		bright_minval;
	HW_S16		plum_var[GTM_LUM_IDX_NUM][GTM_VAR_IDX_NUM];
};

struct isp_tuning_cfa_cfg {
	HW_S32		dir_thres;
};

struct isp_tuning_ctc_cfg {
	HW_U16		min_thres;
	HW_U16		max_thres;
	HW_U16		slope_thres;
	HW_U16		dir_wt;
	HW_U16		dir_thres;
};

struct isp_tuning_blc_gain_cfg {
	HW_S32		value[ISP_RAW_CH_MAX];
};

struct isp_tuning_ccm_cfg {
	HW_U16		temperature;
	struct isp_rgb2rgb_gain_offset	value;
};

struct isp_tuning_pltm_cfg {
	HW_S32		value[ISP_PLTM_MAX];

};

struct isp_tuning_lsc_table_cfg {
	HW_S32		ff_mod;
	HW_S32		center_x;
	HW_S32		center_y;
	HW_S32		rolloff_ratio;
	HW_U16		value[12][768];
	HW_U16		color_temp_triggers[6];
};

struct isp_tuning_gamma_table_cfg {
	HW_S32		number;
	HW_U16		value[5][ISP_GAMMA_TBL_LENGTH];
	HW_U16		lv_triggers[5];
};

struct isp_tuning_linearity_table_cfg {
	HW_U16		value[768];
};

struct isp_tuning_distortion_table_cfg {
	HW_U16		value[512];
};

struct isp_tuning_bdnf_table_cfg {
	HW_U16		thres[ISP_REG_TBL_LENGTH];
};

struct isp_tuning_tdnf_table_cfg {
	HW_U16		thres[ISP_REG_TBL_LENGTH];
	HW_U16		ref_noise[ISP_REG_TBL_LENGTH];
	HW_U8		k_val[ISP_REG_TBL_LENGTH-1];
	HW_U8		diff[256];
#if (ISP_VERSION >= 520)
	HW_U8		d3d_k3d[256];
#endif
};

struct isp_tuning_contrast_table_cfg {
	HW_U16		val[ISP_REG_TBL_LENGTH];
	HW_U16		lum[ISP_REG_TBL_LENGTH];
	HW_U16		pe[128];
};

struct isp_tuning_sharp_table_cfg {
	HW_U16		value[ISP_REG_TBL_LENGTH];
	HW_U16		lum[ISP_REG_TBL_LENGTH];
#if (ISP_VERSION >= 520)
	HW_U16		edge_lum[ISP_REG_TBL_LENGTH];
	HW_U16		hfrq_lum[ISP_REG_TBL_LENGTH];
	HW_U16		hsv[46];
	HW_U8		smap[ISP_REG_TBL_LENGTH];
#endif
};

struct isp_tuning_cem_table_cfg {
	HW_U8		value[ISP_CEM_MEM_SIZE];
};

struct isp_tuning_pltm_table_cfg {
	HW_U8		value[ISP_PLTM_MEM_SIZE];
};

struct isp_tuning_wdr_table_cfg {
	HW_U8       value[ISP_WDR_MEM_SIZE];
};

/* isp_tuning_param cfg */
struct isp_tuning_param_cfg {
	struct isp_tuning_flash_cfg			flash;			/* id: 0x0300000001 */
	struct isp_tuning_flicker_cfg		flicker;		/* id: 0x0300000002 */
	struct isp_tuning_visual_angle_cfg	visual_angle;	/* id: 0x0300000004 */
	struct isp_tuning_gtm_cfg			gtm;			/* id: 0x0300000008 */
	struct isp_tuning_cfa_cfg			cfa;			/* id: 0x0300000010 */
	struct isp_tuning_ctc_cfg			ctc;			/* id: 0x0300000020 */
	struct isp_tuning_blc_gain_cfg		digital_gain;	/* id: 0x0300000040 */
	struct isp_tuning_ccm_cfg			ccm_low;		/* id: 0x0300000080 */
	struct isp_tuning_ccm_cfg			ccm_mid;		/* id: 0x0300000100 */
	struct isp_tuning_ccm_cfg			ccm_high;		/* id: 0x0300000200 */
	struct isp_tuning_pltm_cfg			pltm;			/* id: 0x0300000400 */
	/* tuning tables */
	struct isp_tuning_lsc_table_cfg		lsc;			/* id: 0x0400000001 */
	struct isp_tuning_gamma_table_cfg	gamma;			/* id: 0x0400000002 */
	struct isp_tuning_linearity_table_cfg	linearity;	/* id: 0x0400000004 */
	struct isp_tuning_distortion_table_cfg	distortion;	/* id: 0x0400000008 */
 	struct isp_tuning_bdnf_table_cfg	bdnf;			/* id: 0x0400000010 */
	struct isp_tuning_tdnf_table_cfg	tdnf;			/* id: 0x0400000020 */
	struct isp_tuning_contrast_table_cfg	contrast;	/* id: 0x0400000040 */
	struct isp_tuning_sharp_table_cfg	sharp;			/* id: 0x0400000080 */
	struct isp_tuning_cem_table_cfg		cem;			/* id: 0x0400000100 */
	struct isp_tuning_cem_table_cfg		cem_1;			/* id: 0x0400000200 */
	struct isp_tuning_pltm_table_cfg	pltm_tbl;		/* id: 0x0400000400 */
	struct isp_tuning_wdr_table_cfg		wdr;			/* id: 0x0400000800 */
};

#define ISP_DYNAMIC_GROUP_COUNT 	14

struct isp_dynamic_single_cfg {
	HW_S32		value[ISP_DYNAMIC_GROUP_COUNT];
};

struct isp_dynamic_sharp_item {
	HW_S32		value[ISP_SHARP_MAX];
};

struct isp_dynamic_sharp_cfg {
	HW_S32		trigger;
	struct isp_dynamic_sharp_item	tuning_cfg[ISP_DYNAMIC_GROUP_COUNT];
};

struct isp_dynamic_contrast_item {
	HW_S32		value[ISP_CONTRAST_MAX];
};

struct isp_dynamic_contrast_cfg {
	HW_S32		trigger;
	struct isp_dynamic_contrast_item	tuning_cfg[ISP_DYNAMIC_GROUP_COUNT];
};

struct isp_dynamic_denoise_item {
	HW_S32		value[ISP_DENOISE_MAX];
	HW_S32		color_denoise;
	HW_S32		ratio;
};

struct isp_dynamic_denoise_cfg {
	HW_S32		trigger;
	HW_S32		color_trigger;
	struct isp_dynamic_denoise_item	tuning_cfg[ISP_DYNAMIC_GROUP_COUNT];
};

struct isp_dynamic_sensor_offset_item {
	HW_S32		value[ISP_SO_MAX];
};

struct isp_dynamic_sensor_offset_cfg {
	HW_S32		trigger;
	struct isp_dynamic_sensor_offset_item	tuning_cfg[ISP_DYNAMIC_GROUP_COUNT];
};

struct isp_dynamic_black_level_item {
	HW_S32		value[ISP_BLC_MAX];
};

struct isp_dynamic_black_level_cfg {
	HW_S32		trigger;
	struct isp_dynamic_black_level_item	tuning_cfg[ISP_DYNAMIC_GROUP_COUNT];
};

struct isp_dynamic_dpc_item {
	HW_S32		value[ISP_DPC_MAX];
};

struct isp_dynamic_dpc_cfg {
	HW_S32		trigger;
	struct isp_dynamic_dpc_item	tuning_cfg[ISP_DYNAMIC_GROUP_COUNT];
};

struct isp_dynamic_pltm_item {
	HW_S32		value[ISP_PLTM_DYNAMIC_MAX];
};

struct isp_dynamic_pltm_cfg {
	HW_S32		trigger;
	struct isp_dynamic_pltm_item	tuning_cfg[ISP_DYNAMIC_GROUP_COUNT];
};

struct isp_dynamic_defog_item {
	HW_S32		value;
};

struct isp_dynamic_defog_cfg {
	HW_S32		trigger;
	struct isp_dynamic_defog_item	tuning_cfg[ISP_DYNAMIC_GROUP_COUNT];
};

struct isp_dynamic_cem_item {
	HW_S32		value;
};

struct isp_dynamic_cem_cfg {
	HW_S32		trigger;
	struct isp_dynamic_cem_item	tuning_cfg[ISP_DYNAMIC_GROUP_COUNT];
};

struct isp_dynamic_histogram_item {
	HW_S32		brightness;
	HW_S32		contrast;
};
struct isp_dynamic_histogram_cfg {
	HW_S32		brightness_trigger;
	HW_S32		contrast_trigger;
	struct isp_dynamic_histogram_item	tuning_cfg[ISP_DYNAMIC_GROUP_COUNT];
};

struct isp_dynamic_saturation_item {
	HW_S32		cb;
	HW_S32		cr;
	HW_S32		value[ISP_SATURATION_MAX];
};

struct isp_dynamic_saturation_cfg {
	HW_S32		trigger;
	struct isp_dynamic_saturation_item	tuning_cfg[ISP_DYNAMIC_GROUP_COUNT];
};

struct isp_dynamic_tdf_item {
	HW_S32		value[ISP_TDF_MAX];
};

struct isp_dynamic_tdf_cfg {
	HW_S32		trigger;
	struct isp_dynamic_tdf_item	tuning_cfg[ISP_DYNAMIC_GROUP_COUNT];
};

struct isp_dynamic_ae_item {
	HW_S32		value[ISP_EXP_CFG_MAX];
};

struct isp_dynamic_ae_cfg {
	HW_S32		trigger;
	struct isp_dynamic_ae_item	tuning_cfg[ISP_DYNAMIC_GROUP_COUNT];
};

struct isp_dynamic_gtm_item {
	HW_S32		value[ISP_GTM_HEQ_MAX];
};

struct isp_dynamic_gtm_cfg {
	HW_S32		trigger;
	struct isp_dynamic_gtm_item	tuning_cfg[ISP_DYNAMIC_GROUP_COUNT];
};


/* isp_dynamic_param cfg */
struct isp_dynamic_param_cfg {
	struct isp_dynamic_single_cfg		lum_mapping_point;	/* id: 0x0500000001 */
	struct isp_dynamic_single_cfg		gain_mapping_point;	/* id: 0x0500000002 */
	struct isp_dynamic_sharp_cfg		sharp;				/* id: 0x0500000004 */
	struct isp_dynamic_contrast_cfg		contrast;			/* id: 0x0500000008 */
	struct isp_dynamic_denoise_cfg		denoise;			/* id: 0x0500000010 */
	struct isp_dynamic_sensor_offset_cfg	sensor_offset;	/* id: 0x0500000020 */
	struct isp_dynamic_black_level_cfg	black_level;		/* id: 0x0500000040 */
	struct isp_dynamic_dpc_cfg			dpc;				/* id: 0x0500000080 */
	struct isp_dynamic_pltm_cfg			pltm;				/* id: 0x0500000100 */
	struct isp_dynamic_defog_cfg		defog;				/* id: 0x0500000200 */
	struct isp_dynamic_histogram_cfg	histogram;			/* id: 0x0500000400 */
	struct isp_dynamic_saturation_cfg	saturation;			/* id: 0x0500000800 */
	struct isp_dynamic_cem_cfg			cem;				/* id: 0x0500001000 */
	struct isp_dynamic_tdf_cfg			tdf;				/* id: 0x0500002000 */
	struct isp_dynamic_ae_cfg			ae;					/* id: 0x0500004000 */
	struct isp_dynamic_gtm_cfg			gtm;				/* id: 0x0500008000 */
};

struct isp_params_cfg {
	struct isp_test_param_cfg			isp_test_param;
	struct isp_3a_param_cfg				isp_3a_param;
	struct isp_tuning_param_cfg			isp_tuning_param;
	struct isp_dynamic_param_cfg		isp_dynamic_param;
};

/*
 * get isp cfg
 * @isp: hw isp device
 * @group_id: group id
 * @cfg_ids: cfg ids, supports one or more ids combination by '|'(bit operation)
 * @cfg_data: cfg data to get, data sorted by @cfg_ids
 * @returns: cfg data length, negative if something went wrong
 */
HW_S32 isp_tuning_get_cfg(struct hw_isp_device *isp, HW_U8 group_id, HW_U32 cfg_ids, void *cfg_data);

/*
 * set isp cfg
 * @isp: hw isp device
 * @group_id: group id
 * @cfg_id: cfg ids, supports one or more ids combination by '|'(bit operation)
 * @cfg_data: cfg data to set, data sorted by @cfg_ids
 * @returns: cfg data length, negative if something went wrong
 */
HW_S32 isp_tuning_set_cfg(struct hw_isp_device *isp, HW_U8 group_id, HW_U32 cfg_ids, void *cfg_data);
int isp_config_sensor_info(struct hw_isp_device *isp);
int isp_params_parse(struct hw_isp_device *isp, struct isp_param_config *params, int sync_mode);
int isp_tuning_reset(struct hw_isp_device *isp, struct isp_param_config *param);

int isp_tuning_update(struct hw_isp_device *isp);
struct isp_tuning * isp_tuning_init(struct hw_isp_device *isp, const struct isp_param_config *params);
void isp_tuning_exit(struct hw_isp_device *isp);



#endif /* _ISP_TUNING_CFG_H_ */
