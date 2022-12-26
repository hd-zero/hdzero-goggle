#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "../minIni/minIni.h"

#include "common.hh"
#include "style.h"
#include "../page/page_version.h"
#include "../page/page_common.h"
#include "../driver/i2c.h"
#include "../driver/uart.h"
#include "../driver/fans.h"
#include "../driver/dm5680.h"
#include "../core/main_menu.h"

static lv_coord_t col_dsc[] = {160,160,160,160,160,160,160, LV_GRID_TEMPLATE_LAST};
static lv_coord_t row_dsc[] = {60,60,60,60,60,60,60,60,60,60, LV_GRID_TEMPLATE_LAST};


static lv_obj_t *bar0 = NULL;
static lv_obj_t *bar1 = NULL;
static lv_obj_t *label0 = NULL;
static lv_obj_t *label1 = NULL;

static btn_group_t btn_group0;
static btn_group_t btn_group1;

static slider_group_t slider_group0;
static slider_group_t slider_group1;

#define ADDR_AL 0x65
#define ADDR_FPGA 0x64
#define I2C_Write(s, a, d)  i2c_write(2, s, a, d)
#define I2C_Read(s, a)      i2c_read(2, s, a)

static bool is_need_update_progress = false;
static bool reboot_flag = false;
static lv_obj_t* cur_ver_label;

int generate_current_version(sys_version_t *sys_ver)
{
	char strline[128];
	char strtmp[25];
	memset(strtmp, 0, sizeof(strtmp));
	sys_ver->va = I2C_Read(ADDR_FPGA, 0xff);
	sys_ver->app = 0;
	sys_ver->rx = rx_status[0].rx_ver;

	FILE *fp = fopen("/mnt/app/version", "r");
	if(!fp)
		goto err_open;

	while(!feof(fp))
	{
		char *p = fgets(strline, sizeof(strline), fp);
		if(!p)
			goto err_fget;

		if(strncmp(strline, "major", 5) == 0)
		{
			strcat(strtmp, &strline[7]);
			sys_ver->app = atoi(strtmp);
			break;
		}
		Printf(">>%s\n", strline);
	}

	Printf("va:%d, rx:%d, app:%d\n", sys_ver->va,
							sys_ver->rx,
							sys_ver->app);
	fclose(fp);

	sprintf(sys_ver->current, "%d.%d.%d",
							sys_ver->app,
							sys_ver->rx,
							sys_ver->va);
	return 0;

err_fget:
	fclose(fp);
err_open:
	return -1;
}

