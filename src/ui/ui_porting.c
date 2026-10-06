#include "ui/ui_porting.h"

#include <stdio.h>

#include <log/log.h>

#include "../core/common.hh"
#include "../core/settings.h"
#ifndef EMULATOR_BUILD
#include "fbtools.h" // Linux framebuffer — goggle display path only (emulator uses SDL)
#endif
#include "lvgl/lvgl.h"

#ifdef EMULATOR_BUILD
#include <stdlib.h> // getenv, malloc, free

#include "SDLaccess.h"
#include "core/app_state.h" // g_app_state — video only composites in the video view
#include "emulator/video_emu.h"
static void *fb1, *fb2;
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *texture = NULL;
#ifndef WINDOW_NAME
#define WINDOW_NAME "HDZero"
#endif

// ---------------------------------------------------------------------------
// Fake video-feed compositing (EMULATOR only, opt-in via the HDZ_VIDEO env var)
// ---------------------------------------------------------------------------
// When HDZ_VIDEO points at a video file, a decoded video is played as the FPV
// BACKGROUND, composited UNDERNEATH the LVGL OSD so the FPV viewer is
// representative for UI design. The OSD paints its video region solid gray
// 0x7f7f7f; we chroma-key that gray to transparent so the video shows through.
//
// This is entirely inert unless `video_active` is true (which only happens when
// HDZ_VIDEO is set AND the decoder starts OK). When inactive, the flush path is
// byte-for-byte identical to the original opaque LVGL copy: no video texture, no
// per-pixel conversion, no extra allocations.
#define VIDEO_CHROMA_KEY 127 // gray 0x7f7f7f channel value of the OSD video region
#define VIDEO_CHROMA_TOL 4   // per-channel tolerance (covers e.g. RGB565 quantisation of the key)

static SDL_Texture *video_texture = NULL;     // RGB24 streaming texture: latest decoded video frame (background)
static SDL_Texture *lvgl_argb_texture = NULL; // ARGB8888 streaming texture: keyed LVGL frame drawn on top
static uint8_t *video_rgb_buf = NULL;         // scratch RGB24 decoder frame  (W*H*3)
static uint8_t *lvgl_argb_buf = NULL;         // scratch ARGB8888 keyed frame (W*H*4)
static bool video_active = false;             // true only when the fake video feed is running

// True when (r,g,b) is within tolerance of the gray chroma key (127,127,127).
static inline bool video_is_chroma_key(uint8_t r, uint8_t g, uint8_t b) {
    return (r >= VIDEO_CHROMA_KEY - VIDEO_CHROMA_TOL) && (r <= VIDEO_CHROMA_KEY + VIDEO_CHROMA_TOL) &&
           (g >= VIDEO_CHROMA_KEY - VIDEO_CHROMA_TOL) && (g <= VIDEO_CHROMA_KEY + VIDEO_CHROMA_TOL) &&
           (b >= VIDEO_CHROMA_KEY - VIDEO_CHROMA_TOL) && (b <= VIDEO_CHROMA_KEY + VIDEO_CHROMA_TOL);
}
#endif

typedef enum {
    ORBIT_NONE = 0x00,
    ORBIT_D = 0x01,
    ORBIT_R = 0x02,
    ORBIT_U = 0x04,
    ORBIT_L = 0x08,
    ORBIT_FLUSH = 0x10,
} orbit_state_t;

#ifndef EMULATOR_BUILD
FBDEV fbdev; // goggle framebuffer handle; the emulator flushes via SDL instead
#endif
static lv_disp_draw_buf_t draw_buf;
#ifdef EMULATOR_BUILD
// The emulator malloc's the draw buffer at init (below) instead of a large static:
// mingw stores a zero-init static in the PE .data section (~33MB in the .exe),
// whereas ELF keeps it in .bss (unstored). Same buffer, allocated on the heap.
static lv_color_t *disp_buf;
#else
static lv_color_t disp_buf[DRAW_HOR_RES_FHD * DRAW_VER_RES_FHD * 4];
#endif
static lv_disp_drv_t disp_drv;
static orbit_state_t disp_orbit_state = ORBIT_NONE;
static int disp_orbit_x, disp_orbit_y;
static lv_disp_t *disp;

static void hdz_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    static int orbit_flush = 0;

    // Manipulates the orbiting speed
    if (++orbit_flush % 500 == 0) {
        disp_orbit_state |= ORBIT_FLUSH;
    }

