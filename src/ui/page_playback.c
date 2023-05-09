#include "page_playback.h"

#include <ctype.h>
#include <dirent.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <log/log.h>

#include "common.hh"
#include "core/app_state.h"
#include "core/osd.h"
#include "ui/page_common.h"
#include "ui/ui_player.h"
#include "ui/ui_style.h"
#include "util/file.h"
#include "util/math.h"

LV_IMG_DECLARE(img_arrow1);

static lv_coord_t col_dsc[] = {320, 320, 320, LV_GRID_TEMPLATE_LAST};
static lv_coord_t row_dsc[] = {150, 30, 150, 30, 150, 30, 30, LV_GRID_TEMPLATE_LAST};

static media_db_t media_db;
static pb_ui_item_t pb_ui[ITEMS_LAYOUT_CNT];

static lv_obj_t *page_playback_create(lv_obj_t *parent, panel_arr_t *arr) {
    lv_obj_t *page = lv_menu_page_create(parent, NULL);
    lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(page, 1142, 900);
    lv_obj_add_style(page, &style_subpage, LV_PART_MAIN);
    lv_obj_set_style_pad_top(page, 46, 0);

    lv_obj_t *section = lv_menu_section_create(page);
    lv_obj_add_style(section, &style_submenu, LV_PART_MAIN);
    lv_obj_set_size(section, 1142, 894);

    create_text(NULL, section, false, "Playback:", LV_MENU_ITEM_BUILDER_VARIANT_2);

    lv_obj_t *cont = lv_obj_create(section);
    lv_obj_set_size(cont, 1164, 760);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(cont, &style_context, LV_PART_MAIN);

    for (uint32_t pos = 0; pos < ITEMS_LAYOUT_CNT; pos++) {
        pb_ui[pos]._img = lv_img_create(cont);
        lv_obj_set_size(pb_ui[pos]._img, ITEM_PREVIEW_W, ITEM_PREVIEW_H);
        lv_obj_add_flag(pb_ui[pos]._img, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_style(pb_ui[pos]._img, &style_pb_dark, LV_PART_MAIN);

        pb_ui[pos]._arrow = lv_img_create(cont);
        lv_img_set_src(pb_ui[pos]._arrow, &img_arrow1);
        lv_obj_add_flag(pb_ui[pos]._arrow, LV_OBJ_FLAG_HIDDEN);

        pb_ui[pos]._label = lv_label_create(cont);
        lv_obj_set_style_text_font(pb_ui[pos]._label, &lv_font_montserrat_26, 0);
        lv_label_set_long_mode(pb_ui[pos]._label, LV_LABEL_LONG_SCROLL_CIRCULAR);
        lv_obj_set_style_text_align(pb_ui[pos]._label, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_add_flag(pb_ui[pos]._label, LV_OBJ_FLAG_HIDDEN);

        pb_ui[pos].x = pos % ITEMS_LAYOUT_COLS;
        pb_ui[pos].y = (uint32_t)((double)(pos) / (double)(ITEMS_LAYOUT_COLS));

        pb_ui[pos].x = PB_X_START + pb_ui[pos].x * (ITEM_PREVIEW_W + ITEM_GAP_W);
        pb_ui[pos].y = PB_Y_START + pb_ui[pos].y * (ITEM_PREVIEW_H + ITEM_GAP_H);
        pb_ui[pos].state = ITEM_STATE_INVISIBLE;

        lv_obj_set_pos(pb_ui[pos]._img, pb_ui[pos].x + (ITEM_GAP_W >> 1), pb_ui[pos].y);

        lv_obj_set_pos(pb_ui[pos]._arrow, pb_ui[pos].x + (ITEM_PREVIEW_W >> 2) - 10,
                       pb_ui[pos].y + ITEM_PREVIEW_H + 10);

        lv_obj_set_pos(pb_ui[pos]._label, pb_ui[pos].x + (ITEM_PREVIEW_W >> 2) + ITEM_GAP_W,
                       pb_ui[pos].y + ITEM_PREVIEW_H + 10);
    }

    lv_obj_t *label = lv_label_create(cont);
    lv_label_set_text(label, "*Long press the Enter button to exit\n**Long press the Func button to delete");
    lv_obj_set_style_text_font(label, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_set_style_text_color(label, lv_color_make(255, 255, 255), 0);
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(label, 10, 700);

    return page;
}

static void show_pb_item(uint8_t pos, char *label) {
    char fname[256];
    if (pb_ui[pos].state == ITEM_STATE_INVISIBLE) {
        lv_obj_add_flag(pb_ui[pos]._img, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(pb_ui[pos]._label, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(pb_ui[pos]._arrow, LV_OBJ_FLAG_HIDDEN);
        return;
    }

    lv_label_set_text(pb_ui[pos]._label, label);
    lv_obj_clear_flag(pb_ui[pos]._label, LV_OBJ_FLAG_HIDDEN);

    sprintf(fname, "%s/%s.jpg", TMP_DIR, label);
    if (file_exists(fname))
        sprintf(fname, "A:%s/%s.jpg", TMP_DIR, label);
    else
        osd_resource_path(fname, "%s", OSD_RESOURCE_720, DEF_VIDEOICON);
    lv_img_set_src(pb_ui[pos]._img, fname);

    if (pb_ui[pos].state == ITEM_STATE_HIGHLIGHT) {
        lv_obj_remove_style(pb_ui[pos]._img, &style_pb_dark, LV_PART_MAIN);
        lv_obj_add_style(pb_ui[pos]._img, &style_pb, LV_PART_MAIN);
        lv_obj_clear_flag(pb_ui[pos]._arrow, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_remove_style(pb_ui[pos]._img, &style_pb, LV_PART_MAIN);
        lv_obj_add_style(pb_ui[pos]._img, &style_pb_dark, LV_PART_MAIN);
        lv_obj_add_flag(pb_ui[pos]._arrow, LV_OBJ_FLAG_HIDDEN);
    }
    lv_obj_clear_flag(pb_ui[pos]._img, LV_OBJ_FLAG_HIDDEN);
}

int get_videofile_cnt() {
    return media_db.count;
}

void clear_videofile_cnt() {
    media_db.count = 0;
    media_db.cur_sel = 0;
}

static media_file_node_t *get_list(int seq) {
    int seq_reserve = media_db.count - 1 - seq;
    return &media_db.list[seq_reserve];
}

static bool get_seleteced(int seq, char *fname) {
    media_file_node_t *pnode = get_list(seq);
    if (!pnode)
        return false;
    sprintf(fname, "%s/%s", MEDIA_FILES_DIR, pnode->filename);
    return true;
}

int hot_alphasort(const struct dirent **a, const struct dirent **b) {
    const bool a_hot = strncmp((*a)->d_name, "hot_", 4) == 0;
    const bool b_hot = strncmp((*b)->d_name, "hot_", 4) == 0;
    if (a_hot && !b_hot) {
        return -1;
    }
    if (!a_hot && b_hot) {
        return 1;
    }
    return strcoll((*a)->d_name, (*b)->d_name);
}

static int walk_sdcard() {
    media_db.count = 0;
    media_db.cur_sel = 0;

    struct dirent **namelist;
    int count = scandir(MEDIA_FILES_DIR, &namelist, NULL, hot_alphasort);
    if (count == -1) {
        return 0;
    }

    for (size_t i = 0; i < count; i++) {
        struct dirent *in_file = namelist[i];
        if (in_file->d_name[0] == '.') {
            continue;
        }

        const char *dot = strrchr(in_file->d_name, '.');
        if (dot == NULL) {
            // '.' not found
            continue;
        }

        if (strcasecmp(dot, ".ts") != 0 && strcasecmp(dot, ".mp4") != 0) {
            continue;
        }

        char fname[512];
        sprintf(fname, "%s/%s", MEDIA_FILES_DIR, in_file->d_name);

        long size = file_get_size(fname);
        size >>= 20; // in MB
        if (size < 5) {
            // skip small files
            continue;
        }

        if (media_db.count >= MAX_VIDEO_FILES) {
            LOGI("max video file cnt reached %d,skipped", MAX_VIDEO_FILES);
            continue;
        }

        media_file_node_t *pnode = &media_db.list[media_db.count];
        strcpy(pnode->filename, in_file->d_name);
        strncpy(pnode->label, in_file->d_name, dot - in_file->d_name);
        strcpy(pnode->ext, dot + 1);
        pnode->size = size;

        LOGI("%d: %s-%dMB", media_db.count, pnode->filename, size);

        media_db.count++;
    }

    for (size_t i = 0; i < count; i++) {
        free(namelist[i]);
    }
    free(namelist);

    // copy all thumbnail files to /tmp
    char fname[128];
    sprintf(fname, "cp %s/*.jpg %s", MEDIA_FILES_DIR, TMP_DIR);
    system(fname);

    return media_db.count;
}

static int find_hot_index() {
    DIR *fd = opendir(MEDIA_FILES_DIR);
    if (!fd) {
        return 1;
    }

    int result = 0;
    struct dirent *in_file;
    while ((in_file = readdir(fd))) {
        if (in_file->d_name[0] == '.' || strncmp(in_file->d_name, "hot_", 4) != 0) {
            continue;
        }

        const char *dot = strrchr(in_file->d_name, '.');
        if (dot == NULL) {
            // '.' not found
            continue;
        }

        if (strcasecmp(dot, ".ts") != 0 && strcasecmp(dot, ".mp4") != 0) {
            continue;
        }

        int index = 0;
        if (sscanf(in_file->d_name, "hot_hdz_%d", &index) != 1) {
            continue;
        }

        if (index > result) {
            result = index;
        }
    }
    closedir(fd);

    return result + 1;
}

static void update_page() {
    uint32_t page_num = (uint32_t)floor((double)media_db.cur_sel / ITEMS_LAYOUT_CNT);
    uint32_t end_pos = media_db.count - page_num * ITEMS_LAYOUT_CNT;
    uint32_t cur_pos = media_db.cur_sel - page_num * ITEMS_LAYOUT_CNT;

    for (uint8_t i = 0; i < ITEMS_LAYOUT_CNT; i++) {
        uint32_t seq = i + page_num * ITEMS_LAYOUT_CNT;
        if (seq < media_db.count) {
            media_file_node_t *pnode = get_list(seq);
            if (!pnode) {
                perror("update_page failed.");
                return;
            }

            if (i < cur_pos)
                pb_ui[i].state = ITEM_STATE_NORMAL;
            else if (i == cur_pos)
                pb_ui[i].state = ITEM_STATE_HIGHLIGHT;
            else if (i < end_pos)
                pb_ui[i].state = ITEM_STATE_NORMAL;
            else
                pb_ui[i].state = ITEM_STATE_INVISIBLE;

            show_pb_item(i, pnode->label);
        } else {
            pb_ui[i].state = ITEM_STATE_INVISIBLE;
            show_pb_item(i, NULL);
        }
    }
}

static void mark_video_file(int seq) {
    media_file_node_t *pnode = get_list(seq);
    if (!pnode) {
        return;
    }
    if (strncmp(pnode->filename, "hot_", 4) == 0) {
        // file already marked hot
        return;
    }

    const int index = find_hot_index();

    char cmd[256];
    sprintf(cmd, "mv  %s/%s %s/hot_hdz_%03d.%s", MEDIA_FILES_DIR, pnode->filename, MEDIA_FILES_DIR, index, pnode->ext);
    system(cmd);
    sprintf(cmd, "mv %s/%s.jpg %s/hot_hdz_%03d.jpg", MEDIA_FILES_DIR, pnode->label, MEDIA_FILES_DIR, index);
    system(cmd);

    walk_sdcard();
    media_db.cur_sel = constrain(seq, 0, (media_db.count - 1));
    update_page();
}

static void delete_video_file(int seq) {
    media_file_node_t *pnode = get_list(seq);
    if (!pnode) {
        LOGE("delete_video_file failed. (PNODE ERROR)");
        return;
    }

    char cmd[128];
    sprintf(cmd, "rm %s/%s.*", MEDIA_FILES_DIR, pnode->label);

    if (system(cmd) != -1) {
        walk_sdcard();
        media_db.cur_sel = constrain(seq, 0, (media_db.count - 1));
        update_page();
        LOGD("delete_video_file successful.");
    } else {
        LOGE("delete_video_file failed.");
    }
}

static void page_playback_exit() {
    clear_videofile_cnt();
    update_page();
}

static void page_playback_enter() {
    const int ret = walk_sdcard();
    update_page();

    if (ret == 0) {
        // no files found, back out
        submenu_exit();
    }
}

void pb_key(uint8_t key) {
    static bool done = true;
    static uint8_t state = 0; // 0= select video files, 1=playback
    char fname[128];

    // LOGI("onkey:Key=%d,Count=%d",key,media_db.count);

    if (!key || !media_db.count || !done) {
        return;
    }

    if (state == 1) {
        if (mplayer_on_key(key)) {
            state = 0;
            app_state_push(APP_STATE_SUBMENU);
        }
        return;
    }

    done = false;
    switch (key) {
    case DIAL_KEY_UP: // up
        if (media_db.cur_sel != (media_db.count - 1)) {
            media_db.cur_sel++;
            update_page();
        }
        break;

    case DIAL_KEY_DOWN: // down
        if (media_db.cur_sel) {
            media_db.cur_sel--;
            update_page();
        }
        break;

    case DIAL_KEY_CLICK: // Enter
        if (get_seleteced(media_db.cur_sel, fname)) {
            mplayer_file(fname);
            state = 1;
            app_state_push(APP_STATE_PLAYBACK);
        }
        break;

    case DIAL_KEY_PRESS: // long press
        page_playback_exit();
        break;

    case RIGHT_KEY_CLICK:
        mark_video_file(media_db.cur_sel);
        break;

    case RIGHT_KEY_PRESS:
        delete_video_file(media_db.cur_sel);
        break;
    }
    done = true;
}

static void page_playback_on_roller(uint8_t key) {
    pb_key(key);
}

static void page_playback_on_click(uint8_t key, int sel) {
    pb_key(key);
}

static void page_playback_on_right_button(bool is_short) {
    pb_key(is_short ? RIGHT_KEY_CLICK : RIGHT_KEY_PRESS);
}

page_pack_t pp_playback = {
    .name = "Playback",
    .create = page_playback_create,
    .enter = page_playback_enter,
    .exit = page_playback_exit,
    .on_roller = page_playback_on_roller,
    .on_click = page_playback_on_click,
    .on_right_button = page_playback_on_right_button,
};