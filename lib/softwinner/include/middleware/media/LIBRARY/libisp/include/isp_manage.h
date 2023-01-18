
/*
 ******************************************************************************
 *
 * isp_algo.h
 *
 * Hawkview ISP - isp_algo.h module
 *
 * Copyright (c) 2016 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Version		  Author         Date		    Description
 *
 *   3.0		  Yang Feng   	2016/04/08	VIDEO INPUT
 *
 *****************************************************************************
 */

#ifndef __BSP__ISP__ALGO__H
#define __BSP__ISP__ALGO__H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>

#include "isp_type.h"
#include "isp_module_cfg.h"
#include "isp_3a_af.h"
#include "isp_3a_afs.h"
#include "isp_3a_md.h"
#include "isp_3a_awb.h"
#include "isp_3a_ae.h"
#include "isp_tone_mapping.h"
#include "isp_iso_config.h"
#include "isp_base.h"
#include "isp_rolloff.h"
#include "isp_pltm.h"

#include "../isp_tuning/isp_tuning_priv.h"

extern unsigned int isp_lib_log_param;

#if (ISP_VERSION == 521)
#define ISP_AE_ROW	16
#define ISP_AE_COL	24

#define ISP_AWB_ROW	32
#define ISP_AWB_COL	32
#else
#define ISP_AE_ROW	32
#define ISP_AE_COL	48

#define ISP_AWB_ROW	32
#define ISP_AWB_COL	48
#endif

#define ISP_AF_ROW	8
#define ISP_AF_COL	8

#define ISP_HIST_NUM	256
#define ISP_AFS_NUM	128

#define ISP_MD_ROW	ISP_AE_ROW
#define ISP_MD_COL	ISP_AE_COL

#define ISP_GTM_ROW	ISP_AE_ROW
#define ISP_GTM_COL	ISP_AE_COL

#define ISP_PLTM_ROW	24
#define ISP_PLTM_COL	32

#define SATURATION_MAX	150

#define LSC_PIC_SIZE	4096

#define H3A_PIC_OFFSET	-1000
#define H3A_PIC_SIZE	2000

#define ISP_AF_START_FRAME_NUM 3
#define ISP_AWB_START_FRAME_NUM 3

enum colorfx {
	ISP_COLORFX_NONE = 0,
	ISP_COLORFX_GRAY,
	ISP_COLORFX_NEGATIVE,
	ISP_COLORFX_ANTIQUE,
	ISP_COLORFX_RTONE,
	ISP_COLORFX_GTONE,
	ISP_COLORFX_BTONE,
};

struct isp_stat_buf_s {
	void *ae_buf;
	void *af_buf;
	void *awb_buf;
	void *hist_buf;
	void *afs_buf;
	void *pltm_buf;
};

struct isp_ae_stats_s {
	HW_U32 win_pix_n;
	HW_U32 accum_r[ISP_AE_ROW][ISP_AE_COL];
	HW_U32 accum_g[ISP_AE_ROW][ISP_AE_COL];
	HW_U32 accum_b[ISP_AE_ROW][ISP_AE_COL];
	HW_U32 avg[ISP_AE_ROW*ISP_AE_COL];

	HW_U32 hist[ISP_HIST_NUM];
};

struct hw_isp_wb_gain {
	HW_U16 r_gain;
	HW_U16 gr_gain;
	HW_U16 gb_gain;
	HW_U16 b_gain;
};

struct isp_awb_stats_s {
	HW_U32 awb_sum_r[ISP_AWB_ROW][ISP_AWB_COL];
	HW_U32 awb_sum_g[ISP_AWB_ROW][ISP_AWB_COL];
	HW_U32 awb_sum_b[ISP_AWB_ROW][ISP_AWB_COL];
	HW_U32 awb_sum_cnt[ISP_AWB_ROW][ISP_AWB_COL];