#ifndef EMULATOR_BUILD
    for (lv_coord_t y = area->y1 + disp_orbit_y; y <= area->y2; y++) {
        fbdev.fb_mem_offset = (y * (disp_drv.hor_res - DISP_OVERSCAN) + disp_orbit_x) * 4;
        fbdev.fb_fix.smem_len = 4 * (area->x2 - area->x1 + 1 - DISP_OVERSCAN);
        memcpy(fbdev.fb_mem + fbdev.fb_mem_offset, ((char *)color_p), fbdev.fb_fix.smem_len);
        color_p += (area->x2 - area->x1 + 1);
    }

    fb_sync(&fbdev);
#else
    SDL_LockMutex(global_sdl_mutex);
    SDL_Rect
        src = {
            .x = 0,
            .y = 0,
            .w = disp_drv.hor_res,
            .h = disp_drv.ver_res,
        },
        dst = {
            .x = 0,
            .y = 0,
            .w = disp_drv.hor_res - DISP_OVERSCAN,
            .h = disp_drv.ver_res - DISP_OVERSCAN,
        };

    // The mock video plane only exists in the video view, exactly like the real
    // DM5680 overlay (the app paints the 0x7f7f7f "video hole" only there, and
    // calls HDZero_Close() on the way to the menu). Outside APP_STATE_VIDEO we
    // must NOT chroma-key -- otherwise menu pixels near gray get keyed and show
    // video -- and we pause the decoder so the menu costs no video CPU.
    bool show_video = video_active && (g_app_state == APP_STATE_VIDEO);
    if (video_active)
        video_emu_set_paused(!show_video);

    if (show_video) {
        // ---- Fake video-feed compositing path (only when HDZ_VIDEO is set) ----
        // 1) Refresh the background video texture with the latest decoded frame.
        //    video_emu_get may return false (no frame yet); we then keep the
        //    previously uploaded frame, so the background never flickers.
        if (video_emu_get(video_rgb_buf, DRAW_HOR_RES_FHD, DRAW_VER_RES_FHD)) {
            SDL_UpdateTexture(video_texture, NULL, video_rgb_buf, DRAW_HOR_RES_FHD * 3);
        }

        // 2) Convert the LVGL framebuffer to ARGB8888, chroma-keying the gray
        //    video region (RGB 127,127,127 == 0x7f7f7f) to alpha 0x00 so the
        //    video shows through; every other pixel is fully opaque (0xFF).
        //
        //    Pixel conversion: lv_color_t is LV_COLOR_DEPTH bits. lv_color_to32()
        //    normalises ANY depth to 0xAARRGGBB (for LV_COLOR_DEPTH==16 it expands
        //    the packed RGB565 5/6/5 channels to 8-bit each; for ==32 it is the
        //    value as-is). We then repack as ARGB8888 with our computed alpha.
        //    On little-endian hosts the uint32 0xAARRGGBB is stored as bytes
        //    B,G,R,A -- exactly what SDL_PIXELFORMAT_ARGB8888 expects.
        {
            lv_color_t *cp = color_p;
            uint32_t *out = (uint32_t *)lvgl_argb_buf;
            int count = disp_drv.hor_res * disp_drv.ver_res;
            for (int i = 0; i < count; i++) {
                uint32_t px = lv_color_to32(cp[i]); // 0xAARRGGBB, alpha bits ignored below
                uint8_t r = (uint8_t)((px >> 16) & 0xFF);
                uint8_t g = (uint8_t)((px >> 8) & 0xFF);
                uint8_t b = (uint8_t)(px & 0xFF);
                uint8_t a = video_is_chroma_key(r, g, b) ? 0x00 : 0xFF;
                out[i] = ((uint32_t)a << 24) | ((uint32_t)r << 16) |
                         ((uint32_t)g << 8) | (uint32_t)b;
            }
        }
        // Upload the keyed frame (same sub-rect + pitch convention as the
        // original opaque copy, but ARGB8888 is fixed 4 bytes/pixel).
        SDL_UpdateTexture(lvgl_argb_texture, &src, lvgl_argb_buf, disp_drv.hor_res * 4);
        src.x = disp_orbit_x;
        src.w = dst.w;
        src.y = disp_orbit_y;
        src.h = dst.h;

        // 3) Composite: clear, video underneath (full window), keyed OSD on top.
        //    lvgl_argb_texture has SDL_BLENDMODE_BLEND so alpha==0 pixels reveal
        //    the video background.
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, video_texture, NULL, NULL);
        SDL_RenderCopy(renderer, lvgl_argb_texture, &src, &dst);
        SDL_RenderPresent(renderer);
    } else {
        // ---- Default path: unchanged opaque LVGL copy via the original texture ----
        SDL_UpdateTexture(texture, &src, color_p, disp_drv.hor_res * ((LV_COLOR_DEPTH + 7) / 8));
        src.x = disp_orbit_x;
        src.w = dst.w;
        src.y = disp_orbit_y;
        src.h = dst.h;
        SDL_RenderCopy(renderer, texture, &src, &dst);
        SDL_RenderPresent(renderer);
    }
    SDL_UnlockMutex(global_sdl_mutex);
