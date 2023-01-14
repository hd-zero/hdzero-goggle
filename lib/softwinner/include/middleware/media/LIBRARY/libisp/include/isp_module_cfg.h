
/*
 ******************************************************************************
 *
 * isp_module_cfg.h
 *
 * Hawkview ISP - isp_module_cfg.h module
 *
 * Copyright (c) 2016 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Version		  Author         Date		    Description
 *
 *   3.0		  Yang Feng   	2016/03/31	VIDEO INPUT
 *
 *****************************************************************************
 */

#ifndef __ISP__MODULE__CFG__H
#define __ISP__MODULE__CFG__H

#include "isp_comm.h"
#include "isp_type.h"
#include "isp_debug.h"

/* ISP module config */
/* TABLE */

#define ISP_LUT_TBL_SIZE	256
#define ISP_LENS_TBL_SIZE	256
#define ISP_DRC_TBL_SIZE	256

/*
 *  struct isp_drc_config - .
 */
struct isp_drc_config {
 	HW_U16 drc_table[ISP_DRC_TBL_SIZE];
	HW_U16 drc_table_last[ISP_DRC_TBL_SIZE];
};

/*
 *  struct isp_cem_config - .
 */
struct isp_cem_config {
 	HW_U8 cem_table[ISP_CEM_MEM_SIZE];
};

/*
 *  struct isp_lut_lens_gamma_config - .
 */

struct isp_lut_config {
	HW_S32 lut_num;
 	HW_U8 lut_dpc_src0_table[ISP_LUT_TBL_SIZE*4];
	HW_U8 lut_dpc_src1_table[ISP_LUT_TBL_SIZE*4];
};

struct isp_lens_config {
	struct isp_lsc_config lsc_cfg;
	HW_U16 lens_r_table[ISP_LENS_TBL_SIZE];
	HW_U16 lens_g_table[ISP_LENS_TBL_SIZE];
	HW_U16 lens_b_table[ISP_LENS_TBL_SIZE];
};

struct isp_gamma_config {
	HW_U16 gamma_tbl[ISP_GAMMA_TBL_LENGTH]; // for hardware
};

/*
 *  struct isp_ae_config - .
 */
struct isp_ae_config {
	struct isp_h3a_reg_win ae_reg_win;
};

/*
 *  struct isp_af_config - .
 */
struct isp_af_config {
	HW_U16 af_sap_lim;
	struct isp_h3a_reg_win af_reg_win;
};

/*
 *  struct isp_awb_config - .
 */
struct isp_awb_config {
	HW_U16 awb_r_sat_lim;
	HW_U16 awb_g_sat_lim;
	HW_U16 awb_b_sat_lim;
	struct isp_h3a_reg_win awb_reg_win;
};


/*
 *  struct isp_wb_gain_config - .
 */
struct isp_wb_gain_config {
	HW_U32 clip_val;
	struct isp_wb_gain wb_gain;

};

/*
 *  struct isp_hist_config - .
 */
struct isp_hist_config {
	HW_S32 hist_threshold;
	enum isp_hist_mode hist_mode;
	struct isp_h3a_reg_win hist_reg_win;
};

/*
 *  struct isp_afs_config - .
 */
struct isp_afs_config {
	HW_U32 inc_line;
};

/*
 *  struct isp_mode_config - .
 */

struct isp_mode_config {
	unsigned int input_fmt;
	unsigned int wdr_mode;
	unsigned int wdr_cmp_mode;
	unsigned int otf_dpc_mode;
	unsigned int saturation_mode;
	unsigned int hist_mode;
	unsigned int hist_sel;
	unsigned int cfa_mode;
	unsigned int ae_mode;
	unsigned int awb_mode;
	unsigned int dg_mode;
};

/*
 *  struct isp_rgb2rgb_config - .
 */
struct isp_rgb2rgb_config {
	 struct isp_rgb2rgb_gain_offset color_matrix;
};

/*
 *  struct isp_gain_offset_config - .
 */
struct isp_gain_offset_config {
	struct isp_offset offset;
	struct isp_dg_gain gain;
	struct isp_offset sensor_offset;
};

/*
 *  struct isp_module_config - .
 */
struct isp_module_config {
	HW_U32 isp_dev_id;
	HW_U32 isp_platform_id;
	HW_U32 module_enable_flag;

