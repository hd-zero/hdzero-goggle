/********************************
  File name : fbtools.h
  */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifndef EMULATOR_BUILD
#include <linux/fb.h>
#else
// Stub definitions for emulator build
struct fb_fix_screeninfo {
    char id[16];
    unsigned long smem_start;
    uint32_t smem_len;
    uint32_t type;
    uint32_t visual;
    uint16_t xpanstep;
    uint16_t ypanstep;
    uint16_t ywrapstep;
    uint32_t line_length;
    unsigned long mmio_start;
    uint32_t mmio_len;
};

struct fb_var_screeninfo {
    uint32_t xres;
    uint32_t yres;
    uint32_t xres_virtual;
    uint32_t yres_virtual;
    uint32_t xoffset;
    uint32_t yoffset;
    uint32_t bits_per_pixel;
    // Add other fields as needed
};
#endif

#include <stdint.h>
#include <stdio.h>

// a framebuffer device structure;
typedef struct fbdev {
    int fb;
    unsigned long fb_mem_offset;
    void *fb_mem;
    struct fb_fix_screeninfo fb_fix;
    struct fb_var_screeninfo fb_var;
    char dev[20];
} FBDEV, *PFBDEV;

int fb_clean();
int fb_open(PFBDEV pFbdev);
int fb_close(PFBDEV pFbdev);
int get_display_depth(PFBDEV pFbdev);
void fb_memset(void *addr, int c, size_t len);
void fb_sync(PFBDEV pFbdev);

#ifdef __cplusplus
}
#endif
