
/*
 * Hawkview ISP - isp_comm.h module
 * Copyright (c) 2018 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 * Version: 1.0
 * Author: zhaowei
 * Date: 2018/07/11
 * Description isp500 && isp520 register header file
 */

#ifndef __BSP__ISP__COMM__H
#define __BSP__ISP__COMM__H

#include "isp_type.h"
#include "../isp_version.h"

#define ISP_REG_TBL_LENGTH			33
#define ISP_REG_TBL_NODE_LENGTH			9

#if (ISP_VERSION == 520)
#define ISP_GAMMA_TBL_LENGTH			(3*1024)

//ISP LOAD DRAM
#define ISP_LOAD_DRAM_SIZE			0x9f00

#define ISP_LOAD_REG_SIZE			0x1000
#define ISP_FE_TABLE_SIZE			0x0600
#define ISP_BAYER_TABLE_SIZE			0x5600
#define ISP_RGB_TABLE_SIZE			0x1c00
#define ISP_YUV_TABLE_SIZE			0x1700

#define ISP_S1_LC_R_TBL_SIZE			0x0200
#define ISP_S1_LC_G_TBL_SIZE			0x0200
#define ISP_S1_LC_B_TBL_SIZE			0x0200

#define ISP_S0_LC_R_TBL_SIZE			0x0200
#define ISP_S0_LC_G_TBL_SIZE			0x0200
#define ISP_S0_LC_B_TBL_SIZE			0x0200

#define ISP_WDR_GAMMA_FE_MEM_SIZE		0x2000
#define ISP_WDR_GAMMA_BE_MEM_SIZE		0x2000
#define ISP_WDR_MEM_SIZE (ISP_WDR_GAMMA_FE_MEM_SIZE + ISP_WDR_GAMMA_BE_MEM_SIZE)

#define ISP_LSC_TBL_SIZE			0x0800
#define ISP_D3D_K_3D_INCREASE_SIZE		0x0100
#define ISP_D3D_DIFF_TBL_SIZE			0x0100
#define ISP_PLTM_H_MEM_SIZE			0x0100
#define ISP_PLTM_V_MEM_SIZE			0x0100
#define ISP_PLTM_POW_MEM_SIZE			0x0200
#define ISP_PLTM_F_MEM_SIZE			0x0200
#define ISP_PLTM_MEM_SIZE (ISP_PLTM_H_MEM_SIZE + ISP_PLTM_V_MEM_SIZE + ISP_PLTM_POW_MEM_SIZE + ISP_PLTM_F_MEM_SIZE)

#define ISP_SATURATION_MEM_SIZE			0x0200
#define ISP_RGB_DRC_TBL_SIZE			0x0200
#define ISP_GAMMA_TBL_SIZE			0x1000
#define ISP_DEHAZE_PP_TBL_SIZE			0x0100
#define ISP_DEHAZE_PD_TBL_SIZE			0x0100
#define ISP_DEHAZE_TR_TBL_SIZE			0x0100
#define ISP_DEHAZE_BT_TBL_SIZE			0x0400

#define ISP_CEM_TBL0_SIZE			0x0cc0
#define ISP_CEM_TBL1_SIZE			0x0a40
#define ISP_CEM_MEM_SIZE (ISP_CEM_TBL0_SIZE + ISP_CEM_TBL1_SIZE)

//FE TBL
#define ISP_S1_LINEAR_MEM_OFS			0x0

//BAYER TBL
#define ISP_S0_LINEAR_MEM_OFS			0x0
#define ISP_WDR_GAMMA_FE_MEM_OFS		0x0600
#define ISP_WDR_GAMMA_BE_MEM_OFS		0x2600
#define ISP_LSC_MEM_OFS				0x4600
#define ISP_TDNF_K3D_MEM_OFS			0x4e00
#define ISP_TDNF_DIFF_MEM_OFS			0x4f00
#define ISP_PLTM_H_MEM_OFS			0x5000
#define ISP_PLTM_V_MEM_OFS			0x5100
#define ISP_PLTM_POW_MEM_OFS			0x5200
#define ISP_PLTM_F_MEM_OFS			0x5400

//RGB TBL
#define ISP_SATURATION_MEM_OFS			0x0
#define ISP_DRC_MEM_OFS				0x0200
#define ISP_GAMMA_MEM_OFS			0x0400
#define ISP_DEHAZE_MEM_OFS			0x1400

//YUV TBL
#define ISP_CEM_MEM_OFS				0x0

//ISP SAVE DRAM
#define ISP_SAVE_DRAM_SIZE			0xea40
#define ISP_SAVE_REG_SIZE			0x0040

#define ISP_STAT_TOTAL_SIZE			0xea40
#define ISP_STATISTIC_SIZE			0xea00

#define ISP_STAT_HIST_MEM_SIZE			0x0200
#define ISP_STAT_AE_MEM_SIZE			0x4800
#define ISP_STAT_AF_IIR_ACC_SIZE		0x0c00
#define ISP_STAT_AF_FIR_ACC_SIZE		0x0c00
#define ISP_STAT_AF_IIR_CNT_SIZE		0x0c00
#define ISP_STAT_AF_FIR_CNT_SIZE		0x0c00
#define ISP_STAT_AF_HL_CNT_SIZE			0x0c00
#define ISP_STAT_AFS_MEM_SIZE			0x0200
#define ISP_STAT_AWB_RGB_MEM_SIZE		0x4800
#define ISP_STAT_AWB_CNT_MEM_SIZE		0x0c00
#define ISP_STAT_PLTM_LST_MEM_SIZE		0x0600
#define ISP_STAT_DEHAZE_DC_HIST_SIZE		0x0400
#define ISP_STAT_DEHAZE_BT_SIZE			0x0400

#define ISP_STAT_HIST_MEM_OFS			0x0040
#define ISP_STAT_AE_MEM_OFS			0x0240
#define ISP_STAT_AF_MEM_OFS			0x4a40
#define ISP_STAT_AFS_MEM_OFS			0x8640
#define ISP_STAT_AWB_MEM_OFS			0x8840
#define ISP_STAT_PLTM_LST_MEM_OFS		0xdc40
#define ISP_STAT_DEHAZE_MEM_OFS			0xe240


/*
 *  update table
 */

