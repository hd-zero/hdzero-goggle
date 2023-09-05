/*******************************************************************************
 * Size: 10 px
 * Bpp: 1
 * Opts: 
 ******************************************************************************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#ifndef CONTHRAX_10
#define CONTHRAX_10 1
#endif

#if CONTHRAX_10

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */
    0x0,

    /* U+0021 "!" */
    0xf9,

    /* U+0022 "\"" */
    0xb6, 0x80,

    /* U+0023 "#" */
    0x24, 0x4b, 0xf9, 0x22, 0x5f, 0xc9, 0x12,

    /* U+0024 "$" */
    0x11, 0xfe, 0x44, 0x8f, 0xe2, 0x44, 0xff, 0x10,

    /* U+0025 "%" */
    0xf1, 0x24, 0x89, 0x22, 0x50, 0xfb, 0xc4, 0x91,
    0x24, 0x8f,

    /* U+0026 "&" */
    0x78, 0x48, 0x58, 0x32, 0xd6, 0x8c, 0x8c, 0xf6,

    /* U+0027 "'" */
    0xe0,

    /* U+0028 "(" */
    0x7a, 0x49, 0x24, 0x93, 0x30,

    /* U+0029 ")" */
    0xcc, 0x92, 0x49, 0x25, 0xe0,

    /* U+002A "*" */
    0x21, 0x3e, 0x45, 0x0,

    /* U+002B "+" */
    0x21, 0x9, 0xf2, 0x10,

    /* U+002C "," */
    0xe0,

    /* U+002D "-" */
    0xe0,

    /* U+002E "." */
    0x80,

    /* U+002F "/" */
    0x11, 0x12, 0x22, 0x44, 0x48, 0x80,

    /* U+0030 "0" */
    0x7e, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x7e,

    /* U+0031 "1" */
    0xd5, 0x55,

    /* U+0032 "2" */
    0xfe, 0x4, 0x8, 0x1f, 0xf0, 0x20, 0x7f,

    /* U+0033 "3" */
    0xfe, 0x4, 0x8, 0x1f, 0xe0, 0x40, 0xff,

    /* U+0034 "4" */
    0xe, 0x1a, 0x32, 0x62, 0xc2, 0xff, 0x2, 0x2,

    /* U+0035 "5" */
    0xff, 0x2, 0x4, 0xf, 0xe0, 0x40, 0xff,

    /* U+0036 "6" */
    0x7f, 0x2, 0x4, 0xf, 0xf0, 0x60, 0xbf,

    /* U+0037 "7" */
    0xff, 0x3, 0x7, 0x6, 0xc, 0x18, 0x30, 0x30,

    /* U+0038 "8" */
    0xff, 0x81, 0x81, 0x81, 0xff, 0x81, 0x81, 0xff,

    /* U+0039 "9" */
    0xfd, 0x6, 0xc, 0x1f, 0xe0, 0x40, 0xfe,

    /* U+003A ":" */
    0x88,

    /* U+003B ";" */
    0x8e,

    /* U+003C "<" */
    0x0, 0xee, 0x30, 0x70, 0x30,

    /* U+003D "=" */
    0xf8, 0x1, 0xf0,

    /* U+003E ">" */
    0x83, 0x81, 0x83, 0x72, 0x0,

    /* U+003F "?" */
    0xf8, 0x42, 0x66, 0x20, 0x8,

    /* U+0040 "@" */
    0xfe, 0x81, 0x9f, 0x91, 0x91, 0x9f, 0x80, 0x80,
    0x7e,

    /* U+0041 "A" */
    0xc, 0x7, 0x81, 0x20, 0xcc, 0x23, 0x1f, 0xc4,
    0x1b, 0x2,

    /* U+0042 "B" */
    0xff, 0x81, 0x81, 0x81, 0xff, 0x81, 0x81, 0xff,

    /* U+0043 "C" */
    0x7f, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x7f,

    /* U+0044 "D" */
    0xfe, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0xfe,

    /* U+0045 "E" */
    0x7f, 0xc0, 0xc0, 0xc0, 0xfe, 0xc0, 0xc0, 0x7f,

    /* U+0046 "F" */
    0x7f, 0x80, 0x80, 0x80, 0xfe, 0x80, 0x80, 0x80,

    /* U+0047 "G" */
    0x7f, 0x80, 0x80, 0x80, 0x8f, 0x81, 0x81, 0x7f,

    /* U+0048 "H" */
    0x83, 0x6, 0xc, 0x1f, 0xf0, 0x60, 0xc1,

    /* U+0049 "I" */
    0xff,

    /* U+004A "J" */
    0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0xfe,

    /* U+004B "K" */
    0x87, 0xa, 0x34, 0xcf, 0x11, 0x23, 0x43,

    /* U+004C "L" */
    0x81, 0x2, 0x4, 0x8, 0x10, 0x20, 0x3f,

    /* U+004D "M" */
    0xe1, 0xf8, 0x7a, 0x16, 0xcd, 0x92, 0x64, 0x99,
    0xe6, 0x31,

    /* U+004E "N" */
    0xe1, 0xa1, 0xb1, 0x99, 0x99, 0x8d, 0x85, 0x87,

    /* U+004F "O" */
    0x7e, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x7e,

    /* U+0050 "P" */
    0xfe, 0x81, 0x81, 0x81, 0xfe, 0x80, 0x80, 0x80,

    /* U+0051 "Q" */
    0x7e, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x7e,
    0x6, 0x3,

    /* U+0052 "R" */
    0xfe, 0x40, 0xa0, 0x50, 0x2f, 0xf4, 0x32, 0xd,
    0x3,

    /* U+0053 "S" */
    0xff, 0x2, 0x4, 0xf, 0xe0, 0x40, 0xff,

    /* U+0054 "T" */
    0xff, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8,

    /* U+0055 "U" */
    0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x7e,

    /* U+0056 "V" */
    0xc0, 0x90, 0x66, 0x18, 0x8c, 0x33, 0x4, 0x81,
    0xe0, 0x30,

    /* U+0057 "W" */
    0xc7, 0x1a, 0x28, 0x91, 0x44, 0xda, 0x66, 0xcb,
    0x14, 0x50, 0xa2, 0x87, 0x1c,

    /* U+0058 "X" */
    0x61, 0x11, 0x8d, 0x83, 0x81, 0xc1, 0xb1, 0x8d,
    0x83,

    /* U+0059 "Y" */
    0x41, 0x31, 0x88, 0x82, 0x81, 0xc0, 0x40, 0x20,
    0x10,

    /* U+005A "Z" */
    0xfe, 0x7, 0xf, 0x3e, 0x78, 0xe0, 0xc0, 0xff,

    /* U+005B "[" */
    0xf2, 0x49, 0x24, 0x92, 0x70,

    /* U+005C "\\" */
    0x88, 0x44, 0x44, 0x22, 0x21, 0x10,

    /* U+005D "]" */
    0xe4, 0x92, 0x49, 0x24, 0xf0,

    /* U+005E "^" */
    0x30, 0xc4, 0x92, 0x48, 0x0,

    /* U+005F "_" */
    0xfc,

    /* U+0060 "`" */
    0xb4,

    /* U+0061 "a" */
    0xfc, 0x7, 0xfc, 0x18, 0x3f, 0xc0,

    /* U+0062 "b" */
    0x81, 0x3, 0xf4, 0x18, 0x30, 0x60, 0xff,

    /* U+0063 "c" */
    0x7f, 0x2, 0x4, 0x8, 0x1f, 0xc0,

    /* U+0064 "d" */
    0x2, 0x5, 0xfc, 0x18, 0x30, 0x60, 0xff,

    /* U+0065 "e" */
    0x7f, 0x7, 0xfc, 0x8, 0xf, 0xc0,

    /* U+0066 "f" */
    0x7a, 0x3e, 0x84, 0x21, 0x8,

    /* U+0067 "g" */
    0xff, 0x6, 0xc, 0x1f, 0xe0, 0x40, 0xff,

    /* U+0068 "h" */
    0x81, 0x3, 0xf4, 0x18, 0x30, 0x60, 0xc1,

    /* U+0069 "i" */
    0xbf,

    /* U+006A "j" */
    0x20, 0x92, 0x49, 0x3c,

    /* U+006B "k" */
    0x81, 0x2, 0x14, 0x4f, 0x91, 0x23, 0x43,

    /* U+006C "l" */
    0xff,

    /* U+006D "m" */
    0xff, 0xc4, 0x62, 0x31, 0x18, 0x8c, 0x44,

    /* U+006E "n" */
    0xff, 0x6, 0xc, 0x18, 0x30, 0x40,

    /* U+006F "o" */
    0xff, 0x6, 0xc, 0x18, 0x3f, 0xc0,

    /* U+0070 "p" */
    0xff, 0x6, 0xc, 0x18, 0x3f, 0xe0, 0x40,

    /* U+0071 "q" */
    0xff, 0x6, 0xc, 0x18, 0x3f, 0xc0, 0x81,

    /* U+0072 "r" */
    0x7e, 0x8, 0x20, 0x82, 0x0,

    /* U+0073 "s" */
    0x7f, 0x81, 0xf8, 0x10, 0x2f, 0xc0,

    /* U+0074 "t" */
    0x44, 0xf4, 0x44, 0x47,

    /* U+0075 "u" */
    0x83, 0x6, 0xc, 0x18, 0x3f, 0xc0,

    /* U+0076 "v" */
    0xc3, 0x43, 0x66, 0x24, 0x34, 0x18,

    /* U+0077 "w" */
    0xc6, 0x24, 0xe2, 0x4b, 0x66, 0x96, 0x79, 0x43,
    0x1c,

    /* U+0078 "x" */
    0x42, 0x24, 0x18, 0x38, 0x64, 0xc2,

    /* U+0079 "y" */
    0x41, 0x63, 0x22, 0x36, 0x14, 0x18, 0x18, 0x30,

    /* U+007A "z" */
    0xfc, 0xc, 0x73, 0x8c, 0x1f, 0x80,

    /* U+007B "{" */
    0x74, 0x44, 0x4c, 0x44, 0x44, 0x47,

    /* U+007C "|" */
    0xff, 0xc0,

    /* U+007D "}" */
    0xe2, 0x22, 0x23, 0x22, 0x22, 0x2e,

    /* U+007E "~" */
    0xe4, 0xc0
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 54, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 43, .box_w = 1, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 2, .adv_w = 79, .box_w = 3, .box_h = 3, .ofs_x = 1, .ofs_y = 5},
    {.bitmap_index = 4, .adv_w = 136, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 11, .adv_w = 131, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 19, .adv_w = 183, .box_w = 10, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 29, .adv_w = 140, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 37, .adv_w = 43, .box_w = 1, .box_h = 3, .ofs_x = 1, .ofs_y = 5},
    {.bitmap_index = 38, .adv_w = 72, .box_w = 3, .box_h = 12, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 43, .adv_w = 72, .box_w = 3, .box_h = 12, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 48, .adv_w = 93, .box_w = 5, .box_h = 6, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 52, .adv_w = 98, .box_w = 5, .box_h = 6, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 56, .adv_w = 43, .box_w = 1, .box_h = 3, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 57, .adv_w = 61, .box_w = 3, .box_h = 1, .ofs_x = 1, .ofs_y = 3},
    {.bitmap_index = 58, .adv_w = 43, .box_w = 1, .box_h = 1, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 59, .adv_w = 66, .box_w = 4, .box_h = 11, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 65, .adv_w = 145, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 73, .adv_w = 54, .box_w = 2, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 75, .adv_w = 138, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 82, .adv_w = 135, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 89, .adv_w = 145, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 97, .adv_w = 138, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 104, .adv_w = 139, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 111, .adv_w = 133, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 119, .adv_w = 145, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 127, .adv_w = 139, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 134, .adv_w = 43, .box_w = 1, .box_h = 5, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 135, .adv_w = 43, .box_w = 1, .box_h = 7, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 136, .adv_w = 98, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 141, .adv_w = 98, .box_w = 5, .box_h = 4, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 144, .adv_w = 98, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 149, .adv_w = 101, .box_w = 5, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 154, .adv_w = 140, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 163, .adv_w = 156, .box_w = 10, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 173, .adv_w = 147, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 181, .adv_w = 136, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 189, .adv_w = 149, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 197, .adv_w = 136, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 205, .adv_w = 133, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 213, .adv_w = 148, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 221, .adv_w = 145, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 228, .adv_w = 46, .box_w = 1, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 229, .adv_w = 125, .box_w = 7, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 236, .adv_w = 137, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 243, .adv_w = 128, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 250, .adv_w = 191, .box_w = 10, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 260, .adv_w = 156, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 268, .adv_w = 158, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 276, .adv_w = 141, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 284, .adv_w = 158, .box_w = 8, .box_h = 10, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 294, .adv_w = 145, .box_w = 9, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 303, .adv_w = 137, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 310, .adv_w = 140, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 318, .adv_w = 156, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 326, .adv_w = 156, .box_w = 10, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 336, .adv_w = 208, .box_w = 13, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 349, .adv_w = 143, .box_w = 9, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 358, .adv_w = 143, .box_w = 9, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 367, .adv_w = 137, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 375, .adv_w = 72, .box_w = 3, .box_h = 12, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 380, .adv_w = 66, .box_w = 4, .box_h = 11, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 386, .adv_w = 72, .box_w = 3, .box_h = 12, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 391, .adv_w = 98, .box_w = 6, .box_h = 6, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 396, .adv_w = 103, .box_w = 6, .box_h = 1, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 397, .adv_w = 63, .box_w = 2, .box_h = 3, .ofs_x = 1, .ofs_y = 6},
    {.bitmap_index = 398, .adv_w = 126, .box_w = 7, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 404, .adv_w = 139, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 411, .adv_w = 117, .box_w = 7, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 417, .adv_w = 139, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 424, .adv_w = 126, .box_w = 7, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 430, .adv_w = 78, .box_w = 5, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 435, .adv_w = 139, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 442, .adv_w = 141, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 449, .adv_w = 43, .box_w = 1, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 450, .adv_w = 45, .box_w = 3, .box_h = 10, .ofs_x = -1, .ofs_y = -2},
    {.bitmap_index = 454, .adv_w = 127, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 461, .adv_w = 43, .box_w = 1, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 462, .adv_w = 177, .box_w = 9, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 469, .adv_w = 141, .box_w = 7, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 475, .adv_w = 139, .box_w = 7, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 481, .adv_w = 139, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 488, .adv_w = 139, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 495, .adv_w = 98, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 500, .adv_w = 125, .box_w = 7, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 506, .adv_w = 78, .box_w = 4, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 510, .adv_w = 140, .box_w = 7, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 516, .adv_w = 133, .box_w = 8, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 522, .adv_w = 186, .box_w = 12, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 531, .adv_w = 127, .box_w = 8, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 537, .adv_w = 129, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 545, .adv_w = 117, .box_w = 7, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 551, .adv_w = 72, .box_w = 4, .box_h = 12, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 557, .adv_w = 68, .box_w = 1, .box_h = 10, .ofs_x = 2, .ofs_y = -2},
    {.bitmap_index = 559, .adv_w = 72, .box_w = 4, .box_h = 12, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 565, .adv_w = 98, .box_w = 5, .box_h = 2, .ofs_x = 1, .ofs_y = 3}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/