	HW_U32 table_update;

	struct isp_mode_config mode_cfg;
	/* AFS config */
	struct isp_afs_config afs_cfg;
	struct isp_cfa_config cfa_cfg;
	struct isp_sharp_config sharp_cfg;
	struct isp_contrast_config contrast_cfg;
	struct isp_d2d_config bdnf_cfg;

	struct isp_pltm_config pltm_cfg;
	struct isp_wdr_config wdr_cfg;

	struct isp_drc_config drc_cfg;
	struct isp_cem_config cem_cfg;
	struct isp_lut_config lut_cfg;
	struct isp_lens_config lens_cfg;
	struct isp_gamma_config gamma_cfg;
	struct isp_disc_config disc_cfg;

	struct isp_rgb2rgb_config rgb2rgb_cfg;
	struct isp_rgb2yuv_gain_offset rgb2yuv;
	struct isp_ae_config ae_cfg;
	struct isp_af_config af_cfg;
	struct isp_awb_config awb_cfg;
	struct isp_hist_config hist_cfg;
	struct isp_gain_offset_config gain_offset_cfg;
	struct isp_wb_gain_config wb_gain_cfg;
	struct isp_dpc_config otf_cfg;
	struct isp_ctc_config ctc_cfg;
	struct isp_gca_config gca_cfg;
	struct isp_lca_config lca_cfg;

	struct isp_cnr_config cnr_cfg;
	struct isp_tdnf_config tdf_cfg;
	struct isp_saturation_config satu_cfg;

	enum   isp_output_speed output_speed;

	//table addr
	void *table_mapping1;
	void *lens_table;
	void *gamma_table;
	void *linear_table;
	void *wdr_table;
	void *tdnf_table;
	void *pltm_table;
	void *contrast_table;

	void *table_mapping2;
	void *drc_table;
	void *saturation_table;
	void *cem_table;
	/*ISP520*/
	void *fe_table;
	void *bayer_table;
	void *rgb_table;
	void *yuv_table;
	void *dehaze_table;
	/*ISP521*/
	void *msc_table;
};


enum isp_features_flags {
	ISP_FEATURES_AE		=  (1 << 0) ,
	ISP_FEATURES_LINEAR     =  (1 << 1) ,
	ISP_FEATURES_WDR        =  (1 << 2) ,
	ISP_FEATURES_DPC        =  (1 << 3) ,
	ISP_FEATURES_D2D       =  (1 << 4) ,
	ISP_FEATURES_D3D =  (1 << 5) ,
	ISP_FEATURES_AWB	=  (1 << 6) ,
	ISP_FEATURES_WB         =  (1 << 7) ,
	ISP_FEATURES_LSC        =  (1 << 8) ,
	ISP_FEATURES_GAMMA      =  (1 << 9) ,
	ISP_FEATURES_SHARP      =  (1 << 10),
	ISP_FEATURES_AF         =  (1 << 11),
	ISP_FEATURES_RGB2RGB    =  (1 << 12),
	ISP_FEATURES_RGB_DRC    =  (1 << 13),
	ISP_FEATURES_PLTM       =  (1 << 14),
	ISP_FEATURES_CEM        =  (1 << 15),
	ISP_FEATURES_AFS        =  (1 << 16),
	ISP_FEATURES_HIST       =  (1 << 17),
	ISP_FEATURES_BLC        =  (1 << 18),
	ISP_FEATURES_DG         =  (1 << 19),
	ISP_FEATURES_SO         =  (1 << 20),
	ISP_FEATURES_CTC        =  (1 << 21),
	ISP_FEATURES_CONTRAST   =  (1 << 22),
	ISP_FEATURES_CNR        =  (1 << 23),
	ISP_FEATURES_SATU       =  (1 << 24),
	ISP_FEATURES_CFA        =  (1 << 25),
	ISP_FEATURES_MODE       =  (1 << 26),
	ISP_FEATURES_MAX,

	/* all possible flags raised */
	ISP_FEATURES_All = (((ISP_FEATURES_MAX -1 ) << 1) -1 ),
};

void isp_hardware_update(struct isp_module_config *module_cfg);
void isp_map_addr(struct isp_module_config *module_cfg, unsigned long vaddr);

#endif //__ISP__MODULE__CFG__H