#endif

    if (disp_orbit_state & ORBIT_FLUSH) {
        disp_orbit_state &= ~ORBIT_FLUSH;

        int pixels = 1 << g_setting.osd.orbit;

        if (disp_orbit_state & ORBIT_D && disp_orbit_y < pixels) {
            disp_orbit_y += 1;
            if (disp_orbit_y == pixels) {
                disp_orbit_state &= ~ORBIT_D;
                disp_orbit_state |= ORBIT_R;
            }
        } else if (disp_orbit_state & ORBIT_R && disp_orbit_x < pixels) {
            disp_orbit_x += 1;
            if (disp_orbit_x == pixels) {
                disp_orbit_state &= ~ORBIT_R;
                disp_orbit_state |= ORBIT_U;
            }
        } else if (disp_orbit_state & ORBIT_U && disp_orbit_y > 0) {
            disp_orbit_y -= 1;
            if (disp_orbit_y == 0) {
                disp_orbit_state &= !ORBIT_U;
                disp_orbit_state |= ORBIT_L;
            }
        } else if (disp_orbit_state & ORBIT_L && disp_orbit_x > 0) {
            disp_orbit_x -= 1;
            if (disp_orbit_x == 0) {
                disp_orbit_state &= ~ORBIT_L;
                disp_orbit_state |= ORBIT_D;
            }
        }
    }

    lv_disp_flush_ready(disp);
}

