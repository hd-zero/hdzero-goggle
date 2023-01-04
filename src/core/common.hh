#ifndef __COMMON_HH__
#define __COMMON_HH__

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdatomic.h>
#include "defines.h"
#include "lvgl/lvgl.h"      

#define DIAL_KEY_UP     1
#define DIAL_KEY_DOWN   2
#define DIAL_KEY_CLICK  3
#define DIAL_KEY_PRESS  4
#define RIGHT_KEY_CLICK 5
#define RIGHT_KEY_PRESS 6

extern atomic_int g_key;
extern atomic_int g_init_done;
extern pthread_mutex_t lvgl_mutex;

uint8_t slow_key(left_dial_t key,uint8_t* state,uint8_t* cnt);

bool file_compare(char* f1,char* f2);
bool file_exists(const char* filename);

void    gpio_init();
void    open_gpio(int port_num);
void    set_gpio(int port_num, int isHigh);

void    beep_n(int dur_us);
#define beep()            beep_n(500)

#endif //__COMMON_HH__
  