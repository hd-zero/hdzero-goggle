
/*
 ******************************************************************************
 *
 * isp_tuning.h
 *
 * Hawkview ISP - isp_tuning.h module
 *
 * Copyright (c) 2016 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Version		  Author         Date		    Description
 *
 *   3.0		  Yang Feng   	2016/05/09	VIDEO INPUT
 *
 *****************************************************************************
 */

#ifndef _ISP_TUNING_H_
#define _ISP_TUNING_H_
#include "../include/isp_type.h"
#include "../include/isp_comm.h"

struct isp_tuning;
struct hw_isp_device;
struct isp_ae_stats_s;

struct isp_test_param {
	/*isp test param */
	HW_S32 isp_test_mode;

	HW_S32 isp_test_exptime;
	HW_S32 exp_line_start;
	HW_S32 exp_line_step;
	HW_S32 exp_line_end;
	HW_S32 exp_change_interval;

	HW_S32 isp_test_gain;
	HW_S32 gain_start;
	HW_S32 gain_step;
	HW_S32 gain_end;
	HW_S32 gain_change_interval;

	HW_S32 isp_test_focus;
	HW_S32 focus_start;
	HW_S32 focus_step;
	HW_S32 focus_end;
	HW_S32 focus_change_interval;

	HW_S32 isp_log_param;
	HW_S32 isp_gain;
	HW_S32 isp_exp_line;
	HW_S32 isp_color_temp;
	HW_S32 ae_forced;
	HW_S32 lum_forced;

	/*isp enable param */
	HW_S32 manual_en;
	HW_S32 afs_en;
	HW_S32 sharp_en;
	HW_S32 contrast_en;
	HW_S32 denoise_en;
	HW_S32 drc_en;
	HW_S32 cem_en;
	HW_S32 lsc_en;
	HW_S32 gamma_en;
	HW_S32 cm_en;
	HW_S32 ae_en;
	HW_S32 af_en;
	HW_S32 awb_en;
	HW_S32 hist_en;
	HW_S32 blc_en;
	HW_S32 so_en;
	HW_S32 wb_en;
	HW_S32 otf_dpc_en;
	HW_S32 cfa_en;
	HW_S32 tdf_en;
	HW_S32 cnr_en;
	HW_S32 satur_en;
	HW_S32 defog_en;
	HW_S32 linear_en;
	HW_S32 gtm_en;
	HW_S32 dig_gain_en;
	HW_S32 pltm_en;
	HW_S32 wdr_en;
	HW_S32 ctc_en;
};

struct isp_3a_param {
	/*isp ae param */
	HW_S32 define_ae_table;
	HW_S32 ae_max_lv;
	HW_S32 ae_table_preview_length;
	HW_S32 ae_table_capture_length;
	HW_S32 ae_table_video_length;
	HW_S32 ae_table_preview[42];
	HW_S32 ae_table_capture[42];
	HW_S32 ae_table_video[42];
	HW_S32 ae_win_weight[64];
	HW_S32 ae_gain_favor;
	HW_S32 ae_gain_range[4];
	HW_S32 ae_hist_mod_en;
	HW_S32 ae_hist_sel;
	HW_S32 ae_stat_sel;
	HW_S32 ae_ki;
	HW_S32 ae_ConvDataIndex;
	HW_S32 ae_blowout_pre_en;
	HW_S32 ae_blowout_attr;
	HW_S32 ae_delay_frame;
	HW_S32 exp_delay_frame;
	HW_S32 gain_delay_frame;
	HW_S32 exp_comp_step;
	HW_S32 ae_touch_dist_ind;
	HW_S32 ae_iso2gain_ratio;
	HW_S32 ae_fno_step[16];
	HW_S32 wdr_cfg[ISP_WDR_CFG_MAX];
	//HW_S32 ae_table_length[SCENE_MODE_MAX];

	/*isp awb param */
	HW_S32 awb_interval;
	HW_S32 awb_speed;
	HW_S32 awb_stat_sel;
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

	/*isp af param */
	HW_S32 af_use_otp;
	HW_S32 vcm_min_code;
	HW_S32 vcm_max_code;
	HW_S32 af_interval_time;
	HW_S32 af_speed_ind; //0~5
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
};

struct isp_dynamic_config {
	HW_S32 sharp_cfg[ISP_SHARP_MAX];
	HW_S32 contrast_cfg[ISP_CONTRAST_MAX];
	HW_S32 denoise_cfg[ISP_DENOISE_MAX];
	HW_S32 sensor_offset[ISP_SO_MAX];
	HW_S32 black_level[ISP_BLC_MAX];
	HW_S32 dpc_cfg[ISP_DPC_MAX];
	HW_S32 pltm_dynamic_cfg[ISP_PLTM_DYNAMIC_MAX];
	HW_S32 defog_value;
	HW_S32 brightness;
	HW_S32 contrast;
	HW_S32 saturation_cb;
	HW_S32 saturation_cr;
	HW_S32 saturation_cfg[ISP_SATURATION_MAX];
	HW_S32 cem_ratio;
	HW_S32 tdf_cfg[ISP_TDF_MAX];
	HW_S32 color_denoise;
	HW_S32 ae_cfg[ISP_EXP_CFG_MAX];
	HW_S32 gtm_cfg[ISP_GTM_HEQ_MAX];