#define LINEAR_UPDATE		(1 << 1)
#define LENS_UPDATE		(1 << 2)
#define GAMMA_UPDATE		(1 << 3)
#define DRC_UPDATE		(1 << 4)
#define SATU_UPDATE		(1 << 5)
#define WDR_UPDATE		(1 << 6)
#define D3D_UPDATE		(1 << 7)
#define PLTM_UPDATE		(1 << 8)
#define CEM_UPDATE		(1 << 9)
#define DEHAZE_UPDATE		(1 << 11)
#define S1_PARA_READY		(1 << 12)
#define S1_LINEAR_UPDATE	(1 << 13)

#elif (ISP_VERSION == 521)

#define ISP_GAMMA_TBL_LENGTH			(3*1024)

//ISP LOAD DRAM
#define ISP_LOAD_DRAM_SIZE			0x13240

#define ISP_LOAD_REG_SIZE			0x1000
#define ISP_FE_TABLE_SIZE			0x0600
#define ISP_BAYER_TABLE_SIZE			0x6540
#define ISP_RGB_TABLE_SIZE			0x1c00
#define ISP_YUV_TABLE_SIZE			0x1700
#define ISP_DBG_TABLE_SIZE			0x8400

#define ISP_S1_LC_R_TBL_SIZE			0x0200
#define ISP_S1_LC_G_TBL_SIZE			0x0200
#define ISP_S1_LC_B_TBL_SIZE			0x0200

#define ISP_S0_LC_R_TBL_SIZE			0x0200
#define ISP_S0_LC_G_TBL_SIZE			0x0200
#define ISP_S0_LC_B_TBL_SIZE			0x0200

#define ISP_WDR_GAMMA_FE_MEM_SIZE		0x2000
#define ISP_WDR_GAMMA_BE_MEM_SIZE		0x2000
#define ISP_WDR_MEM_SIZE (ISP_WDR_GAMMA_FE_MEM_SIZE + ISP_WDR_GAMMA_BE_MEM_SIZE)

#define ISP_LSC_TBL_SIZE			0x0800
#define ISP_D3D_K_3D_INCREASE_SIZE		0x0100
#define ISP_D3D_DIFF_TBL_SIZE			0x0100
#define ISP_PLTM_H_MEM_SIZE			0x0100
#define ISP_PLTM_V_MEM_SIZE			0x0100
#define ISP_PLTM_POW_MEM_SIZE			0x0200
#define ISP_PLTM_F_MEM_SIZE			0x0200
#define ISP_PLTM_MEM_SIZE (ISP_PLTM_H_MEM_SIZE + ISP_PLTM_V_MEM_SIZE + ISP_PLTM_POW_MEM_SIZE + ISP_PLTM_F_MEM_SIZE)

#define ISP_MSC_TBL_MEM_SIZE			0x0f20

#define ISP_SATURATION_MEM_SIZE			0x0200
#define ISP_RGB_DRC_TBL_SIZE			0x0200
#define ISP_GAMMA_TBL_SIZE			0x1000
#define ISP_DEHAZE_PP_TBL_SIZE			0x0100
#define ISP_DEHAZE_PD_TBL_SIZE			0x0100
#define ISP_DEHAZE_TR_TBL_SIZE			0x0100
#define ISP_DEHAZE_BT_TBL_SIZE			0x0400

#define ISP_CEM_TBL0_SIZE			0x0cc0
#define ISP_CEM_TBL1_SIZE			0x0a40
#define ISP_CEM_MEM_SIZE (ISP_CEM_TBL0_SIZE + ISP_CEM_TBL1_SIZE)

//FE TBL
#define ISP_S1_LINEAR_MEM_OFS			0x0

//BAYER TBL
#define ISP_S0_LINEAR_MEM_OFS			0x0
#define ISP_WDR_GAMMA_FE_MEM_OFS		0x0600
#define ISP_WDR_GAMMA_BE_MEM_OFS		0x2600
#define ISP_LSC_MEM_OFS				0x4600
#define ISP_TDNF_K3D_MEM_OFS			0x4e00
#define ISP_TDNF_DIFF_MEM_OFS			0x4f00
#define ISP_PLTM_H_MEM_OFS			0x5000
#define ISP_PLTM_V_MEM_OFS			0x5100
#define ISP_PLTM_POW_MEM_OFS			0x5200
#define ISP_PLTM_F_MEM_OFS			0x5400
#define ISP_MSC_MEM_OFS				0x5600

//RGB TBL
#define ISP_SATURATION_MEM_OFS			0x0
#define ISP_DRC_MEM_OFS				0x0200
#define ISP_GAMMA_MEM_OFS			0x0400
#define ISP_DEHAZE_MEM_OFS			0x1400

//YUV TBL
#define ISP_CEM_MEM_OFS				0x0

//ISP SAVE DRAM
#define ISP_SAVE_DRAM_SIZE			0x16e40
#define ISP_SAVE_REG_SIZE			0x0040

#define ISP_STAT_TOTAL_SIZE			0xea40
#define ISP_STATISTIC_SIZE			0xea00
#define ISP_DBG_STAT_SIZE			0x8400

#define ISP_STAT_HIST_MEM_SIZE			0x0200
#define ISP_STAT_AE_MEM_SIZE			0x4800
#define ISP_STAT_AF_IIR_ACC_SIZE		0x0c00
#define ISP_STAT_AF_FIR_ACC_SIZE		0x0c00
#define ISP_STAT_AF_IIR_CNT_SIZE		0x0c00
#define ISP_STAT_AF_FIR_CNT_SIZE		0x0c00
#define ISP_STAT_AF_HL_CNT_SIZE			0x0c00
#define ISP_STAT_AFS_MEM_SIZE			0x0200
#define ISP_STAT_AWB_RGB_MEM_SIZE		0x4800
#define ISP_STAT_AWB_CNT_MEM_SIZE		0x0c00
#define ISP_STAT_PLTM_LST_MEM_SIZE		0x0600
#define ISP_STAT_DEHAZE_DC_HIST_SIZE		0x0400
#define ISP_STAT_DEHAZE_BT_SIZE			0x0400

#define ISP_STAT_HIST_MEM_OFS			0x0040
#define ISP_STAT_AE_MEM_OFS			0x0240
#define ISP_STAT_AF_MEM_OFS			0x4a40
#define ISP_STAT_AFS_MEM_OFS			0x8640
#define ISP_STAT_AWB_MEM_OFS			0x8840
#define ISP_STAT_PLTM_LST_MEM_OFS		0xdc40
#define ISP_STAT_DEHAZE_MEM_OFS			0xe240


