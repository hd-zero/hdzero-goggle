/*******************************************************************************
 * Size: 16 px
 * Bpp: 1
 * Opts: 
 ******************************************************************************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#ifndef ROBOTOMONO_16
#define ROBOTOMONO_16 1
#endif

#if ROBOTOMONO_16

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */
    0x0,

    /* U+0021 "!" */
    0xff, 0x30,

    /* U+0022 "\"" */
    0x99, 0x99,

    /* U+0023 "#" */
    0x9, 0x9, 0x4, 0x8f, 0xf1, 0x20, 0x90, 0x48,
    0x48, 0xff, 0x12, 0x9, 0x4, 0x80,

    /* U+0024 "$" */
    0x10, 0x20, 0xf3, 0x34, 0x28, 0x18, 0xc, 0x4,
    0x6, 0xe, 0x37, 0xc2, 0x4, 0x0,

    /* U+0025 "%" */
    0x60, 0x48, 0x24, 0x92, 0xc6, 0x40, 0x40, 0x20,
    0x26, 0x14, 0x92, 0x41, 0x20, 0x60,

    /* U+0026 "&" */
    0x38, 0x4c, 0x44, 0x4c, 0x7c, 0x30, 0x70, 0xd9,
    0x8d, 0x87, 0xc6, 0x7f,

    /* U+0027 "'" */
    0xf0,

    /* U+0028 "(" */
    0x32, 0x44, 0x88, 0x88, 0x88, 0x88, 0x44, 0x62,
    0x0,

    /* U+0029 ")" */
    0xc4, 0x22, 0x11, 0x11, 0x11, 0x11, 0x22, 0x24,
    0x0,

    /* U+002A "*" */
    0x10, 0x22, 0x4f, 0xf1, 0x5, 0x1b, 0x22,

    /* U+002B "+" */
    0x10, 0x10, 0x10, 0xff, 0x10, 0x10, 0x10, 0x10,

    /* U+002C "," */
    0x55, 0x0,

    /* U+002D "-" */
    0xfc,

    /* U+002E "." */
    0xf0,

    /* U+002F "/" */
    0x4, 0x20, 0x86, 0x10, 0x43, 0x8, 0x21, 0x84,
    0x10, 0x80,

    /* U+0030 "0" */
    0x38, 0x8a, 0xc, 0x18, 0x73, 0x6c, 0xf1, 0x83,
    0x5, 0x11, 0xc0,

    /* U+0031 "1" */
    0x17, 0xd1, 0x11, 0x11, 0x11, 0x11,

    /* U+0032 "2" */
    0x3c, 0x46, 0x82, 0x82, 0x2, 0x4, 0xc, 0x18,
    0x30, 0x60, 0x40, 0xff,

    /* U+0033 "3" */
    0x7d, 0x8f, 0x8, 0x10, 0x67, 0x81, 0x81, 0x3,
    0x85, 0x11, 0xc0,

    /* U+0034 "4" */
    0x4, 0xc, 0x1c, 0x14, 0x34, 0x24, 0x44, 0xc4,
    0xff, 0x4, 0x4, 0x4,

    /* U+0035 "5" */
    0x7e, 0x81, 0x2, 0x7, 0xc8, 0xc0, 0x81, 0x3,
    0x85, 0x99, 0xe0,

    /* U+0036 "6" */
    0x1c, 0x41, 0x4, 0xb, 0x98, 0xa0, 0xc1, 0x83,
    0x5, 0x11, 0xc0,

    /* U+0037 "7" */
    0xff, 0x3, 0x2, 0x6, 0x4, 0xc, 0x8, 0x8,
    0x18, 0x10, 0x30, 0x20,

    /* U+0038 "8" */
    0x7d, 0x8e, 0xc, 0x1c, 0x6f, 0xb1, 0xc1, 0x83,
    0x5, 0x11, 0xc0,

    /* U+0039 "9" */
    0x38, 0x8a, 0xc, 0x18, 0x30, 0x51, 0x9d, 0x2,
    0x8, 0x23, 0x80,

    /* U+003A ":" */
    0xf0, 0x3, 0xc0,

    /* U+003B ";" */
    0xf0, 0x1, 0x5c,

    /* U+003C "<" */
    0x6, 0x3d, 0xc6, 0xe, 0x7, 0x81, 0x80,

    /* U+003D "=" */
    0xfe, 0x0, 0x0, 0xf, 0xe0,

    /* U+003E ">" */
    0x81, 0xc0, 0xf0, 0x31, 0xee, 0x30, 0x0,

    /* U+003F "?" */
    0x3c, 0xc5, 0x8, 0x10, 0x61, 0x86, 0xc, 0x0,
    0x0, 0x60, 0xc0,

    /* U+0040 "@" */
    0x1e, 0x11, 0x93, 0x7a, 0x99, 0x4d, 0x26, 0x93,
    0x4b, 0x9b, 0x20, 0x18, 0x7, 0x80,

    /* U+0041 "A" */
    0x18, 0x18, 0x18, 0x3c, 0x2c, 0x24, 0x64, 0x66,
    0x7e, 0xc2, 0xc3, 0x81,

    /* U+0042 "B" */
    0xf9, 0xe, 0xc, 0x18, 0x7f, 0xa1, 0xc1, 0x83,
    0x6, 0x17, 0xc0,

    /* U+0043 "C" */
    0x3c, 0x8e, 0xc, 0x18, 0x10, 0x20, 0x40, 0x83,
    0x5, 0x19, 0xe0,

    /* U+0044 "D" */
    0xf9, 0x1a, 0x14, 0x18, 0x30, 0x60, 0xc1, 0x83,
    0xa, 0x37, 0xc0,

    /* U+0045 "E" */
    0xfd, 0x2, 0x4, 0x8, 0x1f, 0xa0, 0x40, 0x81,
    0x2, 0x7, 0xf0,

    /* U+0046 "F" */
    0xff, 0x2, 0x4, 0x8, 0x10, 0x3f, 0x40, 0x81,
    0x2, 0x4, 0x0,

    /* U+0047 "G" */
    0x3c, 0x62, 0x41, 0x80, 0x80, 0x80, 0x8f, 0x81,
    0x81, 0x41, 0x63, 0x3e,

    /* U+0048 "H" */
    0x83, 0x6, 0xc, 0x18, 0x3f, 0xe0, 0xc1, 0x83,
    0x6, 0xc, 0x10,

    /* U+0049 "I" */
    0xfe, 0x20, 0x40, 0x81, 0x2, 0x4, 0x8, 0x10,
    0x20, 0x47, 0xf0,

    /* U+004A "J" */
    0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x81, 0x3,
    0x85, 0x11, 0xc0,

    /* U+004B "K" */
    0x87, 0x1a, 0x24, 0xcb, 0x1c, 0x3c, 0x4c, 0x89,
    0x1a, 0x1c, 0x30,

    /* U+004C "L" */
    0x81, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x81,
    0x2, 0x7, 0xf0,

    /* U+004D "M" */
    0xc7, 0x8f, 0x1f, 0x7a, 0xb5, 0x6e, 0xc9, 0x93,
    0x6, 0xc, 0x10,

    /* U+004E "N" */
    0x83, 0x87, 0xf, 0x1b, 0x36, 0x66, 0xcd, 0x8f,
    0xe, 0x1c, 0x10,

    /* U+004F "O" */
    0x3c, 0x42, 0x42, 0x81, 0x81, 0x81, 0x81, 0x81,
    0x81, 0x42, 0x42, 0x3c,

    /* U+0050 "P" */
    0xf9, 0xa, 0xc, 0x18, 0x30, 0xbe, 0x40, 0x81,
    0x2, 0x4, 0x0,

    /* U+0051 "Q" */
    0x3c, 0x42, 0x42, 0x81, 0x81, 0x81, 0x81, 0x81,
    0x81, 0x43, 0x42, 0x3e, 0x3, 0x1,

    /* U+0052 "R" */
    0xf9, 0xa, 0xc, 0x18, 0x30, 0xff, 0x46, 0x8d,
    0xa, 0x1c, 0x10,

    /* U+0053 "S" */
    0x38, 0x8a, 0xc, 0xc, 0xe, 0x7, 0x3, 0x3,
    0x7, 0x1b, 0xc0,

    /* U+0054 "T" */
    0xff, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8,
    0x8, 0x8, 0x8, 0x8,

    /* U+0055 "U" */
    0x83, 0x6, 0xc, 0x18, 0x30, 0x60, 0xc1, 0x83,
    0x5, 0x11, 0xc0,

    /* U+0056 "V" */
    0x83, 0xc3, 0xc2, 0x46, 0x46, 0x64, 0x24, 0x2c,
    0x28, 0x38, 0x18, 0x18,

    /* U+0057 "W" */
    0x99, 0x99, 0x99, 0x99, 0x9b, 0xeb, 0xee, 0xe6,
    0x66, 0x66, 0x66, 0x66,

    /* U+0058 "X" */
    0xc3, 0x42, 0x66, 0x2c, 0x3c, 0x18, 0x18, 0x3c,
    0x24, 0x66, 0x42, 0xc3,

    /* U+0059 "Y" */
    0x41, 0x31, 0x88, 0x86, 0x41, 0x40, 0xa0, 0x60,
    0x10, 0x8, 0x4, 0x2, 0x1, 0x0,

    /* U+005A "Z" */
    0xfe, 0xc, 0x10, 0x60, 0x83, 0xc, 0x10, 0x60,
    0x83, 0x7, 0xf0,

    /* U+005B "[" */
    0xf2, 0x49, 0x24, 0x92, 0x49, 0x27,

    /* U+005C "\\" */
    0x83, 0x4, 0x10, 0x60, 0x82, 0x4, 0x10, 0x60,
    0x82, 0xc,

    /* U+005D "]" */
    0xe4, 0x92, 0x49, 0x24, 0x92, 0x4f,

    /* U+005E "^" */
    0x20, 0xc3, 0x14, 0x5b, 0x28, 0xc0,

    /* U+005F "_" */
    0xfe,

    /* U+0060 "`" */
    0x44,

    /* U+0061 "a" */
    0x3c, 0x8e, 0x8, 0x17, 0xf8, 0x60, 0xc3, 0x7a,

    /* U+0062 "b" */
    0x81, 0x2, 0x7, 0xcc, 0x50, 0x60, 0xc1, 0x83,
    0x7, 0x17, 0xc0,

    /* U+0063 "c" */
    0x3c, 0x46, 0x82, 0x80, 0x80, 0x80, 0x82, 0x46,
    0x3c,

    /* U+0064 "d" */
    0x2, 0x4, 0x9, 0xf4, 0x70, 0x60, 0xc1, 0x83,
    0x5, 0x19, 0xf0,

    /* U+0065 "e" */
    0x3c, 0x66, 0xc2, 0xc2, 0xff, 0xc0, 0xc0, 0x62,
    0x3c,

    /* U+0066 "f" */
    0xf, 0x18, 0x10, 0x10, 0xfe, 0x10, 0x10, 0x10,
    0x10, 0x10, 0x10, 0x10, 0x10,

    /* U+0067 "g" */
    0x3e, 0x8e, 0xc, 0x18, 0x30, 0x60, 0xa3, 0x3e,
    0x7, 0x13, 0xc0,

    /* U+0068 "h" */
    0x81, 0x2, 0x5, 0xec, 0x70, 0x60, 0xc1, 0x83,
    0x6, 0xc, 0x10,

    /* U+0069 "i" */
    0x30, 0x60, 0x7, 0x81, 0x2, 0x4, 0x8, 0x10,
    0x20, 0x47, 0xf0,

    /* U+006A "j" */
    0x8, 0x40, 0xf0, 0x84, 0x21, 0x8, 0x42, 0x10,
    0x8d, 0xc0,

    /* U+006B "k" */
    0x81, 0x2, 0x4, 0x69, 0x96, 0x38, 0x78, 0x91,
    0x32, 0x34, 0x30,

    /* U+006C "l" */
    0xf0, 0x20, 0x40, 0x81, 0x2, 0x4, 0x8, 0x10,
    0x20, 0x47, 0xf0,

    /* U+006D "m" */
    0xff, 0x44, 0x62, 0x31, 0x18, 0x8c, 0x46, 0x23,
    0x11, 0x88, 0x80,

    /* U+006E "n" */
    0xbd, 0x8e, 0xc, 0x18, 0x30, 0x60, 0xc1, 0x82,

    /* U+006F "o" */
    0x38, 0x8a, 0xc, 0x18, 0x30, 0x60, 0xa2, 0x38,

    /* U+0070 "p" */
    0xf9, 0x8a, 0xc, 0x18, 0x30, 0x60, 0xe2, 0xf9,
    0x2, 0x4, 0x0,

    /* U+0071 "q" */
    0x3e, 0x8e, 0xc, 0x18, 0x30, 0x60, 0xa3, 0x3e,
    0x4, 0x8, 0x10,

    /* U+0072 "r" */
    0xbe, 0x21, 0x8, 0x42, 0x10, 0x80,

    /* U+0073 "s" */
    0x3c, 0x8f, 0xb, 0x3, 0xc0, 0xf0, 0xe3, 0x7c,

    /* U+0074 "t" */
    0x20, 0x43, 0xf9, 0x2, 0x4, 0x8, 0x10, 0x20,
    0x40, 0x78,

    /* U+0075 "u" */
    0x83, 0x6, 0xc, 0x18, 0x30, 0x60, 0xe3, 0x7a,

    /* U+0076 "v" */
    0x83, 0xc2, 0x46, 0x64, 0x64, 0x2c, 0x38, 0x18,
    0x18,

    /* U+0077 "w" */
    0xc8, 0xa6, 0x53, 0x29, 0xb5, 0x53, 0x98, 0xcc,
    0x66, 0x23, 0x0,

    /* U+0078 "x" */
    0xc2, 0x66, 0x2c, 0x38, 0x18, 0x38, 0x24, 0x66,
    0xc3,

    /* U+0079 "y" */
    0x83, 0xc2, 0x46, 0x66, 0x64, 0x2c, 0x38, 0x18,
    0x10, 0x10, 0x20, 0xe0,

    /* U+007A "z" */
    0xfe, 0xc, 0x30, 0xc1, 0x6, 0x18, 0x60, 0xfe,

    /* U+007B "{" */
    0x19, 0x88, 0x42, 0x10, 0x98, 0x61, 0x8, 0x42,
    0x10, 0x43,

    /* U+007C "|" */
    0xff, 0xfe,

    /* U+007D "}" */
    0xc1, 0x82, 0x8, 0x20, 0x83, 0x7, 0x30, 0x82,
    0x8, 0x20, 0x84, 0x30,

    /* U+007E "~" */
    0x70, 0xc4, 0x61, 0xc0
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 154, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 154, .box_w = 1, .box_h = 12, .ofs_x = 4, .ofs_y = 0},
    {.bitmap_index = 3, .adv_w = 154, .box_w = 4, .box_h = 4, .ofs_x = 3, .ofs_y = 8},
    {.bitmap_index = 5, .adv_w = 154, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 19, .adv_w = 154, .box_w = 7, .box_h = 15, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 33, .adv_w = 154, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 47, .adv_w = 154, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 59, .adv_w = 154, .box_w = 1, .box_h = 4, .ofs_x = 4, .ofs_y = 8},
    {.bitmap_index = 60, .adv_w = 154, .box_w = 4, .box_h = 17, .ofs_x = 3, .ofs_y = -4},
    {.bitmap_index = 69, .adv_w = 154, .box_w = 4, .box_h = 17, .ofs_x = 2, .ofs_y = -4},
    {.bitmap_index = 78, .adv_w = 154, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 85, .adv_w = 154, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 93, .adv_w = 154, .box_w = 2, .box_h = 5, .ofs_x = 2, .ofs_y = -3},
    {.bitmap_index = 95, .adv_w = 154, .box_w = 6, .box_h = 1, .ofs_x = 2, .ofs_y = 5},
    {.bitmap_index = 96, .adv_w = 154, .box_w = 2, .box_h = 2, .ofs_x = 4, .ofs_y = 0},
    {.bitmap_index = 97, .adv_w = 154, .box_w = 6, .box_h = 13, .ofs_x = 2, .ofs_y = -1},
    {.bitmap_index = 107, .adv_w = 154, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 118, .adv_w = 154, .box_w = 4, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 124, .adv_w = 154, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 136, .adv_w = 154, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 147, .adv_w = 154, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 159, .adv_w = 154, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 170, .adv_w = 154, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 181, .adv_w = 154, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 193, .adv_w = 154, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 204, .adv_w = 154, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 215, .adv_w = 154, .box_w = 2, .box_h = 9, .ofs_x = 4, .ofs_y = 0},
    {.bitmap_index = 218, .adv_w = 154, .box_w = 2, .box_h = 12, .ofs_x = 4, .ofs_y = -3},
    {.bitmap_index = 221, .adv_w = 154, .box_w = 7, .box_h = 7, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 228, .adv_w = 154, .box_w = 7, .box_h = 5, .ofs_x = 1, .ofs_y = 3},
    {.bitmap_index = 233, .adv_w = 154, .box_w = 7, .box_h = 7, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 240, .adv_w = 154, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 251, .adv_w = 154, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 265, .adv_w = 154, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 277, .adv_w = 154, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 288, .adv_w = 154, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 299, .adv_w = 154, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 310, .adv_w = 154, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 321, .adv_w = 154, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 332, .adv_w = 154, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 344, .adv_w = 154, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 355, .adv_w = 154, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 366, .adv_w = 154, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 377, .adv_w = 154, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 388, .adv_w = 154, .box_w = 7, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 399, .adv_w = 154, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 410, .adv_w = 154, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 421, .adv_w = 154, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 433, .adv_w = 154, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 444, .adv_w = 154, .box_w = 8, .box_h = 14, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 458, .adv_w = 154, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 469, .adv_w = 154, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 480, .adv_w = 154, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 492, .adv_w = 154, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 503, .adv_w = 154, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 515, .adv_w = 154, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 527, .adv_w = 154, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 539, .adv_w = 154, .box_w = 9, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 553, .adv_w = 154, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 564, .adv_w = 154, .box_w = 3, .box_h = 16, .ofs_x = 3, .ofs_y = -2},
    {.bitmap_index = 570, .adv_w = 154, .box_w = 6, .box_h = 13, .ofs_x = 2, .ofs_y = -1},
    {.bitmap_index = 580, .adv_w = 154, .box_w = 3, .box_h = 16, .ofs_x = 3, .ofs_y = -2},
    {.bitmap_index = 586, .adv_w = 154, .box_w = 6, .box_h = 7, .ofs_x = 2, .ofs_y = 6},
    {.bitmap_index = 592, .adv_w = 154, .box_w = 7, .box_h = 1, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 593, .adv_w = 154, .box_w = 3, .box_h = 2, .ofs_x = 3, .ofs_y = 10},
    {.bitmap_index = 594, .adv_w = 154, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 602, .adv_w = 154, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 613, .adv_w = 154, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 622, .adv_w = 154, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 633, .adv_w = 154, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 642, .adv_w = 154, .box_w = 8, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 655, .adv_w = 154, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 666, .adv_w = 154, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 677, .adv_w = 154, .box_w = 7, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 688, .adv_w = 154, .box_w = 5, .box_h = 15, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 698, .adv_w = 154, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 709, .adv_w = 154, .box_w = 7, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 720, .adv_w = 154, .box_w = 9, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 731, .adv_w = 154, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 739, .adv_w = 154, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 747, .adv_w = 154, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 758, .adv_w = 154, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 769, .adv_w = 154, .box_w = 5, .box_h = 9, .ofs_x = 3, .ofs_y = 0},
    {.bitmap_index = 775, .adv_w = 154, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 783, .adv_w = 154, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 793, .adv_w = 154, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 801, .adv_w = 154, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 810, .adv_w = 154, .box_w = 9, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 821, .adv_w = 154, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 830, .adv_w = 154, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 842, .adv_w = 154, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 850, .adv_w = 154, .box_w = 5, .box_h = 16, .ofs_x = 3, .ofs_y = -3},
    {.bitmap_index = 860, .adv_w = 154, .box_w = 1, .box_h = 15, .ofs_x = 4, .ofs_y = -3},
    {.bitmap_index = 862, .adv_w = 154, .box_w = 6, .box_h = 16, .ofs_x = 2, .ofs_y = -3},
    {.bitmap_index = 874, .adv_w = 154, .box_w = 9, .box_h = 3, .ofs_x = 1, .ofs_y = 3}
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
    .kern_dsc = NULL,
    .kern_scale = 0,
    .cmap_num = 1,
    .bpp = 1,
    .kern_classes = 0,
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
const lv_font_t robotomono_16 = {
#else
lv_font_t robotomono_16 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 18,          /*The maximum line height required by the font*/
    .base_line = 4,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -1,
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc           /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
};



#endif /*#if ROBOTOMONO_16*/