	HW_U32 awb_avg_r[ISP_AWB_ROW][ISP_AWB_COL];	/*range 0~2048*/
	HW_U32 awb_avg_g[ISP_AWB_ROW][ISP_AWB_COL];
	HW_U32 awb_avg_b[ISP_AWB_ROW][ISP_AWB_COL];
	HW_U32 avg[ISP_AWB_ROW][ISP_AWB_COL];
};

struct isp_af_stats_s {
	HW_U32 af_count[ISP_AF_ROW][ISP_AF_COL];
	HW_U32 af_h_d1[ISP_AF_ROW][ISP_AF_COL];
	HW_U32 af_h_d2[ISP_AF_ROW][ISP_AF_COL];
	HW_U32 af_v_d1[ISP_AF_ROW][ISP_AF_COL];
	HW_U32 af_v_d2[ISP_AF_ROW][ISP_AF_COL];
};

struct isp_afs_stats_s {
	HW_U32 pic_width;
	HW_U32 pic_height;
	HW_U32 afs_sum[ISP_AFS_NUM];
};

struct isp_md_stats_s {
	HW_U32 win_pix_n;
	HW_U32 accum_r[ISP_AE_ROW][ISP_AE_COL];
	HW_U32 accum_g[ISP_AE_ROW][ISP_AE_COL];
	HW_U32 accum_b[ISP_AE_ROW][ISP_AE_COL];
	HW_U32 avg[ISP_AE_ROW*ISP_AE_COL];

	HW_U32 hist[ISP_HIST_NUM];
};

struct isp_gtm_stats_s {
	HW_U32 win_pix_n;
	HW_U32 accum_r[ISP_AE_ROW][ISP_AE_COL];
	HW_U32 accum_g[ISP_AE_ROW][ISP_AE_COL];
	HW_U32 accum_b[ISP_AE_ROW][ISP_AE_COL];
	HW_U32 avg[ISP_AE_ROW*ISP_AE_COL];

	HW_U32 hist[ISP_HIST_NUM];
};

struct isp_dynamic_judge_stats_s {
	HW_U32 accum[ISP_AE_ROW][ISP_AE_COL];
	HW_U32 accum_last1[ISP_AE_ROW][ISP_AE_COL];
	HW_U32 accum_last2[ISP_AE_ROW][ISP_AE_COL];
	HW_U32 accum_last3[ISP_AE_ROW][ISP_AE_COL];
	HW_S32 mov_save[7];
	HW_S32 mov_th[6];
	HW_S32 tdnf_comp[STATUS_JUDGE_MAX];
	HW_S32 tdnf_diff_comp[STATUS_JUDGE_MAX];
	HW_S32 lp_th_ratio_comp[STATUS_JUDGE_MAX];
	HW_S32 sharp_hfrq_comp[STATUS_JUDGE_MAX];
	HW_S32 sharp_edge_comp[STATUS_JUDGE_MAX];
	HW_S32 sharp_under_shoot_comp[STATUS_JUDGE_MAX];
	HW_S32 tdnf_comp_target;
	HW_S32 tdnf_diff_comp_target;
	HW_S32 lp_th_ratio_comp_target;
	HW_S32 sharp_hfrq_comp_target;
	HW_S32 sharp_edge_comp_target;
	HW_S32 sharp_under_shoot_comp_target;
	HW_S32 mov;
	HW_S32 mov_old;
	bool enable;
};

struct isp_pltm_stats_s {
	HW_U16 avg_before_pltm;
	HW_U16 avg_after_pltm;
	HW_U16 min_before_pltm;
	HW_U16 max_before_pltm;
	HW_U16 min_after_pltm;
	HW_U16 max_after_pltm;
	HW_U16 lst[ISP_PLTM_ROW * ISP_PLTM_COL];
};

struct isp_stats_s {
	struct isp_ae_stats_s ae_stats;
	struct isp_awb_stats_s awb_stats;
	struct isp_af_stats_s af_stats;
	struct isp_afs_stats_s afs_stats;
	struct isp_pltm_stats_s pltm_stats;
};