/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 32, .range_length = 95, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    }
};

/*-----------------
 *    KERNING
 *----------------*/


/*Map glyph_ids to kern left classes*/
static const uint8_t kern_left_class_mapping[] =
{
    0, 0, 0, 1, 0, 0, 0, 2,
    1, 3, 0, 0, 0, 4, 5, 4,
    6, 7, 8, 0, 9, 10, 11, 12,
    13, 14, 15, 0, 0, 0, 0, 0,
    0, 3, 16, 9, 17, 18, 0, 19,
    20, 0, 0, 21, 22, 23, 24, 0,
    18, 25, 18, 26, 27, 28, 21, 29,
    30, 22, 31, 32, 3, 0, 0, 0,
    0, 0, 33, 34, 0, 0, 35, 36,
    37, 37, 0, 38, 39, 0, 37, 37,
    34, 34, 0, 40, 41, 42, 43, 44,
    45, 39, 44, 46, 3, 0, 0, 0
};

/*Map glyph_ids to kern right classes*/
static const uint8_t kern_right_class_mapping[] =
{
    0, 0, 0, 1, 0, 0, 0, 2,
    1, 0, 0, 0, 0, 3, 4, 3,
    5, 6, 7, 0, 0, 8, 0, 9,
    10, 11, 12, 0, 0, 0, 0, 0,
    13, 0, 14, 0, 15, 0, 15, 16,
    15, 0, 0, 17, 0, 18, 19, 0,
    15, 0, 15, 0, 20, 21, 18, 22,
    23, 24, 25, 26, 0, 0, 0, 0,
    0, 0, 27, 0, 28, 28, 28, 29,
    28, 0, 0, 30, 0, 0, 31, 31,
    28, 31, 28, 31, 32, 33, 34, 35,
    36, 37, 38, 39, 0, 0, 0, 0
};

