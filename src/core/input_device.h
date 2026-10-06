#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h> // uint8_t in the prototypes below

typedef enum {
    RIGHT_CLICK = 0,
    RIGHT_LONG_PRESS = 1,
    RIGHT_DOUBLE_CLICK = 2
} right_button_t;

void input_device_init();
#ifdef EMULATOR_BUILD
void input_device_pump(void);       // pump SDL events (call from main thread on macOS)
void input_device_print_help(void); // print the keyboard<->goggle control map on launch
#endif
void tune_channel(uint8_t key);
void tune_channel_timer();
void tune_channel_confirm();
void exit_tune_channel();
void rbtn_click(right_button_t click_type);

extern void (*btn_click_callback)();
extern void (*btn_press_callback)();

extern void (*rbtn_click_callback)();
extern void (*rbtn_press_callback)();
extern void (*rbtn_double_click_callback)();


extern void (*roller_callback)(uint8_t key);

#ifdef __cplusplus
}
#endif
