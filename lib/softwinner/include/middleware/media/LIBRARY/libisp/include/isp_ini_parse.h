
/*
 ******************************************************************************
 *
 * isp_ini_parse.h
 *
 * Hawkview ISP - isp_ini_parse.h module
 *
 * Copyright (c) 2015 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Version		  Author         Date		    Description
 *
 *   3.0		  Yang Feng   	2015/11/23	ISP Tuning Tools Support
 *
 ******************************************************************************
 */

#ifndef _ISP_INI_PARSE_H_
#define _ISP_INI_PARSE_H_
#include "isp_manage.h"
#include "isp_debug.h"

struct isp_cfg_pt
{
	struct isp_test_param          *isp_test_settings;
	struct isp_3a_param            *isp_3a_settings;
	struct isp_tunning_param     *isp_tunning_settings;
	struct isp_dynamic_param           *isp_iso_settings;
};

struct isp_cfg_array
{
	char sensor_name[20];
	char isp_cfg_name[50];
	int width;
	int height;
	int fps;
	int wdr;
	int ir;
	struct isp_cfg_pt *cfg;
};
int parser_sync_info(struct isp_param_config *param, char *isp_cfg_name, int isp_id);
int parser_ini_info(struct isp_param_config *param, char *sensor_name,
			int w, int h, int fps, int wdr, int ir, int sync_mode, int isp_id);

#endif	/*_ISP_INI_PARSE_H_*/

