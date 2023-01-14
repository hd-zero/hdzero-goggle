
/*
 ******************************************************************************
 *
 * isp_dev.h
 *
 * Hawkview ISP - isp_dev.h module
 *
 * Copyright (c) 2016 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Version		  Author         Date		    Description
 *
 *   3.0		  Yang Feng   	2016/05/11	VIDEO INPUT
 *
 *****************************************************************************
 */

#ifndef _HWISP_H_
#define _HWISP_H_

#include <sys/ioctl.h>
#include <sys/time.h>
#include <linux/v4l2-mediabus.h>
#include <sys/select.h>
#include <stdbool.h>
#include <pthread.h>

#include "../../isp_version.h"
#include "../../isp_dev/media.h"
#include "../V4l2Camera/sunxi_camera_v2.h"
#include "../isp_debug.h"
#include "video.h"

extern unsigned int isp_dev_log_param;

#if (ISP_VERSION >= 520)
#define HW_ISP_DEVICE_NUM	1
#else
#define HW_ISP_DEVICE_NUM	2
#endif

#define PLANE_NUM		1

struct isp_buffer {
	void *start[PLANE_NUM];
	int length[PLANE_NUM];
};

struct hw_isp_device {
	unsigned int id;
	struct media_entity sensor;
	struct media_entity subdev;
	struct media_entity stat;
	const struct isp_dev_operations *ops;

	int load_type;

	unsigned int size;
	void *buffer;

	void *priv;
	void *ctx;
	void *tuning;
};

struct hw_isp_media_dev {
	struct media_device *mdev;
	struct hw_isp_device *isp_dev[HW_ISP_DEVICE_NUM];
	struct isp_video_device *video_dev[HW_VIDEO_DEVICE_NUM];
	pthread_t isp_tid[HW_ISP_DEVICE_NUM];
	unsigned int isp_use_cnt[HW_ISP_DEVICE_NUM];
	int isp_sync_mode;
};

enum hw_isp_event_type {
	HW_ISP_EVENT_READ = 1,
	HW_ISP_EVENT_WRITE = 2,
	HW_ISP_EVENT_EXCEPTION = 4,
};

struct isp_dev_operations {
	void (*stats_ready)(struct hw_isp_device *isp, const void *buffer);

	void (*fsync)(struct hw_isp_device *isp, struct v4l2_event *event);

	void (*stream_off)(struct hw_isp_device *isp);

	void (*ctrl_process)(struct hw_isp_device *isp,	struct v4l2_event *event);

	void (*monitor_fd)(int id, int fd, enum hw_isp_event_type type,
			 void(*callback)(void *priv), void *priv);
	void (*unmonitor_fd)(int id, int fd);
};

struct hw_isp_media_dev *isp_md_open(const char *devname);
void isp_md_close(struct hw_isp_media_dev *isp_md);
int isp_dev_open(struct hw_isp_media_dev *isp_md, int id);
void isp_dev_close(struct hw_isp_media_dev *isp_md, int id);
int isp_video_open(struct hw_isp_media_dev *isp_md, unsigned int id);
int isp_video_to_isp_id(int video_id);
int isp_video_to_sensor_name(int video_id, char *sensor_name);
void isp_video_close(struct hw_isp_media_dev *isp_md, unsigned int id);
void isp_dev_register(struct hw_isp_device *isp, const struct isp_dev_operations *ops);
int isp_dev_start(struct hw_isp_device *isp);
int isp_dev_stop(struct hw_isp_device *isp);
void isp_dev_banding_ctx(struct hw_isp_device *isp, void *priv_lib);
void isp_dev_unbanding_ctx(struct hw_isp_device *isp);
void *isp_dev_get_ctx(struct hw_isp_device *isp);
void isp_dev_unbanding_tuning(struct hw_isp_device *isp);
void isp_dev_banding_tuning(struct hw_isp_device *isp, void *tuning);
void *isp_dev_get_tuning(struct hw_isp_device *isp);
char *isp_dev_get_sensor_name(struct hw_isp_device *isp);

/* Processing parameters */
int isp_set_load_reg(struct hw_isp_device *isp, struct isp_table_reg_map *reg);
int isp_set_table1_map(struct hw_isp_device *isp, struct isp_table_reg_map *tbl);
int isp_set_table2_map(struct hw_isp_device *isp, struct isp_table_reg_map *tbl);

int isp_sensor_get_exposure(struct hw_isp_device *isp, unsigned int *exposure);
int isp_sensor_set_exposure(struct hw_isp_device *isp, unsigned int exposure);
int isp_sensor_set_gain(struct hw_isp_device *isp, unsigned int gain);
int isp_sensor_get_configs(struct hw_isp_device *isp, struct sensor_config *cfg);
int isp_sensor_set_exp_gain(struct hw_isp_device *isp, struct sensor_exp_gain *exp_gain);
int isp_sensor_set_fps(struct hw_isp_device *isp, struct sensor_fps *fps);
int isp_sensor_get_temp(struct hw_isp_device *isp, struct sensor_temp *temp);

int isp_act_init_range(struct hw_isp_device *isp, unsigned int min, unsigned int max);
int isp_act_set_pos(struct hw_isp_device *isp, unsigned int pos);

#endif /*_HWISP_H_*/