int lvgl_init_porting() {
#ifndef EMULATOR_BUILD
    memset(&fbdev, 0x0, sizeof(FBDEV));
    strncpy(fbdev.dev, "/dev/fb0", sizeof(fbdev.dev));
    if (fb_open(&fbdev) == -1) {
        LOGE("open frame buffer error");
        return -1;
    }
    LOGI("register disp drv");

    lv_disp_draw_buf_init(&draw_buf, disp_buf, NULL, DRAW_HOR_RES_FHD * DRAW_VER_RES_FHD * 4);

    lv_disp_drv_init(&disp_drv);
#else
    if (SDL_WasInit(SDL_INIT_VIDEO) == 0) {
        SDL_InitSubSystem(SDL_INIT_VIDEO);
    } else {
        LOGI("SDL already initialised.");
    }

    SDL_LockMutex(global_sdl_mutex);
    window = SDL_CreateWindow(WINDOW_NAME,
                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              DISP_HOR_RES_FHD, DISP_VER_RES_FHD, 0);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);

    texture = SDL_CreateTexture(renderer,
                                (LV_COLOR_DEPTH == 32) ? (SDL_PIXELFORMAT_ARGB8888) : (SDL_PIXELFORMAT_RGB565),
                                SDL_TEXTUREACCESS_STREAMING,
                                DRAW_HOR_RES_FHD,
                                DRAW_VER_RES_FHD);

    // Fake video feed: opt-in via HDZ_VIDEO. Left completely inert (video_active
    // stays false) otherwise, so the default emulator is unchanged.
    const char *vpath = getenv("HDZ_VIDEO");
    if (vpath && *vpath) {
        // Background texture the decoded RGB24 video frames are uploaded into.
        video_texture = SDL_CreateTexture(renderer,
                                          SDL_PIXELFORMAT_RGB24,
                                          SDL_TEXTUREACCESS_STREAMING,
                                          DRAW_HOR_RES_FHD,
                                          DRAW_VER_RES_FHD);
        // Dedicated ARGB8888 texture for the LVGL frame drawn ON TOP of the video.
        // BLEND mode makes chroma-keyed (alpha==0) pixels transparent so the
        // video shows through. Kept separate from `texture` so the default path
        // (which never sets a blend mode) is untouched.
        lvgl_argb_texture = SDL_CreateTexture(renderer,
                                              SDL_PIXELFORMAT_ARGB8888,
                                              SDL_TEXTUREACCESS_STREAMING,
                                              DRAW_HOR_RES_FHD,
                                              DRAW_VER_RES_FHD);
        if (lvgl_argb_texture) {
            SDL_SetTextureBlendMode(lvgl_argb_texture, SDL_BLENDMODE_BLEND);
        }
        video_rgb_buf = (uint8_t *)malloc((size_t)DRAW_HOR_RES_FHD * DRAW_VER_RES_FHD * 3);
        lvgl_argb_buf = (uint8_t *)malloc((size_t)DRAW_HOR_RES_FHD * DRAW_VER_RES_FHD * 4);
        if (video_texture && lvgl_argb_texture && video_rgb_buf && lvgl_argb_buf &&
            video_emu_start(vpath, DRAW_HOR_RES_FHD, DRAW_VER_RES_FHD) == 0) {
            video_active = true;
            LOGI("HDZ_VIDEO: fake video feed active (%s) at %dx%d", vpath, DRAW_HOR_RES_FHD, DRAW_VER_RES_FHD);
        } else {
            // On any failure, roll back so the flush path stays on the original
            // opaque code path (video_active remains false).
            LOGE("HDZ_VIDEO: failed to start fake video feed (%s); running without it", vpath);
            if (video_texture) {
                SDL_DestroyTexture(video_texture);
                video_texture = NULL;
            }
            if (lvgl_argb_texture) {
                SDL_DestroyTexture(lvgl_argb_texture);
                lvgl_argb_texture = NULL;
            }
            free(video_rgb_buf);
            video_rgb_buf = NULL;
            free(lvgl_argb_buf);
            lvgl_argb_buf = NULL;
            video_active = false;
        }
    }
    SDL_UnlockMutex(global_sdl_mutex);

    fb1 = malloc(DRAW_HOR_RES_FHD * DRAW_VER_RES_FHD * ((LV_COLOR_DEPTH + 7) / 8));
    fb2 = malloc(DRAW_HOR_RES_FHD * DRAW_VER_RES_FHD * ((LV_COLOR_DEPTH + 7) / 8));
    // LVGL draw buffer (heap, not a large static — keeps it out of the mingw .data).
    disp_buf = malloc((size_t)DRAW_HOR_RES_FHD * DRAW_VER_RES_FHD * 4 * sizeof(lv_color_t));

    lv_disp_draw_buf_init(&draw_buf, fb1, fb2, DRAW_HOR_RES_FHD * DRAW_VER_RES_FHD);
    lv_disp_drv_init(&disp_drv);
#endif

    disp_drv.full_refresh = 1;
    disp_drv.flush_cb = hdz_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    disp_drv.hor_res = DRAW_HOR_RES_FHD;
    disp_drv.ver_res = DRAW_VER_RES_FHD;
    disp = lv_disp_drv_register(&disp_drv);

    return 0;
}

int lvgl_switch_to_720p() {
    lvgl_screen_orbit(false);
    lv_disp_draw_buf_init(&draw_buf, disp_buf, NULL, DRAW_HOR_RES_HD * DRAW_VER_RES_HD * 4);
    disp_drv.draw_buf = &draw_buf;
    disp_drv.hor_res = DRAW_HOR_RES_HD;
    disp_drv.ver_res = DRAW_VER_RES_HD;
    lv_disp_drv_update(disp, &disp_drv);
    lvgl_screen_orbit(g_setting.osd.orbit > 0);
    return 0;
}

int lvgl_switch_to_1080p() {
    lvgl_screen_orbit(false);
    lv_disp_draw_buf_init(&draw_buf, disp_buf, NULL, DRAW_HOR_RES_FHD * DRAW_VER_RES_FHD * 4);
    disp_drv.draw_buf = &draw_buf;
    disp_drv.hor_res = DRAW_HOR_RES_FHD;
    disp_drv.ver_res = DRAW_VER_RES_FHD;
    lv_disp_drv_update(disp, &disp_drv);
    lvgl_screen_orbit(g_setting.osd.orbit > 0);
    return 0;
}

void lvgl_screen_orbit(bool enable) {
    if (enable) {
        disp_orbit_state = ORBIT_D;
    } else {
        disp_orbit_state = ORBIT_NONE;
        disp_orbit_x = 0;
        disp_orbit_y = 0;
    }
}