	//HW_S32 reserved[0];
};

enum isp_triger_type {
	ISP_TRIGER_BY_LUM_IDX, //ISP_TRIGER_BY_ISO_IDX
	ISP_TRIGER_BY_GAIN_IDX,
	/*ISP_TRIGER_BY_COLOR_TEMPERATURE,*/

	ISP_TRIGER_MAX,
};

typedef struct isp_param_triger {
	enum isp_triger_type sharp_triger;
	enum isp_triger_type contrast_triger;
	enum isp_triger_type denoise_triger;
	enum isp_triger_type sensor_offset_triger;
	enum isp_triger_type black_level_triger;
	enum isp_triger_type dpc_triger;
	enum isp_triger_type defog_value_triger;
	enum isp_triger_type pltm_dynamic_triger;
	enum isp_triger_type brightness_triger;
	enum isp_triger_type gcontrast_triger;
	enum isp_triger_type saturation_triger;
	enum isp_triger_type cem_ratio_triger;
	enum isp_triger_type tdf_triger;
	enum isp_triger_type color_denoise_triger;
	enum isp_triger_type ae_cfg_triger;
	enum isp_triger_type gtm_cfg_triger;
} isp_dynamic_triger_t;

struct isp_dynamic_param {
	/*isp denoise param */
	isp_dynamic_triger_t triger;
	HW_S32 isp_lum_mapping_point[14];
	HW_S32 isp_gain_mapping_point[14];
	struct isp_dynamic_config isp_dynamic_cfg[14];
};

struct isp_tunning_param {
	HW_S32 flash_gain;
	HW_S32 flash_delay_frame;
	HW_S32 flicker_type;
	HW_S32 flicker_ratio;
	HW_S32 hor_visual_angle;
	HW_S32 ver_visual_angle;
	HW_S32 focus_length;
	HW_S32 gamma_num;
	HW_S32 rolloff_ratio;

	/*isp gtm param */
	HW_S32 gtm_type;
	HW_S32 gamma_type;
	HW_S32 auto_alpha_en;
	HW_S32 hist_pix_cnt;
	HW_S32 dark_minval;
	HW_S32 bright_minval;
	HW_S16 plum_var[GTM_LUM_IDX_NUM][GTM_VAR_IDX_NUM];

	/*cfa param*/
	HW_S32 cfa_dir_th;

	/*cross talk param*/
	HW_U16 ctc_th_max;
	HW_U16 ctc_th_min;
	HW_U16 ctc_th_slope;
	HW_U16 ctc_dir_wt;
	HW_U16 ctc_dir_th;

	/*isp tune param */
	HW_S32 bayer_gain[ISP_RAW_CH_MAX];

	HW_S32 ff_mod;
	HW_S32 lsc_center_x;
	HW_S32 lsc_center_y;
	HW_U16 lsc_tbl[12][768];
	HW_U16 lsc_trig_cfg[6]; //Color temp trigger points

	HW_U16 gamma_tbl_ini[5][ISP_GAMMA_TBL_LENGTH]; // form bin file
	HW_U16 gamma_trig_cfg[5]; //LV trigger points

	HW_U16 linear_tbl[768];
	HW_U16 disc_tbl[512];
	struct isp_rgb2rgb_gain_offset color_matrix_ini[3];
	HW_U16 cm_trig_cfg[3]; //Color temp trigger points

	HW_S32 pltm_cfg[ISP_PLTM_MAX];

	HW_U16 isp_bdnf_th[ISP_REG_TBL_LENGTH];
	HW_U16 isp_tdnf_th[ISP_REG_TBL_LENGTH];
	HW_U16 isp_tdnf_ref_noise[ISP_REG_TBL_LENGTH];
	HW_U8 isp_tdnf_k[ISP_REG_TBL_LENGTH-1];
	HW_U16 isp_contrast_val[ISP_REG_TBL_LENGTH];
	HW_U16 isp_contrast_lum[ISP_REG_TBL_LENGTH];
	HW_U16 isp_sharp_val[ISP_REG_TBL_LENGTH];
	HW_U16 isp_sharp_lum[ISP_REG_TBL_LENGTH];
#if (ISP_VERSION >= 520)
	HW_U16 isp_sharp_edge_lum[ISP_REG_TBL_LENGTH];
	HW_U16 isp_sharp_hfrq_lum[ISP_REG_TBL_LENGTH];
	HW_U16 isp_sharp_hsv[46];
	HW_U8 isp_sharp_s_map[ISP_REG_TBL_LENGTH];
	HW_U8 isp_d3d_k3d_incre_curve[256];
#endif
	HW_U8 isp_tdnf_diff[256];
	HW_U16 isp_contrat_pe[128];
 	HW_U8 isp_cem_table[ISP_CEM_MEM_SIZE];
	HW_U8 isp_cem_table1[ISP_CEM_MEM_SIZE];
 	HW_U8 isp_pltm_table[ISP_PLTM_MEM_SIZE];
	HW_U32 isp_wdr_table[ISP_WDR_MEM_SIZE];
};

struct isp_param_config {
	struct isp_test_param isp_test_settings;
	struct isp_3a_param isp_3a_settings;
	struct isp_tunning_param isp_tunning_settings;
	struct isp_dynamic_param isp_iso_settings;
};

#endif /*_ISP_TUNING_H_*/