/*
 *  update table
 */

#define LINEAR_UPDATE		(1 << 1)
#define LENS_UPDATE		(1 << 2)
#define GAMMA_UPDATE		(1 << 3)
#define DRC_UPDATE		(1 << 4)
#define SATU_UPDATE		(1 << 5)
#define WDR_UPDATE		(1 << 6)
#define D3D_UPDATE		(1 << 7)
#define PLTM_UPDATE		(1 << 8)
#define CEM_UPDATE		(1 << 9)
#define MSC_UPDATE		(1 << 10)
#define DEHAZE_UPDATE		(1 << 11)
#define S1_PARA_READY		(1 << 12)
#define S1_LINEAR_UPDATE	(1 << 13)

#else
#define ISP_GAMMA_TBL_LENGTH		(3*256)

//ISP LOAD DRAM
#define ISP_LOAD_DRAM_SIZE		(0x400+0x5a00+0x1f00)
#define ISP_LOAD_REG_SIZE		0x400

#define ISP_TABLE_MAPPING1_SIZE		0x5a00
#define ISP_LSC_MEM_SIZE		(256*8)
#define ISP_GAMMA_MEM_SIZE		(256*4)
#define ISP_LINEAR_MEM_SIZE		(256*6)
#define ISP_WDR_GAMMA_FE_MEM_SIZE	(4096*2)
#define ISP_WDR_GAMMA_BE_MEM_SIZE	(4096*2)
#define ISP_TDNF_DIFF_MEM_SIZE		(256*1)

#define ISP_WDR_MEM_SIZE (ISP_WDR_GAMMA_FE_MEM_SIZE + ISP_WDR_GAMMA_BE_MEM_SIZE)

#define ISP_PLTM_H_MEM_SIZE		(256*1)
#define ISP_PLTM_V_MEM_SIZE		(256*1)
#define ISP_PLTM_POW_MEM_SIZE		(256*2)
#define ISP_PLTM_F_MEM_SIZE		(256*2)

#define ISP_PLTM_MEM_SIZE (ISP_PLTM_H_MEM_SIZE + ISP_PLTM_V_MEM_SIZE + ISP_PLTM_POW_MEM_SIZE + ISP_PLTM_F_MEM_SIZE)

#define ISP_CONTRAST_PE_MEM_SIZE	(128*2)

#define ISP_TABLE_MAPPING2_SIZE		0x1f00
#define ISP_DRC_MEM_SIZE		(256*2)
#define ISP_SATURATION_MEM_SIZE		(256*2)
#define ISP_CEM_MEM_SIZE		(736*8)

#define ISP_LSC_MEM_OFS			0x0
#define ISP_GAMMA_MEM_OFS		0x0800
#define ISP_LINEAR_MEM_OFS		0x0c00
#define ISP_WDR_GAMMA_FE_MEM_OFS	0x1200
#define ISP_WDR_GAMMA_BE_MEM_OFS	0x3200
#define ISP_TDNF_DIFF_MEM_OFS		0x5200
#define ISP_PLTM_H_MEM_OFS		0x5300
#define ISP_PLTM_V_MEM_OFS		0x5400
#define ISP_PLTM_POW_MEM_OFS		0x5500
#define ISP_PLTM_F_MEM_OFS		0x5700
#define ISP_CONTRAST_PE_MEM_OFS		0x5900

#define ISP_DRC_MEM_OFS			0x0
#define ISP_SATURATION_MEM_OFS		0x0600
#define ISP_CEM_MEM_OFS			0x0800

#define ISP_SAVE_DRAM_SIZE		(0x400+0xab00)
#define ISP_SAVE_REG_SIZE		0x0400

#define ISP_STAT_TOTAL_SIZE		0xAB00

#define ISP_STAT_HIST_MEM_SIZE		0x0200
#define ISP_STAT_AE_MEM_SIZE		0x4800
#define ISP_STAT_AF_CNT_MEM_SIZE 	0x0100
#define ISP_STAT_AF_VH_MEM_SIZE		0x0400
#define ISP_STAT_AFS_MEM_SIZE		0x0200
#define ISP_STAT_AWB_RGB_MEM_SIZE	0x4800
#define ISP_STAT_AWB_CNT_MEM_SIZE	0x0C00
#define ISP_STAT_PLTM_LST_MEM_SIZE	0x0600

#define ISP_STAT_HIST_MEM_OFS		0x0000
#define ISP_STAT_AE_MEM_OFS		0x0200
#define ISP_STAT_AF_MEM_OFS		0x4a00
#define ISP_STAT_AFS_MEM_OFS		0x4f00
#define ISP_STAT_AWB_MEM_OFS		0x5100
#define ISP_STAT_PLTM_LST_MEM_OFS	0xa500

/*
 *  update table
 */
#define LINEAR_UPDATE	(1 << 3)
#define LENS_UPDATE	(1 << 4)
#define GAMMA_UPDATE	(1 << 5)
#define DRC_UPDATE	(1 << 6)
#define SATU_UPDATE	(1 << 8)
#define WDR_UPDATE	(1 << 9)
#define D3D_UPDATE	(1 << 10)
#define PLTM_UPDATE	(1 << 11)
#define CEM_UPDATE	(1 << 12)
#define CONTRAST_UPDATE	(1 << 13)
#endif

#define TABLE_UPDATE_ALL 0xffffffff

/*
 *  ISP Module enable
 */
#define S1_BLC_EN	(1 << 0)
#define S1_LC_EN	(1 << 1)
#define S1_DG_EN	(1 << 2)

#define AE_EN		(1 << 0)
#define LC_EN		(1 << 1)
#define WDR_EN		(1 << 2)
#define DPC_EN		(1 << 3)
#define D2D_EN		(1 << 4)
#define D3D_EN		(1 << 5)
#define AWB_EN		(1 << 6)
#define WB_EN		(1 << 7)
#define LSC_EN		(1 << 8)
#define BGC_EN		(1 << 9)
#define SHARP_EN	(1 << 10)
#define AF_EN		(1 << 11)
#define RGB2RGB_EN	(1 << 12)
#define RGB_DRC_EN	(1 << 13)
#define PLTM_EN		(1 << 14)
#define CEM_EN		(1 << 15)
#define AFS_EN		(1 << 16)
#define HIST_EN		(1 << 17)
#define BLC_EN		(1 << 18)
#define DG_EN		(1 << 19)
#define SO_EN		(1 << 20)
#define CTC_EN		(1 << 21)
#if (ISP_VERSION >= 521)
#define MSC_EN     	(1 << 22)
#else
#define CONTRAST_EN     (1 << 22)
#endif
#define CNR_EN		(1 << 23)
#define SATU_EN		(1 << 24)
#define DEHAZE_EN	(1 << 25)
#define S0_DG_EN	(1 << 26)
#define LCA_EN		(1 << 27)
#define GCA_EN		(1 << 28)

