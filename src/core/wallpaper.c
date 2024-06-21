
#include <log/log.h>
#include <lvgl/lvgl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "core/common.hh"
#include "osd.h"
#include "wallpaper.h"

static uint32_t img_menu_bg_data[1920 * 1080]; // 0x00BBGGRR
lv_img_dsc_t img_menu_bg;
static uint8_t wallpaper_state = 0;

int load_bmp_image(const char *file, uint32_t *obuf, int width, int height) {
    char *buf;
    struct stat stFile;
    FILE *fd;
    int size, rd;
    int line_size;

    fd = fopen(file, "rb");
    if (fd == NULL) {
        LOGE("%s cannot open", file);
        return 0;
    }

    // get file size
    fseek(fd, 0, SEEK_END);
    size = ftell(fd);
    fseek(fd, 0, SEEK_SET);

    buf = (unsigned char *)malloc(size);
    if (buf == NULL) {
        LOGE("%s error 1", file);
        return 0;
    }

    rd = fread(buf, 1, size, fd);
    if (rd != size) {
        LOGE("%s error 2", file);
        free(buf);
        return 0;
    }

    fclose(fd);

    // check image size
    bmpFileHead *bmp = (bmpFileHead *)buf;
    char *pb = buf + sizeof(bmpFileHead) + bmp->info.biClrUsed;
    if (bmp->info.biWidth != width || bmp->info.biHeight != height) {
        LOGE("%s error 3", file);
        free(buf);
        return 0;
    }

    line_size = (width * 3 + 3) & 0xFFFC; // 4bytes align

    int x, y;
    uint32_t addr;

    for (y = 0; y < height; y++) {
        addr = y * line_size;
        for (x = 0; x < width; x++)
            obuf[(height - y - 1) * width + x] = (0xff << 24) + ((pb[addr + x * 3] & 0xff)) + ((pb[addr + x * 3 + 1] & 0xff) << 8) + ((pb[addr + x * 3 + 2] & 0xff) << 16);
    }

    free(buf);
    return 1;
}

void load_wallpaper(char *file_path) {
    if (load_bmp_image(file_path, img_menu_bg_data, 1920, 1080)) {
        img_menu_bg.header.cf = LV_IMG_CF_TRUE_COLOR;
        img_menu_bg.header.always_zero = 0;
        img_menu_bg.header.reserved = 0;
        img_menu_bg.header.w = 1920;
        img_menu_bg.header.h = 1080;
        img_menu_bg.data_size = 1920 * 1080 * LV_COLOR_SIZE / 8;
        img_menu_bg.data = (uint8_t *)img_menu_bg_data;
        wallpaper_state = 1;
    }
}

uint8_t wallpaper_is_used() {
    return wallpaper_state;
}