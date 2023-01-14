
#ifndef __VIDEO_H_
#define __VIDEO_H_

#include <stdbool.h>

#include "../V4l2Camera/sunxi_camera_v2.h"

#define HW_VIDEO_DEVICE_NUM 4

struct video_plane {
	unsigned int size;
	int dma_fd;
	void *mem;
	unsigned int  mem_phy;
};

struct video_buffer {
	unsigned int index;
	unsigned int bytesused;
	unsigned int frame_cnt;
	unsigned int exp_time;
	struct timeval timestamp;
	bool error;
	bool allocated;
	unsigned int nplanes;
	struct video_plane *planes;
};

struct buffers_pool {
	unsigned int nbufs;
	struct video_buffer *buffers;
};

struct video_fmt {
	enum v4l2_buf_type type;
	enum v4l2_memory memtype;
	struct v4l2_pix_format_mplane format;
	unsigned int nbufs;
	unsigned int nplanes;
	unsigned int fps;
	unsigned int capturemode;
	unsigned int use_current_win;
	unsigned int wdr_mode;
	unsigned int drop_frame_num;
};

struct osd_fmt {
	int clipcount;		/* number of clips */
	int chromakey;
	int global_alpha;
	int inv_w_rgn[8];
	int inv_h_rgn[8];
	int inv_th;
	int reverse_close[64];	/*osd reverse close, 1:close, 0:open*/
	int rgb_cover[8];
	int glb_alpha[64];
	void *bitmap[64];
	struct v4l2_rect region[64];	/* overlay or cover win */
};

struct video_event {
	unsigned int event_id;
	unsigned int event_type;
	unsigned int frame_cnt;
	struct timespec vsync_ts;
};

struct isp_video_device {
	unsigned int id;
	unsigned int isp_id;
	struct media_entity *entity;

	enum v4l2_buf_type type;
	enum v4l2_memory memtype;

	struct v4l2_pix_format_mplane format;

	unsigned int nbufs;
	unsigned int nplanes;
	unsigned int capturemode;
	unsigned int use_current_win;
	unsigned int wdr_mode;
	struct buffers_pool *pool;
	unsigned int fps;
	unsigned int drop_frame_num;

	void *priv;
};

int video_init(struct isp_video_device *video);
void video_cleanup(struct isp_video_device *video);
int video_to_isp_id(struct isp_video_device *video);
int video_set_fmt(struct isp_video_device *video, struct video_fmt *vfmt);
int video_get_fmt(struct isp_video_device *video, struct video_fmt *vfmt);
int video_req_buffers(struct isp_video_device *video, struct buffers_pool *pool);
int video_free_buffers(struct isp_video_device *video);
int video_wait_buffer(struct isp_video_device *video, int timeout);
int video_dequeue_buffer(struct isp_video_device *video,	struct video_buffer *buffer);
int video_queue_buffer(struct isp_video_device *video, unsigned int buf_id);
int video_stream_on(struct isp_video_device *video);
int video_stream_off(struct isp_video_device *video);

struct buffers_pool *buffers_pool_new(struct isp_video_device *video);
void buffers_pool_delete(struct isp_video_device *video);
int video_save_frames(struct isp_video_device *video, unsigned int buf_id, char *path);

int overlay_set_fmt(struct isp_video_device *video, struct osd_fmt *ofmt);
int overlay_update(struct isp_video_device *video, int on_off);

int video_set_control(struct isp_video_device *video, int cmd, int value);
int video_get_control(struct isp_video_device *video, int cmd, int *value);
int video_query_control(struct isp_video_device *video, struct v4l2_queryctrl *ctrl);
int video_query_menu(struct isp_video_device *video, struct v4l2_querymenu *menu);
int video_get_controls(struct isp_video_device *video, unsigned int count,
		      struct v4l2_ext_control *ctrls);
int video_set_controls(struct isp_video_device *video, unsigned int count,
		      struct v4l2_ext_control *ctrls);

int video_event_subscribe(struct isp_video_device *video, unsigned int type);
int video_event_unsubscribe(struct isp_video_device *video, unsigned int type);
int video_wait_event(struct isp_video_device *video);
int video_dequeue_event(struct isp_video_device *video, struct video_event *vi_event);
int video_set_top_clk(struct isp_video_device *video, unsigned int rate);
int video_set_vin_reset_time(struct isp_video_device *video, unsigned int time);

#endif /* __VIDEO_H_ */