/*Kern values between classes*/
static const int8_t kern_class_values[] =
{
    0, 0, -10, 0, 0, 0, 0, -6,
    -3, 0, -3, 0, 0, -19, -3, 0,
    -16, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -5, 0, 0, -5, -6,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -5, -13, -8, 0, -18,
    0, 0, -3, 0, 0, 0, 0, 0,
    -6, -5, 0, 0, -5, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 18, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -10, 0, 0,
    0, 0, -3, -5, -3, 0, -10, -3,
    0, 0, 0, -3, 0, 0, -3, 0,
    0, -16, -16, -16, 0, -19, 0, 0,
    0, -11, 8, 0, 0, -18, -3, -19,
    0, 0, -19, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -13, 0, 0,
    0, -13, 0, 0, -6, 0, 0, 0,
    -16, -13, -6, -16, -16, 0, 0, 0,
    2, 0, 0, 0, 0, 0, -5, 0,
    -8, -5, 0, 0, -11, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -24, -6, -6, -19, 0, -3, -6, 0,
    0, 0, 0, 0, 0, -6, -10, 0,
    0, -10, -10, 0, -6, 0, 0, 0,
    0, -6, 0, 0, -3, 0, 0, 0,
    -2, 0, 0, -2, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -3, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -5, 0, 0,
    0, 0, 0, 0, 0, -6, -5, -4,
    -5, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -2, 0, 0, -12,
    0, 0, 0, 0, 0, -4, 0, 0,
    -6, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -4, 0, 0, 0, -3, 0, 0, -3,
    0, -5, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -5, 0, -5,
    0, 0, 0, -3, 0, 0, -3, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -18, 0,
    0, -2, 0, -8, -3, 0, -2, -1,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -3, 0, -3, 0, 0,
    0, 0, 0, 0, -2, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -3, 0, -5, 0, 0, 0,
    0, 0, 0, -3, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -19, -2, 0, -13, 0, 0, 0,
    0, 0, 0, 0, 0, -16, 0, -6,
    0, 0, -6, 0, 0, -21, -30, -18,
    0, -27, 0, -3, -5, -8, 6, 0,
    0, -15, -5, -19, 0, 0, -16, 0,
    0, 0, 0, -6, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -3,
    0, -3, 0, -6, 0, 0, 0, 0,
    0, 0, 0, 0, -6, 0, 0, 0,
    0, 0, 0, -3, -6, -3, -5, -8,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -3, 0, 0, 0, -3,
    0, 3, -11, 0, 0, 0, 0, 0,
    0, 0, 0, -11, 0, 0, -13, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -5, 0, 0, 0, 0, 0, 0,
    0, 0, -3, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -3, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -3, 0,
    -6, 0, 0, 0, 0, 0, 0, 0,
    0, -6, 0, 0, -2, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -3, 0, 0, 0, -3, 0, -16, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -5, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -3, -6, -6,
    0, 0, 0, -8, -6, -10, 0, 0,
    -10, -1, 0, 0, 0, -10, 0, 0,
    0, 0, 0, 0, 0, 0, -13, 0,
    0, 0, 0, -2, 0, 0, -30, -29,
    -13, 0, -34, 0, 0, -2, 0, 0,
    0, 0, -11, -2, -18, -7, 0, -18,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -2, 0, 0,
    0, 0, 0, 0, 0, 0, -2, -4,
    0, -5, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -3, -11, 2, -14, 0, 0, 0,
    0, 0, 0, 0, -2, -16, 0, 0,
    -10, 0, 0, 0, 0, 0, -3, -8,
    -4, 0, 0, 0, 0, 0, -2, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -3, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -2, 0, -3, 0, 0,
    -1, 0, -2, -2, -8, -4, 0, -9,
    -3, -2, -5, 0, 0, 0, 0, 0,
    -4, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -6, 0, 0, 0, 0, 0,
    0, 0, 0, -3, 0, 0, 0, 0,
    0, 0, 0, -1, -2, -4, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -2, 0, -3, -3, 0, 0, -10, -16,
    -16, -22, 0, 0, 0, 0, 0, 0,
    0, 0, -21, -3, -3, -29, 2, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -3, 0, 0, -3, -3, 0, 0, 0,
    0, 0, 0, 0, 0, -13, -16, -13,
    -21, 0, 0, 0, 0, 0, 0, 0,
    0, -30, -6, -8, -24, 0, -2, -5,
    0, 0, 0, 0, 0, 0, -5, -15,
    0, 0, -15, -12, 0, -8, 0, 0,
    -5, 0, 0, 0, -11, -16, -6, -14,
    0, 0, 0, 0, 0, 0, 0, 0,
    -18, -3, -8, -16, 0, -5, -5, 0,
    0, 0, 0, 0, 0, -5, -10, 0,
    0, -10, -9, 0, -6, 0, 0, -3,
    0, 0, 0, -10, -19, -16, -22, 0,
    0, 0, 0, 0, 0, 0, 0, -27,
    -8, -11, -33, 0, -4, -5, 0, 0,
    0, 0, 0, 0, -11, -18, 0, 0,
    -18, -16, -5, -10, -3, 0, -5, -3,
    -5, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -3,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -3, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -6, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -5, -3, 0, -4, 0, -5,
    0, 0, 0, -6, 0, 0, 0, 0,
    0, 0, 0, -10, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -2,
    0, -4, -3, -3, -4, 0, -3, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -6, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -2, 0, 0, -2, 0, 5, -5, -19,
    3, -6, 0, 0, 0, 0, 0, 0,
    0, 5, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 3, 0, 0, 0, 3, 0, 0,
    0, 0, 0, 0, -5, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -10, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 12, 0, 0, 0, 0, -4, 0,
    0, -4, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    11, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -2, 0, -8, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -3, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -6, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 3, 0, 0,
    0, 3, 0, 0, 0, 0, 0, 0,
    -4, 0, -3, -3, -5, 0, 0, 0,
    0, 0, 0, 0, -3, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -2, 0, 0, -2, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 3, 0, 0, 0, 3,
    0, 0, 0, 0, 0, 0, 0, 0,
    -3, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -11, -19,
    -5, -14, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -9, 0, 0, -7, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -8, 0, 0,
    -8, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -2, -3,
    0, 0, -3, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -3,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0
};


/*Collect the kern class' data in one place*/
static const lv_font_fmt_txt_kern_classes_t kern_classes =
{
    .class_pair_values   = kern_class_values,
    .left_class_mapping  = kern_left_class_mapping,
    .right_class_mapping = kern_right_class_mapping,
    .left_class_cnt      = 46,
    .right_class_cnt     = 39,
};

/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LV_VERSION_CHECK(8, 0, 0)
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = &kern_classes,
    .kern_scale = 16,
    .cmap_num = 1,
    .bpp = 1,
    .kern_classes = 1,
    .bitmap_format = 0,
#if LV_VERSION_CHECK(8, 0, 0)
    .cache = &cache
#endif
};


/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LV_VERSION_CHECK(8, 0, 0)
const lv_font_t conthrax_10 = {
#else
lv_font_t conthrax_10 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 12,          /*The maximum line height required by the font*/
    .base_line = 2,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -1,
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc           /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
};



#endif /*#if CONTHRAX_10*/

