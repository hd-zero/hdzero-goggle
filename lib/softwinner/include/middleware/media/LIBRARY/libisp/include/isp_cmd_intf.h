
/*
 ******************************************************************************
 *
 * isp_cmd_intf.h
 *
 * Hawkview ISP - isp_cmd_intf.h module
 *
 * Copyright (c) 2016 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Version		  Author         Date		    Description
 *
 *   3.0		  Yang Feng   	2016/03/23	VIDEO INPUT
 *
 *****************************************************************************
 */
#ifndef _ISP_CMD_INTF_H_
#define _ISP_CMD_INTF_H_

void isp_s_brightness(struct isp_lib_context *isp_gen, int value);
void isp_s_contrast(struct isp_lib_context *isp_gen, int value);
void isp_s_saturation(struct isp_lib_context *isp_gen, int value);
void isp_s_hue(struct isp_lib_context *isp_gen, int value);
void isp_s_auto_white_balance(struct isp_lib_context *isp_gen, int value);
void isp_s_exposure(struct isp_lib_context *isp_gen, int value);
void isp_s_auto_gain(struct isp_lib_context *isp_gen, int value);
void isp_s_gain(struct isp_lib_context *isp_gen, int value);
void isp_s_power_line_frequency(struct isp_lib_context *isp_gen, int value);
void isp_s_white_balance_temperature(struct isp_lib_context *isp_gen, int value);
void isp_s_sharpness(struct isp_lib_context *isp_gen, int value);
void isp_s_auto_brightness(struct isp_lib_context *isp_gen,	int value);
void isp_s_band_stop_filter(struct isp_lib_context *isp_gen, int value);
void isp_s_illuminators_1(struct isp_lib_context *isp_gen, int value);
void isp_s_illuminators_2(struct isp_lib_context *isp_gen, int value);
void isp_s_af_metering_mode(struct isp_lib_context	*isp_gen, int value, struct isp_h3a_coor_win *coor);
void isp_s_flash_mode(struct isp_lib_context *isp_gen, int value);
void isp_s_ae_metering_mode(struct isp_lib_context *isp_gen, int value);
void isp_s_ae_roi(struct isp_lib_context *isp_gen, int value, struct isp_h3a_coor_win *coor);
void isp_s_light_mode(struct isp_lib_context *isp_gen, int value);
void isp_s_exposure_auto(struct isp_lib_context *isp_gen, int value);
void isp_s_exposure_absolute(struct isp_lib_context *isp_gen, int value);
void isp_s_aperture(struct isp_lib_context *isp_gen, int value);
void isp_s_focus_absolute(struct isp_lib_context *isp_gen, int value);
void isp_s_focus_relative(struct isp_lib_context *isp_gen, int value);
void isp_s_focus_auto(struct isp_lib_context *isp_gen, int value);
void isp_s_auto_exposure_bias(struct isp_lib_context *isp_gen, int value);
void isp_s_auto_n_preset_white_balance(struct isp_lib_context *isp_gen, int value);
void isp_s_iso_sensitivity(struct isp_lib_context *isp_gen, int value);
void isp_s_iso_sensitivity_auto(struct isp_lib_context *isp_gen, int value);
void isp_s_scene_mode(struct isp_lib_context *isp_gen, int value);
void isp_s_3a_lock(struct isp_lib_context *isp_gen, int value);
void isp_s_auto_focus_start(struct isp_lib_context *isp_gen,	int value);
void isp_s_auto_focus_stop(struct isp_lib_context *isp_gen,	int value);
void isp_s_auto_focus_status(struct isp_lib_context *isp_gen, int value);
void isp_s_auto_focus_range(struct isp_lib_context *isp_gen, int value);
void isp_s_r_gain(struct isp_lib_context *isp_gen, int value);
void isp_s_g_gain(struct isp_lib_context *isp_gen, int value);
void isp_s_b_gain(struct isp_lib_context *isp_gen, int value);

#endif /*_ISP_CMD_INTF_H_*/