#define ISP_MODULE_EN_ALL	(0xffffffff)
#define GTM_LUM_IDX_NUM 9
#define GTM_VAR_IDX_NUM 9

enum isp_raw_ch {
	ISP_RAW_CH_R =  0,
	ISP_RAW_CH_GR,
	ISP_RAW_CH_GB,
	ISP_RAW_CH_G,
	ISP_RAW_CH_MAX,
};

enum isp_sharp_cfg {
	ISP_SHARP_MIN_VAL     = 0,
	ISP_SHARP_MAX_VAL     = 1,
	ISP_SHARP_BLACK_LEVEL     = 2,
	ISP_SHARP_WHITE_LEVEL     = 3,
	ISP_SHARP_BLACK_CLIP     = 4,
	ISP_SHARP_WHITE_CLIP     = 5,
	ISP_SHARP_BLACK_GAIN     = 6,
	ISP_SHARP_BLACK_OFFSET     = 7,
	ISP_SHARP_WHITE_GAIN     = 8,
	ISP_SHARP_WHITE_OFFSET     = 9,
#if (ISP_VERSION >= 520)
	ISP_SHARP_EDGE_SCALE_RATIO = 10,
	ISP_SHARP_HFRQ_SCALE_RATIO = 11,
	ISP_SHARP_EDGE_CONV_PARA = 12,
	ISP_SHARP_HFRQ_CONV_PARA = 13,
	ISP_SHARP_DIR_EQ_RATIO = 14,
	ISP_SHARP_DIR_CLIP_VAL = 15,
	ISP_SHARP_NS_LW_TH = 16,
	ISP_SHARP_NS_HI_TH = 17,
	ISP_SHARP_EDGE_TH = 18,
	ISP_SHARP_HV_EDGE_SM_RATIO = 19,
	ISP_SHARP_AA_EDGE_SM_RATIO = 20,
	ISP_SHARP_EDGE_WHITE_STREN = 21,
	ISP_SHARP_EDGE_BLACK_STREN = 22,
	ISP_SHARP_HFRQ_WHITE_STREN = 23,
	ISP_SHARP_HFRQ_BLACK_STREN = 24,
	ISP_SHARP_OVER_AREA_CTRL = 25,
	ISP_SHARP_UNDER_AREA_CTRL = 26,
	ISP_SHARP_OVER_VAL_CTRL = 27,
	ISP_SHARP_UNDER_VAL_CTRL = 28,
#endif
	ISP_SHARP_MAX,
};

enum isp_contrast_cfg {
	ISP_CONTRAST_MIN_VAL = 0,
	ISP_CONTRAST_MAX_VAL = 1,
	ISP_CONTRAST_BLACK_LEVEL = 2,
	ISP_CONTRAST_WHITE_LEVEL = 3,
	ISP_CONTRAST_BLACK_CLIP = 4,
	ISP_CONTRAST_WHITE_CLIP = 5,
	ISP_CONTRAST_PLAT_TH = 6,
	ISP_CONTRAST_BLACK_GAIN = 7,
	ISP_CONTRAST_BLACK_OFFSET = 8,
	ISP_CONTRAST_WHITE_GAIN = 9,
	ISP_CONTRAST_WHITE_OFFSET = 10,
	ISP_CONTRAST_MAX,
};

enum isp_denoise_cfg {
	ISP_DENOISE_BLACK_GAIN     = 0,
	ISP_DENOISE_BLACK_OFFSET = 1,
	ISP_DENOISE_WHITE_GAIN = 2,
	ISP_DENOISE_WHITE_OFFSET = 3,
#if (ISP_VERSION >= 520)
	ISP_DENOISE_HF_RATIO = 4,
	ISP_DENOISE_BF_RATIO = 5,
	ISP_DENOISE_LF_RATIO = 6,
	ISP_DENOISE_LP0_NP_SIDE_RATIO = 7,
	ISP_DENOISE_LP1_NP_SIDE_RATIO = 8,
	ISP_DENOISE_LP2_NP_SIDE_RATIO = 9,
	ISP_DENOISE_LP0_TH_RATIO = 10,
	ISP_DENOISE_LP1_TH_RATIO = 11,
	ISP_DENOISE_LP2_TH_RATIO = 12,
	ISP_DENOISE_LP3_TH_RATIO = 13,
	ISP_DENOISE_LP0_PCNT_RATIO = 14,
	ISP_DENOISE_LP1_PCNT_RATIO = 15,
	ISP_DENOISE_LP2_PCNT_RATIO = 16,
	ISP_DENOISE_LP3_PCNT_RATIO = 17,
	ISP_DENOISE_HI_TH = 18,
	ISP_DENOISE_LOW_TH = 19,
#endif
	ISP_DENOISE_MAX,
};

enum sensor_offset {
	ISP_SO_R_OFFSET = 0,
	ISP_SO_GR_OFFSET = 1,
	ISP_SO_GB_OFFSET = 2,
	ISP_SO_B_OFFSET = 3,
	ISP_SO_MAX,
};
enum black_level {
	ISP_BLC_R_OFFSET = 0,
	ISP_BLC_GR_OFFSET = 1,
	ISP_BLC_GB_OFFSET = 2,
	ISP_BLC_B_OFFSET = 3,
	ISP_BLC_MAX,
};
enum dpc_cfg {
	ISP_DPC_TH_SLOP = 0,
	ISP_DPC_OTF_MIN_TH = 1,
	ISP_DPC_OTF_MAX_TH = 2,
	ISP_DPC_OTF_MODE = 3,
	ISP_DPC_MAX,
};
enum pltm_dynamic_cfg {
	ISP_PLTM_DYNAMIC_AUTO_STREN = 0,
	ISP_PLTM_DYNAMIC_MANUL_STREN = 1,
	ISP_PLTM_DYNAMIC_AE_COMP = 2,
	ISP_PLTM_DYNAMIC_MIN_TH = 3,
	ISP_PLTM_DYNAMIC_MAX,
};


