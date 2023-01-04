#include "playback.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <ctype.h>
#include <math.h>

#include <log/log.h>

#include "style.h"
#include "ui/page_common.h"
#include "mplayer.h"
#include "common.hh"

///////////////////////////////////////////////////////////////////////////
//locals
static media_db_t  media_db ;
static pb_ui_item_t pb_ui[ITEMS_LAYOUT_CNT];

LV_IMG_DECLARE(img_arrow1);

void create_pb_item(lv_obj_t *parent)
{
    uint8_t pos;
    for(pos = 0; pos < ITEMS_LAYOUT_CNT; pos++) {
        pb_ui[pos]._img = lv_img_create(parent);
        lv_obj_set_size(pb_ui[pos]._img, ITEM_PREVIEW_W,ITEM_PREVIEW_H);
        lv_obj_add_flag(pb_ui[pos]._img, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_style(pb_ui[pos]._img, &style_pb_dark, LV_PART_MAIN);

        pb_ui[pos]._arrow = lv_img_create(parent);
        lv_img_set_src(pb_ui[pos]._arrow, &img_arrow1);
        lv_obj_add_flag(pb_ui[pos]._arrow, LV_OBJ_FLAG_HIDDEN);


        pb_ui[pos]._label = lv_label_create(parent);
        lv_obj_set_style_text_font(pb_ui[pos]._label, &lv_font_montserrat_26, 0);
        lv_label_set_long_mode(pb_ui[pos]._label, LV_LABEL_LONG_SCROLL_CIRCULAR);
        lv_obj_set_style_text_align(pb_ui[pos]._label, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_add_flag(pb_ui[pos]._label, LV_OBJ_FLAG_HIDDEN);


        pb_ui[pos].x = pos % ITEMS_LAYOUT_COLS;
        pb_ui[pos].y = (uint8_t)floor((double)pos / ITEMS_LAYOUT_COLS);
        
        pb_ui[pos].x = PB_X_START + pb_ui[pos].x * (ITEM_PREVIEW_W+ITEM_GAP_W);
        pb_ui[pos].y = PB_Y_START + pb_ui[pos].y * (ITEM_PREVIEW_H+ITEM_GAP_H);
        pb_ui[pos].state = ITEM_STATE_INVISIBLE;

        lv_obj_set_pos(pb_ui[pos]._img,   pb_ui[pos].x+(ITEM_GAP_W>>1), pb_ui[pos].y);

        lv_obj_set_pos(pb_ui[pos]._arrow, pb_ui[pos].x+(ITEM_PREVIEW_W>>2)-10, \
                                          pb_ui[pos].y+ITEM_PREVIEW_H+10);
        
        lv_obj_set_pos(pb_ui[pos]._label, pb_ui[pos].x+(ITEM_PREVIEW_W>>2)+ITEM_GAP_W, \
                                          pb_ui[pos].y+ITEM_PREVIEW_H+10);
        
        media_db.head = (media_file_node_t*)malloc(MAX_VIDEO_FILES*sizeof(media_file_node_t));
        if(media_db.head == NULL) {
            perror("playback malloc failed.");
        }
    }                                        
 }

bool is_file_exist(char* fname)
{
    FILE *fp = fopen(fname, "r");
    if (fp != NULL) {
        fclose(fp);
        return true;
    }
    else return false;
}

void show_pb_item(uint8_t pos, char* label)
{
    char fname[256];
    if(pb_ui[pos].state == ITEM_STATE_INVISIBLE) {
        lv_obj_add_flag(pb_ui[pos]._img, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(pb_ui[pos]._label, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(pb_ui[pos]._arrow, LV_OBJ_FLAG_HIDDEN);
    }
    else {
        lv_label_set_text(pb_ui[pos]._label, label);
        lv_obj_clear_flag(pb_ui[pos]._label, LV_OBJ_FLAG_HIDDEN);
    
    
        sprintf(fname,"%s/%s.jpg",TMP_DIR,label);
        if(is_file_exist(fname))
            sprintf(fname,"A:%s/%s.jpg",TMP_DIR,label);
        else 
            sprintf(fname,"%s%s",RESOURCE_PATH,DEF_VIDEOICON);    
        lv_img_set_src(pb_ui[pos]._img, fname);
        
        if(pb_ui[pos].state == ITEM_STATE_HIGHLIGHT){
            lv_obj_remove_style(pb_ui[pos]._img,&style_pb_dark,LV_PART_MAIN);
            lv_obj_add_style(pb_ui[pos]._img, &style_pb, LV_PART_MAIN);
            lv_obj_clear_flag(pb_ui[pos]._arrow, LV_OBJ_FLAG_HIDDEN);
        }
        else{    
            lv_obj_remove_style(pb_ui[pos]._img,&style_pb,LV_PART_MAIN);
            lv_obj_add_style(pb_ui[pos]._img, &style_pb_dark, LV_PART_MAIN);
            lv_obj_add_flag(pb_ui[pos]._arrow, LV_OBJ_FLAG_HIDDEN);
        }
        lv_obj_clear_flag(pb_ui[pos]._img, LV_OBJ_FLAG_HIDDEN);
    }
}

////////////////////////////////////////////////////////////////////////////////
// create video files list
int insert_to_list(char* fname, char* label, int size)
{
    media_file_node_t* pnode = &media_db.head[media_db.count];
    strcpy(pnode->filename, fname);
    strcpy(pnode->label, label);
    pnode->size = size;

    LOGI("%d: %s-%dMB", media_db.count,fname,size);
    media_db.count++;
    return 1;
}

void free_list()
{
    media_db.count = 0;
    media_db.cur_sel = 0;
}

int get_videofile_cnt()
{
    return media_db.count;
}

void clear_videofile_cnt()
{
    free_list();
}

media_file_node_t* get_list(int seq)
{
    int seq_reserve = media_db.count-1-seq;
    return &media_db.head[seq_reserve];
}
 
static bool get_seleteced(int seq, char* fname)
{
    media_file_node_t* pnode = get_list(seq);
    if(!pnode) return false;
    sprintf(fname, "%s/%s", MEDIA_FILES_DIR,pnode->filename);
    return true;
}

int walk_sdcard()
{
     DIR* FD;
    struct dirent* in_file;
    FILE*   entry_file;
    int  i,j,k,len;
    char label[64],fname[128];
    long int res;

    media_db.count = 0;
    media_db.cur_sel = 0;

        /* Scanning the in directory */
    if (NULL == (FD = opendir (MEDIA_FILES_DIR))) 
        return 0;
    
    while ((in_file = readdir(FD))) {
        if (!strcmp (in_file->d_name, "."))
            continue;
        if (!strcmp (in_file->d_name, ".."))    
            continue;

        // .ts or .mp4 only
        len = strlen(in_file->d_name);
        for(i=len-1;i>0;i--) {
            if(in_file->d_name[i] == '.')
                break;
        }
        if(i==0) continue;

        k=i; //save the '.' position

        j=0;    // .TS and .MP4 only
        while(i<len)
            label[j++] = toupper(in_file->d_name[i++]);
        label[j] = 0;
        if((strcmp(label,".TS") != 0) && (strcmp(label,".MP4") != 0)) continue;

        strncpy(label,in_file->d_name,k);
        label[k] = 0;
        
        sprintf(fname, "%s/%s", MEDIA_FILES_DIR, in_file->d_name);
        entry_file = fopen(fname, "r");
        if(!entry_file) continue;
        fseek(entry_file, 0L, SEEK_END);
        res = ftell(entry_file);
        fclose(entry_file);
        res >>= 20; //in MB
        if(res < 5) //skip small files
            continue;

        if(media_db.count < MAX_VIDEO_FILES) 
            insert_to_list(in_file->d_name, label, (int)res);
        else 
            LOGI("max video file cnt reached %d,skipped",MAX_VIDEO_FILES);
    }
    closedir(FD);

    //copy all thumbnail files to /tmp
    sprintf(fname,"cp %s/*.jpg %s",MEDIA_FILES_DIR,TMP_DIR);
    system(fname);

    return media_db.count;
}

////////////////////////////////////////////////////////////////////////////////
// 
void update_page()
{
    media_file_node_t* pnode;
    int i,seq,page_num = (int)floor((double)media_db.cur_sel/ITEMS_LAYOUT_CNT); 
    int end_pos = media_db.count - page_num*ITEMS_LAYOUT_CNT;
    int cur_pos = media_db.cur_sel - page_num*ITEMS_LAYOUT_CNT;


    for(i=0;i<ITEMS_LAYOUT_CNT;i++) {
        seq = i+page_num*ITEMS_LAYOUT_CNT;
        if (seq < media_db.count) {
            pnode = get_list(seq);
            if(!pnode) {
                perror("update_page failed.");
                return;
            }

            if(i < cur_pos)
                pb_ui[i].state = ITEM_STATE_NORMAL;
            else if(i == cur_pos)   
                pb_ui[i].state = ITEM_STATE_HIGHLIGHT;
            else if(i < end_pos )   
                pb_ui[i].state = ITEM_STATE_NORMAL;
            else     
                pb_ui[i].state = ITEM_STATE_INVISIBLE;
            
            show_pb_item(i, pnode->label);
        }
		else {
			pb_ui[i].state = ITEM_STATE_INVISIBLE;
			show_pb_item(i, NULL);
		}
    }
}

int init_pb()
{
    int ret;
    ret = walk_sdcard();
    update_page();
    return ret;    
}

//////////////////////////////////////////////////////////////////////////////////
// key:
//  1 = dial up
//  2 = dial down
//  3 = click
//  4 = long press
void onkey(uint8_t key)
{
    static bool done = true;
    static uint8_t state = 0; //0= select video files, 1=playback
    char fname[128];
    
    //LOGI("onkey:Key=%d,Count=%d",key,media_db.count);    

    if(!media_db.count || !done) return; 

    if(!key) return;

    if(state == 1) {
        if(mplayer_on_key(key)) {
            state = 0;
            g_menu_op = OPLEVEL_SUBMENU;
        }
        return;
    }

    done = false;
    switch(key) {
        case DIAL_KEY_UP: //up
            if(media_db.cur_sel != (media_db.count-1)) {
                media_db.cur_sel++;
                update_page();
            }
            break;
                
        case DIAL_KEY_DOWN: //down
            if(media_db.cur_sel) {
                media_db.cur_sel--;
                update_page();
            }
            break;

        case DIAL_KEY_CLICK: //Enter
            if(get_seleteced(media_db.cur_sel,fname)) {
                mplayer_file(fname);
                state = 1;
                g_menu_op = OPLEVEL_PLAYBACK;
            }
            break;

        case DIAL_KEY_PRESS: //long press
            free_list(); 
            update_page();
            break;
    }
    done = true;
}


