// Desktop emulator input backend: maps SDL keyboard events onto the goggle's jog
// wheel + centre button + right function button, and feeds them to the shared
// handlers in core/input_device.c. Compiled only for EMULATOR_BUILD -- CMake swaps
// this in for the goggle's driver/input_evdev.c, so neither the shared logic nor
// the hardware path carries any emulator #ifdef.

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "SDLaccess.h"

#include "common.hh"

#include "core/input_device.h"
#include "core/input_device_internal.h"

// Right-button double-click window (ms). Matches the dm5680.c short-click timeout
// on the real goggle, so a single tap is deferred this long to disambiguate it
// from a double-tap.
#define RBTN_DOUBLE_CLICK_MS 250

// Drain pending SDL events (keyboard -> goggle buttons/wheel). macOS Cocoa
// requires this to run on the main thread, so main.c calls it from the main
// loop; other platforms call it from the background input thread below.
void input_device_pump(void) {
    static uint32_t btn_wheel_start = 0;
    static uint32_t btn_fn_start = 0;
    static uint32_t btn_fn_click_ts = 0; // timestamp of a deferred (pending) right click
    static bool btn_fn_pending = false;  // a right tap is waiting to see if it doubles

    SDL_Event event;
    SDL_LockMutex(global_sdl_mutex);
    while (SDL_PollEvent(&event)) {
        SDL_UnlockMutex(global_sdl_mutex);
        switch (event.type) {
        case SDL_QUIT:
            exit(0);

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_d: // D = wheel press (centre button: enter/select)
                if (!btn_wheel_start) {
                    btn_wheel_start = event.key.timestamp;
                }
                break;

            case SDLK_l: // L = function button (right side of the goggle)
                if (!btn_fn_start) {
                    btn_fn_start = event.key.timestamp;
                }
                break;
            }
            break;

        case SDL_KEYUP:
            switch (event.key.keysym.sym) {
            case SDLK_s: // S = jog wheel down (highlight moves down)
                roller_up();
                g_key = DIAL_KEY_UP;
                break;

            case SDLK_w: // W = jog wheel up (highlight moves up)
                roller_down();
                g_key = DIAL_KEY_DOWN;
                break;

            case SDLK_d: // D = wheel press (centre button: enter/select)
                if (event.key.timestamp - btn_wheel_start > 500) {
                    btn_press();
                    g_key = DIAL_KEY_PRESS;
                } else {
                    btn_click();
                    g_key = DIAL_KEY_CLICK;
                }
                btn_wheel_start = 0;
                break;

            case SDLK_l: // L = function button (right side of the goggle)
                if (event.key.timestamp - btn_fn_start > 500) {
                    btn_fn_pending = false; // long press supersedes any pending single
                    rbtn_click(RIGHT_LONG_PRESS);
                    g_key = RIGHT_KEY_PRESS;
                } else if (btn_fn_pending &&
                           event.key.timestamp - btn_fn_click_ts <= RBTN_DOUBLE_CLICK_MS) {
                    btn_fn_pending = false; // second tap inside the window -> double click
                    rbtn_click(RIGHT_DOUBLE_CLICK);
                    g_key = RIGHT_KEY_CLICK;
                } else {
                    // first tap: defer the single click until the double-click window
                    // lapses (fired from the timeout check below), matching dm5680.c
                    btn_fn_pending = true;
                    btn_fn_click_ts = event.key.timestamp;
                }
                btn_fn_start = 0;
                break;
            }
            break;
        }
        SDL_LockMutex(global_sdl_mutex);
    }
    SDL_UnlockMutex(global_sdl_mutex);

    // Fire a deferred single right-click once the double-click window lapses with
    // no second tap (mirrors the dm5680.c short-click timeout on the real goggle).
    if (btn_fn_pending && SDL_GetTicks() - btn_fn_click_ts > RBTN_DOUBLE_CLICK_MS) {
        btn_fn_pending = false;
        rbtn_click(RIGHT_CLICK);
        g_key = RIGHT_KEY_CLICK;
    }
}

// Print the keyboard<->goggle control map to the terminal on launch. The real
// goggle is driven by a jog wheel (rotate + press) plus a right-side function
// button, each distinguishing tap vs long-press; here those are keyboard keys,
// which isn't obvious without being told. Emulator-only -- the firmware has no
// console and no keyboard. Keep this in sync with input_device_pump() above.
void input_device_print_help(void) {
    printf(
        "\n"
        "======================== HDZero Goggle emulator ========================\n"
        "  keyboard  ->  goggle control\n"
        "  ----------------------------------------------------------------------\n"
        "  W ................. jog wheel up        move highlight up\n"
        "  S ................. jog wheel down      move highlight down\n"
        "  D ................. wheel press         tap = select / confirm\n"
        "                      (centre button)     hold >= 0.5s = long press\n"
        "  L ................. function button     tap = click,  hold >= 0.5s = long press\n"
        "                      (right of goggle)   double-tap = recentre head tracker (video)\n"
        "  close window ...... quit\n"
        "  ----------------------------------------------------------------------\n"
        "  in a menu:  wheel navigates, tap selects, long-press wheel = back/exit\n"
        "  in video :  tap = OSD on/off, long-press = open menu, Fn = DVR toggle\n"
        "========================================================================\n"
        "\n");
    fflush(stdout);
}

void input_backend_init(void) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Error initializing SDL: %s\n", SDL_GetError());
    }
}

void *input_backend_thread(void *ptr) {
    (void)ptr;
    // macOS Cocoa and Windows require SDL events to be pumped on the main (window)
    // thread, so there main.c drives input_device_pump() from its loop and this
    // thread idles. Elsewhere (Linux/X11) the background thread pumps.
#if !defined(__APPLE__) && !defined(_WIN32)
    while (true) {
        input_device_pump();
        usleep(50000);
    }
#endif
    return NULL;
}