enum isp_tdf_cfg {
	ISP_TDF_NOISE_CLIP_RATIO = 0,
	ISP_TDF_DIFF_CLIP_RATIO = 1,
	ISP_TDF_K_3D_S = 2,
	ISP_TDF_DIFF_CAL_MODE = 3,
	ISP_TDF_BLACK_GAIN = 4,
	ISP_TDF_BLACK_OFFSET = 5,
	ISP_TDF_WHITE_GAIN = 6,
	ISP_TDF_WHITE_OFFSET = 7,
	ISP_TDF_REF_BLACK_GAIN = 8,
	ISP_TDF_REF_BLACK_OFFSET = 9,
	ISP_TDF_REF_WHITE_GAIN = 10,
	ISP_TDF_REF_WHITE_OFFSET = 11,
#if (ISP_VERSION >= 520)
	ISP_TDF_LUM_DIFF_CLIP_RATIO = 12,
	ISP_TDF_BRIGHT_DIFF_CLIP_RATIO = 13,
	ISP_TDF_BRIGHT_DIFF_RATIO = 14,
	ISP_TDF_MV_ORI_RATIO = 15,
	ISP_TDF_ST_2D_RATIO = 16,
	ISP_TDF_C_WEIGHT1 = 17,
	ISP_TDF_C_WEIGHT2 = 18,
	ISP_TDF_C_WEIGHT3 = 19,
	ISP_TDF_LTF_UPDATE_FRM = 20,
	ISP_TDF_LTF_EN = 21,
#endif
	ISP_TDF_MAX,
};

enum isp_ae_hist_cfg {
	ISP_AE_HIST_DARK_WEIGHT_MIN = 0,
	ISP_AE_HIST_DARK_WEIGHT_MAX = 1,
	ISP_AE_HIST_BRIGHT_WEIGHT_MIN = 2,
	ISP_AE_HIST_BRIGHT_WEIGHT_MAX = 3,
	ISP_AE_HIST_CFG_MAX,
};

enum isp_gtm_comm_cfg {
	ISP_GTM_GAIN = 0,
	ISP_GTM_EQ_RATIO = 1,
	ISP_GTM_EQ_SMOOTH = 2,
	ISP_GTM_BLACK = 3,
	ISP_GTM_WHITE = 4,
	ISP_GTM_BLACK_ALPHA = 5,
	ISP_GTM_WHITE_ALPHA = 6,
	ISP_GTM_GAMMA_IND = 7,

	ISP_GTM_GAMMA_PLUS = 8,
	ISP_GTM_HEQ_MAX,
};

enum isp_pltm_comm_cfg {
	ISP_PLTM_MODE = 0,

	ISP_PLTM_ORIPIC_RATIO = 1,
	ISP_PLTM_TR_ORDER = 2,
	ISP_PLTM_LAST_ORDER_RATIO = 3,
	ISP_PLTM_POW_TBL = 4,
	ISP_PLTM_F_TBL = 5,

	ISP_PLTM_LSS_SWITCH = 6,
	ISP_PLTM_LUM_RATIO = 7,
	ISP_PLTM_LP_HALO_RES = 8,
	ISP_PLTM_WHITE_LEVEL = 9,
	ISP_PLTM_SPATIAL_ASM = 10,
	ISP_PLTM_INTENS_ASYM = 11,
	ISP_PLTM_BLOCK_V_NUM = 12,
	ISP_PLTM_BLOCK_H_NUM = 13,
	ISP_PLTM_CONTRAST = 14,

	ISP_PLTM_TOLERANCE = 15,
	ISP_PLTM_SPEED = 16,
	ISP_PLTM_STEP = 17,

	ISP_PLTM_MAX,
};

enum isp_saturation_cfg {
	ISP_SATURATION_SATU_R = 0,
	ISP_SATURATION_SATU_G = 1,
	ISP_SATURATION_SATU_B = 2,
	ISP_SATURATION_SATU_MODE = 3,
	ISP_SATURATION_SATU_TBL_SG1 = 4,
	ISP_SATURATION_SATU_TBL_SG2 = 5,
	ISP_SATURATION_SATU_TBL_TH = 6,

	ISP_SATURATION_MAX,
};

enum isp_platform {
	ISP_PLATFORM_SUN8IW12P1,
	ISP_PLATFORM_SUN8IW16P1,

	ISP_PLATFORM_NUM,
};

#if (ISP_VERSION >= 520)
struct isp_wdr_mode_cfg {
	unsigned char wdr_ch_seq;
	unsigned char wdr_exp_seq;
	unsigned char wdr_mode;
};
#endif

struct isp_size {
	HW_U32 width;
	HW_U32 height;
};

enum enable_flag {
	DISABLE    = 0,
	ENABLE     = 1,
};

enum wdr_mode {
	DOL_WDR = 0,
	COMANDING_WDR = 1,
};

enum isp_dpc_mode {
	DPC_NORMAL = 0,
	DPC_STRONG = 1,
	DPC_PEPPER = 2,
};

enum saturation_mode {
	SATU_NORM_MODE = 0,
	SATU_STRONG_MODE = 1,
};

enum isp_hist_mode {
	AVG_MODE = 0,
	MIN_MODE = 1,
	MAX_MODE = 2,
};

enum isp_hist_src {
	HIST_AFTER_WDR = 0,
	HIST_AFTER_CNR = 1,
};

enum isp_cfa_mode {
	CFA_NORM_MODE   = 0,
	CFA_BW_MODE = 1,
};

enum isp_ae_mode {
	AE_BEFORE_WDR = 0,
	AE_AFTER_WDR = 1,
	AE_AFTER_CNR = 2,
};

enum isp_awb_mode {
	AWB_AFTER_WDR = 0,
	AWB_AFTER_PLTM = 1,
};

enum isp_dg_mode {
	DG_AFTER_WDR = 0,
	DG_AFTER_SO = 1,
#if (ISP_VERSION >= 521)
	DG_BEFORE_WDR = 2,
#endif
};

#if (ISP_VERSION >= 520)
enum isp_d3d_mode {
	D3D_MAX = 0,
	D3D_MIN = 1,
	D3D_WT = 2,
};

enum isp_af_mode {
	AF_BEFORE_D2D = 0,
	AF_AFTER_D2D = 1,
};
#endif

