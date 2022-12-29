#ifndef __PLAYBACK_H__
#define __PLAYBACK_H__

#include <lvgl/lvgl.h>

#define ITEMS_LAYOUT_ROWS 3
#define ITEMS_LAYOUT_COLS 3
#define ITEMS_LAYOUT_CNT  (ITEMS_LAYOUT_ROWS*ITEMS_LAYOUT_COLS)  

#define ITEM_PREVIEW_W    320
#define ITEM_PREVIEW_H    180
#define ITEM_GAP_W        30
#define ITEM_GAP_H        50
#define PB_X_START        0
#define PB_Y_START        0

#define ITEM_STATE_INVISIBLE    0
#define ITEM_STATE_HIGHLIGHT    1
#define ITEM_STATE_NORMAL       2

#define MAX_VIDEO_FILES   200  

typedef struct _media_file_node{
	char filename[64];
    char label[64];
    int  size;
} media_file_node_t;

typedef struct _media_db{
    media_file_node_t* head;
    int         count;
    int         cur_sel;
} media_db_t;

typedef struct {
	lv_obj_t* _img;
	lv_obj_t* _label;
    lv_obj_t* _arrow;
    uint16_t   x;
    uint16_t   y;
    uint8_t    state; //0: invisible; 1=highlighted; 2= normal
} pb_ui_item_t;

void create_pb_item(lv_obj_t *parent);
void onkey(uint8_t key);
int  init_pb();
int  get_videofile_cnt();
void clear_videofile_cnt();

#endif  //__PLAYBACK_H__