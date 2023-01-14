
/*
 ******************************************************************************
 *
 * isp_base.h
 *
 * Hawkview ISP - isp_base.h module
 *
 * Copyright (c) 2016 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Version		  Author         Date		    Description
 *
 *   3.0		  Yang Feng   	2016/05/09	VIDEO INPUT
 *
 *****************************************************************************
 */
#ifndef _ISP_BASE_H_
#define _ISP_BASE_H_

struct isp_lib_context;
void config_blc(struct isp_lib_context *isp_gen);
void config_dig_gain(struct isp_lib_context *isp_gen, unsigned int exp_digital_gain);
void config_wdr(struct isp_lib_context *isp_gen, int flag);
void config_gamma(struct isp_lib_context *isp_gen) ;
void config_lens_table(struct isp_lib_context * isp_gen, int vcm_std_pos);
void config_lens_center(struct isp_lib_context * isp_gen);
void config_band_step(struct isp_lib_context * isp_gen);
void isp_handle_stats(struct isp_lib_context *isp_gen, void *buffer);
void isp_handle_stats_sync(struct isp_lib_context *isp_gen, void *buffer0, void *buffer1);
void isp_apply_colormatrix(struct isp_lib_context *isp_gen);
void isp_apply_settings(struct isp_lib_context *isp_gen);
void __isp_stat_dynamic_judge(struct isp_lib_context *isp_gen);

#endif /*_ISP_BASE_H_*/