lv_obj_t *page_version_create(lv_obj_t *parent, struct panel_arr *arr)
{
    lv_obj_t *page = lv_menu_page_create(parent, NULL);
	lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_size(page, 1053, 900);
	lv_obj_add_style(page, &style_subpage, LV_PART_MAIN);
	lv_obj_set_style_pad_top(page, 94, 0);

    lv_obj_t *section = lv_menu_section_create(page);
	lv_obj_add_style(section, &style_submenu, LV_PART_MAIN);
	lv_obj_set_size(section, 1053, 894);

    create_text(NULL,section, false, "Firmware:", LV_MENU_ITEM_BUILDER_VARIANT_2);

    lv_obj_t *cont = lv_obj_create(section);
    lv_obj_set_size(cont, 960, 600);
    lv_obj_set_pos(cont, 0, 0);
    lv_obj_set_layout(cont, LV_LAYOUT_GRID);
	lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_add_style(cont, &style_context, LV_PART_MAIN);

    lv_obj_set_style_grid_column_dsc_array(cont, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(cont, row_dsc, 0);

	create_select_item(arr, cont);
	cur_ver_label = create_label_item(cont, "Current Version:", 1, 0, 2);

	label0 = create_label_item(cont, "Update VTX", 1, 1, 2);
	label1 = create_label_item(cont, "Update Goggle", 1, 2, 2);
	create_label_item(cont, "<Back", 1, 3, 1);

	bar0 = lv_bar_create(cont);
    lv_obj_set_size(bar0, 320, 20);
	lv_obj_set_grid_cell(bar0, LV_GRID_ALIGN_CENTER, 3, 3,
						 LV_GRID_ALIGN_CENTER, 1, 1);
	lv_obj_add_flag(bar0, LV_OBJ_FLAG_HIDDEN);

	bar1 = lv_bar_create(cont);
    lv_obj_set_size(bar1, 320, 20);
	lv_obj_set_grid_cell(bar1, LV_GRID_ALIGN_CENTER, 3, 3,
						 LV_GRID_ALIGN_CENTER, 2, 1);
	lv_obj_add_flag(bar1, LV_OBJ_FLAG_HIDDEN);

	return page;
}

uint8_t command_monitor(char* cmd)
{
	FILE   *stream;
	char   buf[128];
	size_t rsize = 0;
	uint8_t ret;

	stream = popen(cmd , "r" );
	if(!stream) return 0;

	Printf("---%s---\n", cmd);
	ret = 0;
	do{
		rsize = fread( buf, sizeof(char), sizeof(buf),  stream);
		Printf("%s", buf);
		if(strstr(buf, "all done"))  {ret = 1; break;}
		else if(strstr(buf, "skip"))  {ret = 2; break;}
		else if(strstr(buf, "repeat"))  {ret = 3; break;}
	} while(rsize  == sizeof(buf));
	pclose( stream );
	Printf("\n");
	return ret;
}

void version_update(int sel)
{
	if(sel == 0) {
		FILE* fp;
		char buf[80];
		int dat[16];
		fp = fopen("/tmp/wr_reg","r");
		if(fp) {
			while(fgets(buf,80,fp)) {
				sscanf(buf,"%x %x %x",&dat[0],&dat[1],&dat[2]);
				DM5680_WriteReg(dat[0], dat[1], dat[2]);
				Printf("DM5680 REG[%02x,%02x]<-%02x\n", dat[0], dat[1], dat[2]);
				usleep(100000);
			}
			fclose(fp);
			//system("rm /tmp/wr_reg");
		}

		fp = fopen("/tmp/rd_reg","r");
		if(!fp) return;
		while(fgets(buf,80,fp)) {
			sscanf(buf,"%x %x",&dat[0],&dat[1]);
			DM5680_ReadReg(dat[0], dat[1]);
			sleep(1);
			Printf("DM5680_0 REG[%02x,%02x]-> %02x\n", dat[0], dat[1], rx_status[0].rx_regval);
			Printf("DM5680_1 REG[%02x,%02x]-> %02x\n", dat[0], dat[1], rx_status[1].rx_regval);
		}
		fclose(fp);
		//system("rm /tmp/rd_reg");
	}

	else if(sel == 1) {
		uint8_t ret;
		lv_obj_clear_flag(bar0, LV_OBJ_FLAG_HIDDEN);
		lv_label_set_text(label0, "Flashing..");
		lv_timer_handler();

		is_need_update_progress = true;
		ret = command_monitor("/mnt/app/script/update_vtx.sh");
		is_need_update_progress = false;

		if(ret == 1){
			if(file_compare("/tmp/HDZERO_TX.bin","/tmp/HDZERO_TX_RB.bin")) {
				lv_label_set_text(label0, "#000FF00 SUCCESS#");
			}
			else
				lv_label_set_text(label0, "#FF0000 Verification failed, try it again#");
		}
		else if(ret == 2) {
			lv_label_set_text(label0, "#FFFF00 No firmware found.#");
		}
		else{
			lv_label_set_text(label0, "#FF0000 Failed, check connection...#");
		}
		system("rm /tmp/HDZERO_TX.bin");
		system("rm /tmp/HDZERO_TX_RB.bin");
		lv_obj_add_flag(bar0, LV_OBJ_FLAG_HIDDEN);
	}
	else if((sel == 2) && !reboot_flag) {
		uint8_t ret = 0;
		lv_obj_clear_flag(bar1, LV_OBJ_FLAG_HIDDEN);
		lv_label_set_text(label1, "WAIT... DO NOT POWER OFF... ");
		lv_timer_handler();

		is_need_update_progress = true;
		ret = command_monitor("/mnt/app/script/update_goggle.sh");
		is_need_update_progress = false;
		lv_obj_add_flag(bar1, LV_OBJ_FLAG_HIDDEN);
		if(ret == 1)
		{
			//bool b1 = file_compare("/tmp//tmp/goggle_update/HDZERO_RX.bin","/tmp//tmp/goggle_update/HDZERO_RX_RBL.bin");
			//bool b2 = file_compare("/tmp//tmp/goggle_update/HDZERO_RX.bin","/tmp//tmp/goggle_update/HDZERO_RX_RBR.bin");
			//bool b3 = file_compare("/tmp//tmp/goggle_update/HDZERO_VA.bin","/tmp//tmp/goggle_update/HDZERO_VA_RB.bin");
			//Printf("Verify result: %d %d %d\n", b1,b2,b3);
			//if(b1 && b2 && b3) {
			if(1){
				lv_timer_handler();
				lv_label_set_text(label1, "#00FF00 Update success, repower goggle NOW!#");
				beep();usleep(1000000); beep();usleep(1000000);beep();
			}
			else
				lv_label_set_text(label1, "#FF0000 FAILED#");
			reboot_flag = true;
			lv_timer_handler();
			while(1); //dead loop
		}
		else if(ret == 2)
		{
			lv_label_set_text(label1, "#FFFF00 No firmware found.#");
		}
		else if(ret == 3)
		{
			lv_label_set_text(label1, "#FFFF00 Multiple versions been found. Keep only one.#");
		}
		else
			lv_label_set_text(label1, "#FF0000 FAILED#");
		lv_obj_add_flag(bar1, LV_OBJ_FLAG_HIDDEN);
	}
}

void process_bar_update(const int value0,
		const int value1)
{
	if(bar0 && bar1)
	{
		//Printf("v0=%d, v1=%d\n", value0, value1);
		lv_bar_set_value(bar0, value0, LV_ANIM_OFF);
		lv_bar_set_value(bar1, value1, LV_ANIM_OFF);
	}
}

void bar_update(int sel, int value)
{
	if(bar1 && sel)
		lv_bar_set_value(bar1, value, LV_ANIM_OFF);
	else if(bar0 && !sel)
		lv_bar_set_value(bar0, value, LV_ANIM_OFF);
	lv_timer_handler();
}

void update_current_version()
{
	char strtmp[128];
	static bool bInit = true;
	if(bInit) {
		sys_version_t sys_version;
		generate_current_version(&sys_version);
		memset(strtmp, 0, sizeof(strtmp));
		strcat(strtmp, "Current Version:");
		strcat(strtmp, sys_version.current);
		lv_label_set_text(cur_ver_label, strtmp);
		bInit = false;
	}
}

void version_update_title()
{
	update_current_version();
	lv_label_set_text(label0, "Update VTX");
	if(!reboot_flag)
		lv_label_set_text(label1, "Update Goggle");
}


///////////////////////////////////////////////////////////////////////////////
// for progress info
static int get_progress_info(int *v0, int *v1)
{
		FILE   *stream;
		char   buf[128];
		memset( buf, '\0', sizeof(buf) );
		stream = popen( "/mnt/app/script/get_progress_info.sh" , "r" );
		fread( buf, sizeof(char), sizeof(buf),  stream);
		pclose( stream );

		char *pos = strchr(buf, 0xa);
		char buf_v0[10];
		char buf_v1[10];
		memset( buf_v0, '\0', sizeof(buf_v0) );
		memset( buf_v1, '\0', sizeof(buf_v1) );
		memcpy(buf_v0, buf, pos - buf);
		memcpy(buf_v1, pos+1, strlen(buf) - (pos-buf-1));

		*v0 = atoi(buf_v0);
		*v1 = atoi(buf_v1);
		return 0;
}

extern pthread_mutex_t lvgl_mutex;

void *thread_version(void *ptr)
{
	int count = 0;
	int sec = 0;
	int sec_last = 0;
	int v0 = 0;
	int v1 = 0;
	bool is_step1 = false;
	bool is_step2 = false;

	int percentage = 0;
	for(;;)
	{
		if(is_need_update_progress)
		{
			//pthread_mutex_lock(&lvgl_mutex);
			get_progress_info(&v0, &v1);
			if(v1 == 0)
			{
				percentage = 0;
			}
			else if(v1 == 1)
			{
				if(is_step1 == false)
					percentage = 1;

				is_step1 = true;

				if(sec_last != sec)
					percentage++;

				if(percentage > 45)
					percentage = 45;
			}
			else if(v1 == 45)
			{
				if(is_step2 == false)
					percentage = 45;

				is_step2 = true;
				if(sec_last != sec)
					percentage++;

				if(percentage > 99)
					percentage = 99;
			}
			else if(v1 == 100)
			{
				is_step1 = false;
				is_step2 = false;

				percentage = 100;
			}


			sec_last = sec;
			process_bar_update(v0, percentage);
			lv_timer_handler();
		//	pthread_mutex_unlock(&lvgl_mutex);
		}

		if(count >= 10)
		{
			count = 0;
			sec++;
		}

		///Progess bar
		progress_bar_update();

		count++;
		usleep(100000);
	}
	return NULL;
}