typedef struct isp_af_entity_context {
	af_param_t *af_param;
	af_stats_t af_stats;
	af_result_t af_result;
	isp_af_core_ops_t *ops;
	void *af_entity;
} isp_af_entity_context_t;


typedef struct isp_afs_entity_context {
	afs_param_t *afs_param;
	afs_stats_t afs_stats;
	afs_result_t afs_result;
	isp_afs_core_ops_t *ops;
	void *afs_entity;
} isp_afs_entity_context_t;

typedef struct isp_md_entity_context {
	md_param_t *md_param;
	md_stats_t md_stats;
	md_result_t md_result;
	isp_md_core_ops_t *ops;
	void *md_entity;
} isp_md_entity_context_t;

typedef struct isp_awb_entity_context {
	awb_param_t *awb_param;
	awb_stats_t awb_stats;
	awb_result_t awb_result;
	isp_awb_core_ops_t *ops;
	void *awb_entity;
} isp_awb_entity_context_t;

typedef struct isp_ae_entity_context {
	ae_param_t *ae_param;
	ae_stats_t ae_stats;
	ae_result_t ae_result;
	isp_ae_core_ops_t *ops;
	void *ae_entity;
} isp_ae_entity_context_t;

typedef struct isp_gtm_entity_context {
	gtm_param_t *gtm_param;
	gtm_stats_t gtm_stats;
	gtm_result_t gtm_result;
	isp_gtm_core_ops_t *ops;
	void *gtm_entity;
} isp_gtm_entity_context_t;

typedef struct isp_pltm_entity_context {
	pltm_param_t *pltm_param;
	pltm_stats_t pltm_stats;
	pltm_result_t pltm_result;
	isp_pltm_core_ops_t *ops;
	void *pltm_entity;
} isp_pltm_entity_context_t;

typedef struct isp_iso_entity_context {
	iso_param_t *iso_param;
	iso_result_t iso_result;
	isp_iso_core_ops_t *ops;
	void *iso_entity;
} isp_iso_entity_context_t;

typedef struct isp_rolloff_entity_context {
	rolloff_param_t *rolloff_param;
	rolloff_stats_t rolloff_stats;
	rolloff_result_t rolloff_result;
	isp_rolloff_core_ops_t *ops;
	void *rolloff_entity;
} isp_rolloff_entity_context_t;

typedef struct isp_defog_context {
	HW_S32 min_rgb_pre[8];
	HW_S32 defog_pre;
	HW_S32 defog_changed;
} isp_defog_context_t;

struct isp_driver_to_3a_stat {
	struct isp_size pic_size;

	HW_S32 min_rgb_saved;
	HW_S32 c_noise_saved;
};


/*
 *
 *   struct auto_focus_settings - Stores the auto focuse related settings.
 *
 */
typedef struct isp_af_settings {
	HW_S32 focus_absolute;
	HW_S32 focus_relative;
	enum auto_focus_run_mode af_mode;
	enum auto_focus_metering_mode af_metering_mode;
	enum auto_focus_range_new af_range;
	bool focus_lock;
	struct isp_h3a_coor_win af_coor;
} isp_af_settings_t;

/*
 *
 *   struct white_balance_settings - Stores the white balance related settings.
 *
 */
struct white_balance_settings {
	enum white_balance_mode wb_mode;
	HW_S32 wb_temperature;
	bool white_balance_lock;
	struct isp_wb_gain wb_gain_manual;
};

struct drc_gen_ctrl {
	HW_U32 pic_size;
	HW_U32 pic_avg;
	HW_U32 hi_cnt;
};


struct isp_stats_context {
	HW_U32 pic_w;
	HW_U32 pic_h;