enum isp_input_seq {
	ISP_BGGR = 4,
	ISP_RGGB = 5,
	ISP_GBRG = 6,
	ISP_GRBG = 7,
};

enum JUDGE_COMP_CFG {
	STATUS_STATIC = 0,
	STATUS_SUS_STATIC = 1,
	STATUS_SUS_MOTION = 2,
	STATUS_MOTION = 3,
	STATUS_JUDGE_MAX,
};

struct isp_ctc_config {
	HW_U16 ctc_th_max;
	HW_U16 ctc_th_min;
	HW_U16 ctc_th_slope;
	HW_U16 ctc_dir_wt;
	HW_U16 ctc_dir_th;
};

struct isp_wdr_config {
	HW_U16 wdr_low_th;
	HW_U16 wdr_hi_th;
	HW_U16 wdr_exp_ratio;
	HW_U16 wdr_slope;
#if (ISP_VERSION >= 520)
	HW_U16 wdr_mv_th;
	HW_U16 wdr_mv_scale;
	HW_U16 wdr_out_sel;
#endif
	HW_U8 wdr_table[ISP_WDR_MEM_SIZE];
};

struct isp_dpc_config {
#if (ISP_VERSION >= 521)
	HW_U8 hot_ratio;
	HW_U8 cold_ratio;
	HW_U8 nbhd_diff_ratio;
	HW_U8 nearest_diff_ratio;
	HW_U16 slope_th:10;
	HW_U16 cold_abs_th:9;
#else
	HW_U16 th_slop;
	HW_U16 min_th;
	HW_U16 max_th;
#endif
};

struct isp_gca_config {
	HW_U16 gca_ct_h;
	HW_U16 gca_ct_w;
	HW_U16 gca_r_para0;
	HW_U16 gca_r_para1;
	HW_U16 gca_r_para2;
	HW_U16 gca_b_para0;
	HW_U16 gca_b_para1;
	HW_U16 gca_b_para2;
	HW_U16 gca_int_cns;
};

struct isp_lca_config {
	HW_U16 lca_gf_cor_ratio;
	HW_U16 lca_pf_cor_ratio;
	HW_U16 lca_lum_th;
	HW_U16 lca_grad_th;
	HW_U16 lca_clr_gth;
	HW_U16 lca_pf_rshf;
	HW_U16 lca_pf_bslp;
	HW_U8 lca_clrs_lum_th;
	HW_U8 lca_pf_clrc_ratio;
	HW_U8 lca_gf_clrc_ratio;
	HW_U8 lca_pf_decr_ratio;
};

struct isp_cnr_config {
	HW_U16 c_theshold;
#if (ISP_VERSION >= 521)
	HW_U16 y_theshold;
	HW_U16 st_v_yth;
	HW_U16 st_h_yth;
#endif
};

struct isp_d2d_config {
#if (ISP_VERSION >= 520)
	unsigned char lf_ratio;
	unsigned char bf_ratio;
	unsigned char hf_ratio;
	unsigned char lp0_np_core_ratio;
	unsigned char lp1_np_core_ratio;
	unsigned char lp2_np_core_ratio;
	unsigned char lp3_np_core_ratio;
	unsigned char lp0_np_side_ratio;
	unsigned char lp1_np_side_ratio;
	unsigned char lp2_np_side_ratio;
	unsigned char lp0_pcnt_ratio;
	unsigned char lp1_pcnt_ratio;
	unsigned char lp2_pcnt_ratio;
	unsigned char lp3_pcnt_ratio;

	HW_U16 d2d_lp0_th[ISP_REG_TBL_LENGTH];
	HW_U16 d2d_lp1_th[ISP_REG_TBL_LENGTH];
	HW_U16 d2d_lp2_th[ISP_REG_TBL_LENGTH];
	HW_U16 d2d_lp3_th[ISP_REG_TBL_LENGTH];
#endif
	HW_U16 bdnf_th[ISP_REG_TBL_LENGTH];//rbw[5]+gw[7]
};

struct isp_tdnf_config {
	HW_U8 rec_en;
	HW_U16 noise_clip_ratio;
#if (ISP_VERSION >= 520)
	unsigned char bright_diff_ratio;
	unsigned char bright_diff_clip_ratio;
	unsigned char lum_diff_clip_ratio;
	unsigned char st_2d_ratio;
	unsigned char mv_ori_ratio;
	unsigned char ltf_en;
	unsigned char k3d_increase_mode;
	unsigned short c_weight1;
	unsigned short c_weight2;
	unsigned short c_weight3;
	unsigned short ltf_update_frm;
	HW_U16 tdnf_lum_th[ISP_REG_TBL_LENGTH];
	HW_U16 tdnf_bri_th[ISP_REG_TBL_LENGTH];
	HW_U8 tdnf_k_delta[ISP_REG_TBL_LENGTH-1];
#else
	HW_U16 diff_clip_ratio;
	HW_U16 k_3d_s;
	HW_U16 diff_cal_mode;
#endif
	HW_U16 tdnf_th[ISP_REG_TBL_LENGTH];
	HW_U16 tdnf_ref_noise[ISP_REG_TBL_LENGTH];
	HW_U8 tdnf_k[ISP_REG_TBL_LENGTH-1];
};

struct isp_contrast_config {
	HW_U16 contrast_min_val;
	HW_U16 contrast_max_val;
	HW_U16 black_clip;
	HW_U16 white_clip;
	HW_U16 black_level;
	HW_U16 white_level;
	HW_U16 plat_th;
	HW_U16 contrast_val[ISP_REG_TBL_LENGTH];
	HW_U16 contrast_lum[ISP_REG_TBL_LENGTH];
};

struct isp_offset {
	HW_S16 r_offset;
	HW_S16 gr_offset;
	HW_S16 gb_offset;
	HW_S16 b_offset;
};

struct isp_dg_gain {
	HW_U16 r_gain;
	HW_U16 gr_gain;
	HW_U16 gb_gain;
	HW_U16 b_gain;
};

struct isp_pltm_config {
	HW_U8 lss_switch;
	HW_U8 cal_en;
	HW_U8 frm_sm_en;
	HW_U8 last_order_ratio;
	HW_U8 tr_order;
	HW_U8 oripic_ratio;

	HW_U8 intens_asym;
	HW_U8 spatial_asm;
	HW_U8 white_level;
	HW_U8 lp_halo_res;
	HW_U8 lum_ratio;

	HW_U8 block_height;
	HW_U8 block_width;
	HW_U8 block_v_num;
	HW_U8 block_h_num;

