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
#include "ui/page_common.h"
#include "ui/ui_player.h"
#include "ui/ui_style.h"

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
    lv_obj_set_size(cont, 1164, 700);
    lv_obj_set_layout(cont, LV_LAYOUT_GRID);
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
        sprintf(fname, "%s%s", RESOURCE_PATH, DEF_VIDEOICON);
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

static int insert_to_list(char *fname, char *label, int size) {
    media_file_node_t *pnode = &media_db.list[media_db.count];
    strcpy(pnode->filename, fname);
    strcpy(pnode->label, label);
    pnode->size = size;

    LOGI("%d: %s-%dMB", media_db.count, fname, size);
    media_db.count++;
    return 1;
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

static int walk_sdcard() {
    DIR *FD;
    struct dirent *in_file;
    FILE *entry_file;
    int i, j, k, len;
    char label[64], fname[128];
    long int res;

    media_db.count = 0;
    media_db.cur_sel = 0;

    /* Scanning the in directory */
    if (NULL == (FD = opendir(MEDIA_FILES_DIR)))
        return 0;

    while ((in_file = readdir(FD))) {
        if (!strcmp(in_file->d_name, "."))
            continue;
        if (!strcmp(in_file->d_name, ".."))
            continue;

        // .ts or .mp4 only
        len = strlen(in_file->d_name);
        for (i = len - 1; i > 0; i--) {
            if (in_file->d_name[i] == '.')
                break;
        }
        if (i == 0)
            continue;

        k = i; // save the '.' position

        j = 0; // .TS and .MP4 only
        while (i < len)
            label[j++] = toupper(in_file->d_name[i++]);
        label[j] = 0;
        if ((strcmp(label, ".TS") != 0) && (strcmp(label, ".MP4") != 0))
            continue;

        strncpy(label, in_file->d_name, k);
        label[k] = 0;

        sprintf(fname, "%s/%s", MEDIA_FILES_DIR, in_file->d_name);
        entry_file = fopen(fname, "r");
        if (!entry_file)
            continue;
        fseek(entry_file, 0L, SEEK_END);
        res = ftell(entry_file);
        fclose(entry_file);
        res >>= 20;  // in MB
        if (res < 5) // skip small files
            continue;

        if (media_db.count < MAX_VIDEO_FILES)
            insert_to_list(in_file->d_name, label, (int)res);
        else
            LOGI("max video file cnt reached %d,skipped", MAX_VIDEO_FILES);
    }
    closedir(FD);

    // copy all thumbnail files to /tmp
    sprintf(fname, "cp %s/*.jpg %s", MEDIA_FILES_DIR, TMP_DIR);
    system(fname);

    return media_db.count;
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

static void page_playback_exit() {
    clear_videofile_cnt();
    update_page();
}

static void page_playback_enter() {
    const int ret = walk_sdcard();
    update_page();

    if (ret == 0) {
        // no files found, back out
        page_playback_exit();
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
            g_menu_op = OPLEVEL_SUBMENU;
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
            g_menu_op = OPLEVEL_PLAYBACK;
        }
        break;

    case DIAL_KEY_PRESS: // long press
        page_playback_exit();
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

page_pack_t pp_playback = {
    .create = page_playback_create,
    .enter = page_playback_enter,
    .exit = page_playback_exit,
    .on_roller = page_playback_on_roller,
    .on_click = page_playback_on_click,
};