	struct isp_stats_s stats;
	struct isp_wb_gain wb_gain_saved;
	struct isp_dynamic_judge_stats_s dynamic_stats;
	bool enabled;
};
struct gain_cfg {
	HW_S32 gain_favor;
	HW_S32 ana_gain_min;
	HW_S32 ana_gain_max;
	HW_S32 dig_gain_min;
	HW_S32 dig_gain_max;
};
typedef struct isp_tune_setting {
	HW_S32 contrast_level;
	HW_S32 saturation_level;
	HW_S32 sharpness_level;
	HW_S32 brightness_level;
	HW_S32 denoise_level;
	HW_S32 hue_level;
	HW_S32 pltmwdr_level;
	HW_S32 tdf_level;
	HW_S32 highlight_level;
	HW_S32 backlight_level;
	struct gain_cfg gains;
	enum colorfx effect;
} isp_tune_setting_t;

typedef struct isp_adjust_setting {
	HW_S32 contrast;
	HW_S32 brightness;
	HW_S32 defog_value;
} isp_adjust_setting_t;

/*
 *
 *   struct isp_lib_context - Stores the isp settings
 *   also stores the stat buffer for 3a algorithms.
 */
struct isp_lib_context {
	HW_S32 isp_id;
	FILE *stat_log_fd;
	FILE *isp_log_fd;
	/* ISP settings changed flags */
	HW_U32 isp_3a_change_flags;

	/* Auto Exposure settings */
	isp_ae_settings_t ae_settings;

	/* Auto focus settings */
	isp_af_settings_t af_settings;

	/* Auto White balance settings */
	isp_awb_setting_t awb_settings;

	isp_tune_setting_t tune;
	isp_adjust_setting_t adjust;

	HW_U64 awb_frame_cnt;
	HW_U64 ae_frame_cnt;
	HW_U64 af_frame_cnt;
	HW_U64 gtm_frame_cnt;
	HW_U64 md_frame_cnt;
	HW_U64 afs_frame_cnt;
	HW_U64 iso_frame_cnt;
	HW_U64 rolloff_frame_cnt;

	HW_U64 alg_frame_cnt;
	/* 3A statistic buffers and other values*/
	struct isp_driver_to_3a_stat stat;
	isp_sensor_info_t sensor_info;

	/* ISP module config */
	struct isp_module_config module_cfg;

	struct isp_param_config isp_ini_cfg;
	isp_defog_context_t defog_ctx;

	struct isp_stats_context stats_ctx;
	//AF Ctx
	isp_af_entity_context_t af_entity_ctx;
	//AFS Ctx
	isp_afs_entity_context_t afs_entity_ctx;
	//MD Ctx
	isp_md_entity_context_t md_entity_ctx;
	//AWB Ctx
	isp_awb_entity_context_t awb_entity_ctx;
	//AE Ctx
	isp_ae_entity_context_t ae_entity_ctx;
	//GTM Ctx
	isp_gtm_entity_context_t gtm_entity_ctx;
	//PLTM Ctx
	isp_pltm_entity_context_t pltm_entity_ctx;
	//ISO Ctx
	isp_iso_entity_context_t iso_entity_ctx;
	//ISO Rolloff
	isp_rolloff_entity_context_t rolloff_entity_ctx;

	const struct isp_ctx_operations *ops;
	pthread_mutex_t ctx_lock;
	void *isp_stat_buf;
	void *load_reg_base;
#if (ISP_VERSION == 520 || ISP_VERSION == 521)
	/* WDR_BE */
	//HW_U16 wdr_be_tbl_save[ISP_WDR_GAMMA_BE_MEM_SIZE/2];
	//HW_U16 gamma_tbl_save[ISP_GAMMA_TBL_LENGTH];
	HW_U16 anti_gamma_tbl[4096];
#endif
};

/*
 *   Flags raised when a setting is changed.
 */
enum e3a_settings_flags {
	ISP_SET_SCENE_MODE = 1 << 0,
	ISP_SET_AWB_MODE = 1 << 1,
	ISP_SET_FLICKER_MODE = 1 << 2,
	ISP_SET_SHARPNESS = 1 << 3,
	ISP_SET_BRIGHTNESS = 1 << 4,
	ISP_SET_SATURATION = 1 << 5,
	ISP_SET_EFFECT = 1 << 6,
	ISP_SET_AF_METERING_MODE = 1 << 7,
	ISP_SET_AE_METERING_MODE = 1 << 8,
	ISP_SET_CONTRAST = 1 << 9,
	ISP_SET_HUE = 1 << 10,
	ISP_SET_GAIN_STR = 1 << 11,
	ISP_SETTING_MAX,

