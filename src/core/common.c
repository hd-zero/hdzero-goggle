#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdatomic.h>
#include "defines.h"
#include "common.hh"
#include "self_test.h"
#include "ui/page_common.h"

///////////////////////////////////////////////////////////////////////////////
//globals
pthread_mutex_t lvgl_mutex;
atomic_int g_key = 0;
atomic_int g_init_done = 0;
///////////////////////////////////////////////////////////////////////////////

uint8_t slow_key(left_dial_t key,uint8_t* state,uint8_t* cnt)
{
    if((key == LEFT_DAIL_CLICK) || (key == LEFT_DAIL_LONGPRESS)) {
        *state = 0;
        *cnt = 0;
        return key;
    }

    switch(*state) {
        case 0:
            *state = key + 10;
            *cnt = 1;
            break;

        case 11:
            if(key == LEFT_DAIL_UP) {
                if(*cnt == DIAL_SENSITIVITY) {
                    *state = 0;
                    return LEFT_DAIL_UP;
                }
                else (*cnt)++;
            }
            else { // key= 2
                *state = 12;
            }
            break;

        case 12:
            if(key == LEFT_DAIL_DOWN) {
                if(*cnt == DIAL_SENSITIVITY) {
                    *state = 0;
                    return LEFT_DAIL_DOWN;
                }
                else (*cnt)++;
            }
            else { //key =1
                *state = 11;
            }
            break;
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
// file compare
bool file_compare(char* f1,char* f2)
{
    FILE* fp1;
    FILE* fp2;
    char  c1,c2;
    bool  ret;

    fp1 = fopen(f1,"r");
    if(!fp1) return false;

    fp2 = fopen(f2,"r");
    if(!fp2) {
        fclose(fp1);
        return false;
    }

    ret = true;
    while(!feof(fp1)) {
        fread(&c1,1,1,fp1);
        fread(&c2,1,1,fp2);
        if(c1 != c2) {
            ret = false;
            break;
        }
    }
    fclose(fp1);
    fclose(fp2);
    return ret;
}

bool file_exists(const char* filename) {
    return access(filename, F_OK) == 0;
}

///////////////////////////////////////////////////////////////////////////////
// GPIO
void gpio_init()
{
    open_gpio(GPIO_RXRESET);
    open_gpio(GPIO_TXRESET);
    open_gpio(GPIO_BEEP);
    open_gpio(GPIO_TP2825_RSTB);
    open_gpio(GPIO_ESP32_EN);
    open_gpio(GPIO_ESP32_BOOT0);

    set_gpio(GPIO_BEEP,0);
    set_gpio(GPIO_TXRESET,0);
    set_gpio(GPIO_RXRESET,1);
    set_gpio(GPIO_TP2825_RSTB,0);
    set_gpio(GPIO_ESP32_EN,0);
    set_gpio(GPIO_ESP32_BOOT0,0);
}


void open_gpio(int port_num)
{
	char buf[64];
	FILE* fp;

	sprintf(buf,"%s/gpio%d/direction",CLASS_PATH_GPIO,port_num);
	fp=fopen(buf,"r");
	if(!fp) {
		sprintf(buf,"echo \"%d\">%s/export",port_num,CLASS_PATH_GPIO);
		system(buf);  //LOGI("%s",buf);
		sprintf(buf,"echo \"out\">%s/gpio%d/direction",CLASS_PATH_GPIO,port_num);
		system(buf);  //LOGI("%s",buf);
	}
	else fclose(fp);
}

void set_gpio(int port_num, int isHigh)
{
	char buf[64];
	sprintf(buf,"echo \"%d\">%s/gpio%d/value",isHigh,CLASS_PATH_GPIO,port_num);
	system(buf); //LOGI("%s",buf);
}


void beep_n(int dur_us)
{
    static bool bInit = true;
    if(bInit) {
        bInit = false;
        open_gpio(GPIO_BEEP);
    }
    set_gpio(GPIO_BEEP,1);
    usleep(dur_us);
    set_gpio(GPIO_BEEP,0);
}