	HW_U32 statistic_div;

 	HW_U8 pltm_table[ISP_PLTM_MEM_SIZE];
};

struct isp_cfa_config {
	HW_S32 min_rgb;
	HW_U16 dir_th;
#if (ISP_VERSION >= 521)
	HW_U8 interp_mode;
	HW_U8 zig_zag;
#endif
};

struct isp_sharp_config {
#if (ISP_VERSION >= 520)
	unsigned short edge_black_stren;
	unsigned short edge_white_stren;
	unsigned short hfrq_black_stren;
	unsigned short hfrq_white_stren;
	unsigned char edge_scale_ratio;
	unsigned char hfrq_scale_ratio;
	unsigned char edge_conv_para;
	unsigned char hfrq_conv_para;
	unsigned short ns_lw_th;
	unsigned short ns_hi_th;
	unsigned short dir_clip_val;
	unsigned short dir_eq_ratio;
	unsigned char edge_th;
	unsigned char hv_edge_sm_ratio;
	unsigned char aa_edge_sm_ratio;
	unsigned short over_val_ctrl;
	unsigned short over_area_ctrl;
	unsigned short under_val_ctrl;
	unsigned short under_area_ctrl;

	HW_U16 sharp_edge_lum[ISP_REG_TBL_LENGTH];
	HW_U16 sharp_hfrq_lum[ISP_REG_TBL_LENGTH];
	HW_U16 sharp_hsv[46];
	HW_U8 sharp_s_map[ISP_REG_TBL_LENGTH];
#else
	HW_U16 sharp_min_val;
	HW_U16 sharp_max_val;
	HW_U16 black_clip;
	HW_U16 white_clip;
	HW_U16 black_level;
	HW_U16 white_level;
#endif
	HW_U16 sharp_val[ISP_REG_TBL_LENGTH];
	HW_U16 sharp_lum[ISP_REG_TBL_LENGTH];
};

struct isp_saturation_config {
	HW_S16 satu_r;
	HW_S16 satu_g;
	HW_S16 satu_b;
	HW_S16 satu_gain;
	HW_S16 saturation_table[ISP_SATURATION_MEM_SIZE / 2];
};

#if (ISP_VERSION >= 520)
struct isp_dehaze_config {
	unsigned char dog_width_set;
	unsigned char dog_stat_num_power;
	unsigned short bright_scale;
	unsigned char blc_num_w;
	unsigned char blc_num_h;
	unsigned char blc_min_ratio;
	unsigned char fogw;
	unsigned char hazew;
	unsigned short blc_w;
	unsigned short blc_h;
	unsigned short blc_w_rec;
	unsigned short blc_h_rec;
	unsigned short airlight_r;
	unsigned short airlight_g;
	unsigned short airlight_b;
	unsigned int airlight_r_rec;
	unsigned int airlight_g_rec;
	unsigned int airlight_b_rec;
	unsigned int airlight_stat_num_th;
	unsigned short airlight_stat_value_th;
	unsigned short protect_dark_mean;
	unsigned short protect_proj_mean;
};

struct isp_af_en_config {
	unsigned char af_iir0_en;
	unsigned char af_iir1_en;
	unsigned char af_fir0_en;
	unsigned char af_fir1_en;
	unsigned char af_iir0_sec0_en;
	unsigned char af_iir0_sec1_en;
	unsigned char af_iir0_sec2_en;
	unsigned char af_iir1_sec0_en;
	unsigned char af_iir1_sec1_en;
	unsigned char af_iir1_sec2_en;
	unsigned char af_iir0_ldg_en;
	unsigned char af_iir1_ldg_en;
	unsigned char af_fir0_ldg_en;
	unsigned char af_fir1_ldg_en;
	unsigned char af_iir_ds_en;
	unsigned char af_fir_ds_en;
	unsigned char af_offset_en;
	unsigned char af_peak_en;
	unsigned char af_squ_en;
};

struct isp_af_filter_config {
	unsigned short af_iir0_g0;
	unsigned short af_iir0_g1;
	unsigned short af_iir0_g2;
	unsigned short af_iir0_g3;
	unsigned short af_iir0_g4;
	unsigned short af_iir0_g5;
	unsigned short af_iir1_g0;
	unsigned short af_iir1_g1;
	unsigned short af_iir1_g2;
	unsigned short af_iir1_g3;
	unsigned short af_iir1_g4;
	unsigned short af_iir1_g5;
	unsigned short af_iir0_s0;
	unsigned short af_iir1_s0;
	unsigned short af_iir0_s1;
	unsigned short af_iir1_s1;
	unsigned short af_iir0_s2;
	unsigned short af_iir1_s2;
	unsigned short af_iir0_s3;
	unsigned short af_iir1_s3;
	unsigned char af_fir0_g0;
	unsigned char af_fir0_g1;
	unsigned char af_fir0_g2;
	unsigned char af_fir0_g3;
	unsigned char af_fir0_g4;
	unsigned char af_fir1_g0;
	unsigned char af_fir1_g1;
	unsigned char af_fir1_g2;
	unsigned char af_fir1_g3;
	unsigned char af_fir1_g4;
	unsigned char af_iir0_dilate;
	unsigned char af_iir1_dilate;
	unsigned char af_iir0_ldg_lgain;
	unsigned char af_iir0_ldg_hgain;
	unsigned char af_iir1_ldg_lgain;
	unsigned char af_iir1_ldg_hgain;
	unsigned char af_iir0_ldg_lth;
	unsigned char af_iir0_ldg_hth;
	unsigned char af_iir1_ldg_lth;
	unsigned char af_iir1_ldg_hth;
	unsigned char af_fir0_ldg_lgain;
	unsigned char af_fir0_ldg_hgain;
	unsigned char af_fir1_ldg_lgain;
	unsigned char af_fir1_ldg_hgain;
	unsigned char af_fir0_ldg_lth;
	unsigned char af_fir0_ldg_hth;
	unsigned char af_fir1_ldg_lth;
	unsigned char af_fir1_ldg_hth;
	unsigned char af_iir0_ldg_lslope;
	unsigned char af_iir0_ldg_hslope;
	unsigned char af_iir1_ldg_lslope;
	unsigned char af_iir1_ldg_hslope;
	unsigned char af_fir0_ldg_lslope;
	unsigned char af_fir0_ldg_hslope;
	unsigned char af_fir1_ldg_lslope;
	unsigned char af_fir1_ldg_hslope;
	unsigned char af_iir0_core_th;
	unsigned char af_iir1_core_th;
	unsigned char af_iir0_core_peak;
	unsigned char af_iir1_core_peak;
	unsigned char af_fir0_core_th;
	unsigned char af_fir1_core_th;
	unsigned char af_fir0_core_peak;
	unsigned char af_fir1_core_peak;
	unsigned char af_iir0_core_slope;
	unsigned char af_iir1_core_slope;
	unsigned char af_fir0_core_slope;
	unsigned char af_fir1_core_slope;
	unsigned char af_hlt_th;
	unsigned char af_hlt_cnt_shift;
	unsigned short af_r_offset;
	unsigned short af_g_offset;
	unsigned short af_b_offset;
};
#endif