	/* all possible flags raised */
	ISP_SETTINGS_ALL = (((ISP_SETTING_MAX -1 ) << 1) -1 ),
};

/*
 *  ISP Module API
 */
struct isp_ctx_operations {
	void (*ae_done)(struct isp_lib_context *isp, ae_result_t *result);
	void (*af_done)(struct isp_lib_context *isp, af_result_t *result);
	void (*awb_done)(struct isp_lib_context *isp, awb_result_t *result);
	void (*afs_done)(struct isp_lib_context *isp, afs_result_t *result);
	void (*md_done)(struct isp_lib_context *isp, md_result_t *result);
	void (*pltm_done)(struct isp_lib_context *isp, pltm_result_t *result);
};

void isp_rolloff_set_params_helper(isp_rolloff_entity_context_t *isp_rolloff_cxt, rolloff_param_type_t cmd_type);
void isp_afs_set_params_helper(isp_afs_entity_context_t *isp_afs_cxt, afs_param_type_t cmd_type);
void isp_iso_set_params_helper(isp_iso_entity_context_t *isp_iso_cxt, iso_param_type_t cmd_type);
void isp_md_set_params_helper(isp_md_entity_context_t *isp_md_cxt, md_param_type_t cmd_type);
void isp_af_set_params_helper(isp_af_entity_context_t *isp_af_cxt, af_param_type_t cmd_type);
void isp_awb_set_params_helper(isp_awb_entity_context_t *isp_awb_cxt, awb_param_type_t cmd_type);
void isp_ae_set_params_helper(isp_ae_entity_context_t *isp_ae_cxt, ae_param_type_t cmd_type);
void isp_gtm_set_params_helper(isp_gtm_entity_context_t *isp_gtm_cxt, gtm_param_type_t cmd_type);
void isp_pltm_set_params_helper(isp_pltm_entity_context_t *isp_pltm_cxt, pltm_param_type_t cmd_type);

HW_S32 isp_ctx_algo_init(struct isp_lib_context *isp_gen, const struct isp_ctx_operations *ops);
HW_S32 isp_ctx_algo_exit(struct isp_lib_context *isp_gen);
HW_S32 isp_ctx_stats_prepare(struct isp_lib_context *isp_gen, void *buffer);
HW_S32 isp_ctx_stats_prepare_sync(struct isp_lib_context *isp_gen, void *buffer0, void *buffer1);
HW_S32 isp_ctx_stats_req(struct isp_lib_context *isp_gen, struct isp_stats_context *stats_ctx);
HW_S32 isp_ctx_config_init(struct isp_lib_context *isp_gen);
HW_S32 isp_ctx_config_exit(struct isp_lib_context *isp_gen);
HW_S32 isp_ctx_update_ae_tbl(struct isp_lib_context *isp_gen, int sensor_fps);
HW_S32 isp_ctx_config_update(struct isp_lib_context *isp_gen);
HW_S32 isp_ctx_config_reset(struct isp_lib_context *isp_gen);
HW_S32 isp_ctx_algo_run(struct isp_lib_context *isp_gen);

int isp_stat_save_init(struct isp_lib_context *ctx);
void isp_stat_save_exit(struct isp_lib_context *ctx);
void isp_stat_save_run(struct isp_lib_context *ctx);
int isp_ctx_save_init(struct isp_lib_context *ctx);
int isp_ctx_save_exit(struct isp_lib_context *ctx);
void isp_log_save_init(struct isp_lib_context *ctx);
int isp_log_save_run(struct isp_lib_context *ctx);
void isp_log_save_exit(struct isp_lib_context *ctx);

#endif //__BSP__ISP__ALGO__H


