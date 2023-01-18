
#ifndef _ISP_PLTM_H_
#define _ISP_PLTM_H_

typedef struct isp_pltm_ini_cfg {
	HW_S32 pltm_cfg[ISP_PLTM_MAX];
	HW_S32 pltm_dynamic_cfg[ISP_PLTM_DYNAMIC_MAX];
}pltm_ini_cfg_t;

typedef enum isp_pltm_param_type {
	ISP_PLTM_PARAM_TYPE_MAX,
} pltm_param_type_t;

typedef struct isp_pltm_param {
	pltm_param_type_t type;
	int isp_platform_id;
	int pltm_frame_id;
	HW_BOOL pltm_enable;
	HW_BOOL ae_enable;
	pltm_ini_cfg_t pltm_ini;
	isp_sensor_info_t sensor_info;
	HW_U16 *pltm_table;
	int wdr_bit_offset;
} pltm_param_t;

typedef struct isp_pltm_stats {
	struct isp_pltm_stats_s *pltm_stats;
} pltm_stats_t;

typedef struct isp_pltm_result {
	int pltm_last_order_ratio;
	int pltm_tr_order;
	int pltm_oripic_ratio;
	int pltm_cal_en;
	int pltm_frm_sm_en;
	int pltm_block_height;
	int pltm_block_width;
	HW_U32 pltm_statistic_div;
	HW_U16 pltm_tbl[768];
	HW_U8 pltm_ae_comp;
	HW_U16 pltm_old_stren;
	HW_U16 pltm_next_stren;
	HW_U16 pltm_cal_stren;
	HW_U16 pltm_min_th;
} pltm_result_t;

typedef struct isp_pltm_core_ops {
	int (*isp_pltm_set_params)(void *pltm_core_obj, pltm_param_t *param, pltm_result_t *result);
	int (*isp_pltm_get_params)(void *pltm_core_obj, pltm_param_t **param);
	int (*isp_pltm_run)(void *pltm_core_obj, pltm_stats_t *stats, pltm_result_t *result);
} isp_pltm_core_ops_t;

void* pltm_init(isp_pltm_core_ops_t **pltm_core_ops);
void  pltm_exit(void *pltm_core_obj);


#endif /*_ISP_TONE_MAPPING_H_*/