enum isp_output_speed {
	ISP_OUTPUT_SPEED_0 = 0,
	ISP_OUTPUT_SPEED_1 = 1,
	ISP_OUTPUT_SPEED_2 = 2,
	ISP_OUTPUT_SPEED_3 = 3,
};

struct isp_bayer_gain_offset {
	HW_U16 r_gain;
	HW_U16 gr_gain;
	HW_U16 gb_gain;
	HW_U16 b_gain;

	HW_S16 r_offset;
	HW_S16 gr_offset;
	HW_S16 gb_offset;
	HW_S16 b_offset;
};

struct isp_wb_gain {
	HW_U16 r_gain;
	HW_U16 gr_gain;
	HW_U16 gb_gain;
	HW_U16 b_gain;
};

/**
 * struct isp_rgb2rgb_gain_offset - RGB to RGB Blending
 * @matrix:
 *              [RR] [GR] [BR]
 *              [RG] [GG] [BG]
 *              [RB] [GB] [BB]
 * @offset: Blending offset value for R,G,B.
 */
struct isp_rgb2rgb_gain_offset {
	HW_S16 matrix[3][3];
	HW_S16 offset[3];
};

struct isp_rgb2yuv_gain_offset {
	HW_S16 matrix[3][3];
	HW_S16 offset[3];
};

struct isp_lsc_config {
	HW_U16 ct_x;
	HW_U16 ct_y;
	HW_U16 rs_val;
};

struct isp_disc_config {
	HW_U16 disc_ct_x;
	HW_U16 disc_ct_y;
	HW_U16 disc_rs_val;
};

struct isp_h3a_reg_win {
	HW_U8 hor_num;
	HW_U8 ver_num;
	HW_U32 width;
	HW_U32 height;
	HW_U32 hor_start;
	HW_U32 ver_start;
};

struct isp_h3a_coor_win {
	HW_S32 x1;
	HW_S32 y1;
	HW_S32 x2;
	HW_S32 y2;
};

typedef struct isp_sensor_info {
	/*frome sensor*/
	char *name;
	HW_S32 hflip;
	HW_S32 vflip;
	HW_U32 hts;
	HW_U32 vts;
	HW_U32 pclk;
	HW_U32 fps_fixed;
	HW_U32 bin_factor;
	HW_U32 gain_min;
	HW_U32 gain_max;
	HW_U32 exp_min;
	HW_U32 exp_max;
	HW_S32 sensor_width;
	HW_S32 sensor_height;
	HW_U32 hoffset;
	HW_U32 voffset;
	HW_U32 input_seq;
	HW_U32 wdr_mode;
	HW_U32 color_space;

	/*from ae*/
	HW_U32 exp_line;
	HW_U32 ang_gain;
	HW_U32 dig_gain;

	HW_U32 ae_tbl_idx;
	HW_U32 ae_tbl_idx_max;

	HW_U32 fps;
	HW_U32 frame_time;
	HW_S32 ae_gain;
	HW_U8 is_ae_done;
	HW_U8 backlight;

	/*from motion detect*/
	HW_S32 motion_flag;

	/*awb*/
	HW_S32 ae_lv;
	struct isp_bayer_gain_offset gain_offset;

	/*from af*/
	HW_S32 is_af_busy;
} isp_sensor_info_t;

enum exposure_cfg_type {
	ANTI_EXP_WIN_OVER     = 0,
	ANTI_EXP_WIN_UNDER = 1,
	ANTI_EXP_HIST_OVER = 2,
	ANTI_EXP_HIST_UNDER = 3,

	AE_PREVIEW_SPEED = 4,
	AE_CAPTURE_SPEED = 5,
	AE_VIDEO_SPEED = 6,
	AE_TOUCH_SPEED = 7,
	AE_TOLERANCE = 8,
	AE_TARGET = 9,

	AE_HIST_DARK_WEIGHT_MIN = 10,
	AE_HIST_DARK_WEIGHT_MAX = 11,
	AE_HIST_BRIGHT_WEIGHT_MIN = 12,
	AE_HIST_BRIGHT_WEIGHT_MAX = 13,

	ISP_EXP_CFG_MAX,
};

enum wdr_cfg_type {
	WDR_EXP_RATIO     = 0,
	WDR_LOW_TH = 1,
	WDR_HI_TH = 2,

	ISP_WDR_CFG_MAX,
};

enum ae_table_mode {
	SCENE_MODE_PREVIEW = 0,
	SCENE_MODE_CAPTURE,
	SCENE_MODE_VIDEO,

	SCENE_MODE_BACKLIGHT,
	SCENE_MODE_BEACH_SNOW,
	SCENE_MODE_FIREWORKS,
	SCENE_MODE_LANDSCAPE,
	SCENE_MODE_NIGHT,
	SCENE_MODE_SPORTS,

	SCENE_MODE_USER_DEF0,
	SCENE_MODE_USER_DEF1,
	SCENE_MODE_USER_DEF2,
	SCENE_MODE_USER_DEF3,
	SCENE_MODE_USER_DEF4,
	SCENE_MODE_USER_DEF5,
	SCENE_MODE_SENSOR_DRIVER,

	SCENE_MODE_MAX,
};

struct ae_table {
	HW_U32 min_exp;  //us
	HW_U32 max_exp;
	HW_U32 min_gain;
	HW_U32 max_gain;
	HW_U32 min_iris;
	HW_U32 max_iris;
};

struct ae_table_info {
	struct ae_table ae_tbl[10];
	HW_S32 length;
	HW_S32 ev_step;
	HW_S32 shutter_shift;
};

#endif //__BSP__ISP__COMM__H

