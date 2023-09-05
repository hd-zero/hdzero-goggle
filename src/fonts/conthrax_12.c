/*******************************************************************************
 * Size: 12 px
 * Bpp: 1
 * Opts: 
 ******************************************************************************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#ifndef CONTHRAX_12
#define CONTHRAX_12 1
#endif

#if CONTHRAX_12

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */
    0x0,

    /* U+0021 "!" */
    0xff, 0xf0, 0xf0,

    /* U+0022 "\"" */
    0xbb, 0x99,

    /* U+0023 "#" */
    0x22, 0x11, 0x3f, 0xe4, 0x42, 0x21, 0x13, 0xfe,
    0x44, 0x22, 0x0,

    /* U+0024 "$" */
    0x8, 0x7f, 0xf2, 0x19, 0xf, 0xf8, 0x4c, 0x27,
    0xff, 0x8, 0x4, 0x0,

    /* U+0025 "%" */
    0xf8, 0xc8, 0x88, 0x89, 0x8, 0xa0, 0xff, 0xf0,
    0x51, 0x9, 0x11, 0x11, 0x31, 0xf0,

    /* U+0026 "&" */
    0x7e, 0x19, 0x86, 0x61, 0xf8, 0x18, 0xde, 0x6c,
    0xf3, 0x1c, 0xc7, 0xbf, 0x30,

    /* U+0027 "'" */
    0xaa,

    /* U+0028 "(" */
    0x74, 0x88, 0x88, 0x88, 0x88, 0x88, 0x47,

    /* U+0029 ")" */
    0xcc, 0x92, 0x49, 0x24, 0x97, 0x80,

    /* U+002A "*" */
    0x10, 0x23, 0x59, 0xc2, 0x8d, 0x80, 0x0,

    /* U+002B "+" */
    0x10, 0x41, 0x3f, 0x10, 0x41, 0x0,

    /* U+002C "," */
    0xfe,

    /* U+002D "-" */
    0xf0,

    /* U+002E "." */
    0xf0,

    /* U+002F "/" */
    0x8, 0xc4, 0x21, 0x10, 0x84, 0x62, 0x11, 0x88,
    0x0,

    /* U+0030 "0" */
    0x7f, 0xb0, 0x3c, 0xf, 0x3, 0xc0, 0xf0, 0x3c,
    0xf, 0x3, 0x7f, 0x80,

    /* U+0031 "1" */
    0xed, 0xb6, 0xdb, 0x60,

    /* U+0032 "2" */
    0xff, 0x0, 0xc0, 0x60, 0x37, 0xf6, 0x3, 0x1,
    0x80, 0xff, 0x80,

    /* U+0033 "3" */
    0xff, 0x0, 0xc0, 0x60, 0x3f, 0xf0, 0xc, 0x6,
    0x3, 0xff, 0x0,

    /* U+0034 "4" */
    0x3, 0x83, 0x61, 0x98, 0xc6, 0xe1, 0xb0, 0x6f,
    0xfc, 0x6, 0x1, 0x80,

    /* U+0035 "5" */
    0xff, 0xe0, 0x30, 0x18, 0xf, 0xf0, 0xc, 0x6,
    0x3, 0xff, 0x0,

    /* U+0036 "6" */
    0x7f, 0xe0, 0x30, 0x18, 0xf, 0xfe, 0xf, 0x7,
    0x83, 0x7f, 0x0,

    /* U+0037 "7" */
    0xff, 0x80, 0x70, 0x18, 0xc, 0x7, 0x3, 0x81,
    0xc0, 0xe0, 0x30, 0x0,

    /* U+0038 "8" */
    0x7f, 0xb0, 0x3c, 0xf, 0x3, 0x7f, 0xb0, 0x3c,
    0xf, 0x3, 0x7f, 0x80,

    /* U+0039 "9" */
    0x7f, 0x60, 0xf0, 0x78, 0x37, 0xf8, 0xc, 0x6,
    0x3, 0xff, 0x0,

    /* U+003A ":" */
    0xf0, 0xf0,

    /* U+003B ";" */
    0xf0, 0xfe,

    /* U+003C "<" */
    0x2, 0x3b, 0xc4, 0xf, 0x3, 0x81, 0x80,

    /* U+003D "=" */
    0xfc, 0x0, 0x3f,

    /* U+003E ">" */
    0x81, 0xe0, 0x70, 0x31, 0xde, 0x20, 0x0,

    /* U+003F "?" */
    0xfe, 0xc, 0x18, 0x73, 0x84, 0x8, 0x0, 0x30,
    0x60,

    /* U+0040 "@" */
    0x7f, 0x30, 0x6c, 0x1b, 0x7e, 0xd9, 0xb6, 0x6d,
    0x9b, 0x7c, 0xc0, 0x30, 0x7, 0xfc,

    /* U+0041 "A" */
    0xe, 0x0, 0xf0, 0x1b, 0x1, 0x98, 0x39, 0x83,
    0xc, 0x7f, 0xc6, 0x6, 0xc0, 0x60,

    /* U+0042 "B" */
    0xff, 0xbf, 0xfc, 0xf, 0x3, 0xff, 0xb0, 0x3c,
    0xf, 0xff, 0xff, 0x80,

    /* U+0043 "C" */
    0x7f, 0xff, 0xf0, 0x18, 0xc, 0x6, 0x3, 0x1,
    0xff, 0x7f, 0x80,

    /* U+0044 "D" */
    0xff, 0xbf, 0xfc, 0xf, 0x3, 0xc0, 0xf0, 0x3c,
    0xf, 0xff, 0xff, 0x80,

    /* U+0045 "E" */
    0x7f, 0xff, 0xf0, 0x18, 0xf, 0xfe, 0x3, 0x1,
    0xff, 0x7f, 0x80,

    /* U+0046 "F" */
    0x7f, 0xff, 0xf0, 0x18, 0xf, 0xfe, 0x3, 0x1,
    0x80, 0xc0, 0x0,

    /* U+0047 "G" */
    0x7f, 0xff, 0xfc, 0x3, 0x0, 0xcf, 0xf0, 0x3c,
    0xf, 0xff, 0x7f, 0xc0,

    /* U+0048 "H" */
    0xc1, 0xe0, 0xf0, 0x78, 0x3f, 0xfe, 0xf, 0x7,
    0x83, 0xc1, 0x80,

    /* U+0049 "I" */
    0xff, 0xff, 0xc0,

    /* U+004A "J" */
    0x1, 0x80, 0xc0, 0x60, 0x30, 0x18, 0xc, 0x7,
    0xff, 0xff, 0x0,

    /* U+004B "K" */
    0xc1, 0xe1, 0xb0, 0xd8, 0xcf, 0xc6, 0x33, 0xd,
    0x87, 0xc1, 0x80,

    /* U+004C "L" */
    0xc0, 0x60, 0x30, 0x18, 0xc, 0x6, 0x3, 0x1,
    0xff, 0x7f, 0x80,

    /* U+004D "M" */
    0xe0, 0x3f, 0x83, 0xf4, 0x1f, 0xb1, 0xbd, 0x8d,
    0xe4, 0x4f, 0x36, 0x78, 0xa3, 0xc7, 0x18,

    /* U+004E "N" */
    0xe0, 0xfc, 0x3d, 0x8f, 0x63, 0xcc, 0xf1, 0xbc,
    0x6f, 0xf, 0xc1, 0xc0,

    /* U+004F "O" */
    0x7f, 0xbf, 0xfc, 0xf, 0x3, 0xc0, 0xf0, 0x3c,
    0xf, 0xff, 0x7f, 0x80,

    /* U+0050 "P" */
    0xff, 0x7f, 0xf0, 0x78, 0x3c, 0x1f, 0xfb, 0x1,
    0x80, 0xc0, 0x0,

    /* U+0051 "Q" */
    0x7f, 0xbf, 0xfc, 0xf, 0x3, 0xc0, 0xf0, 0x3c,
    0xf, 0xff, 0x7f, 0x80, 0xc0, 0x18, 0x6,

    /* U+0052 "R" */
    0xff, 0x7f, 0xf0, 0x78, 0x3c, 0x1f, 0xfb, 0xd,
    0x86, 0xc1, 0x80,

    /* U+0053 "S" */
    0x7f, 0xff, 0xf0, 0x18, 0x7, 0xf0, 0xc, 0x7,
    0xff, 0xff, 0x0,

    /* U+0054 "T" */
    0xff, 0xff, 0xf0, 0xc0, 0x30, 0xc, 0x3, 0x0,
    0xc0, 0x30, 0xc, 0x0,

    /* U+0055 "U" */
    0xc0, 0xf0, 0x3c, 0xf, 0x3, 0xc0, 0xf0, 0x3c,
    0xf, 0xff, 0x7f, 0x80,

    /* U+0056 "V" */
    0xc0, 0x66, 0x6, 0x60, 0xc3, 0xc, 0x31, 0x81,
    0x98, 0x1b, 0x0, 0xf0, 0xe, 0x0,

    /* U+0057 "W" */
    0xc3, 0x86, 0xc7, 0x8d, 0x8f, 0x1b, 0x36, 0x26,
    0x66, 0xc6, 0xcd, 0x8f, 0x1b, 0x1e, 0x1c, 0x1c,
    0x38,

    /* U+0058 "X" */
    0x60, 0xc6, 0x30, 0x6c, 0x7, 0x0, 0xe0, 0x3e,
    0xc, 0xe3, 0x8c, 0xe0, 0xc0,

    /* U+0059 "Y" */
    0xc0, 0xd8, 0x66, 0x18, 0xcc, 0x1e, 0x7, 0x80,
    0xc0, 0x30, 0xc, 0x0,

    /* U+005A "Z" */
    0xff, 0xff, 0xc0, 0x60, 0xe1, 0xc3, 0x83, 0x1,
    0xff, 0xff, 0x80,

    /* U+005B "[" */
    0xf2, 0x49, 0x24, 0x92, 0x49, 0xc0,

    /* U+005C "\\" */
    0x86, 0x10, 0x84, 0x30, 0x84, 0x30, 0x84, 0x30,
    0x80,

    /* U+005D "]" */
    0xe4, 0x92, 0x49, 0x24, 0x93, 0xc0,

    /* U+005E "^" */
    0x31, 0x45, 0x12, 0x8a, 0x38, 0x40,

    /* U+005F "_" */
    0xff,

    /* U+0060 "`" */
    0xc9, 0x80,

    /* U+0061 "a" */
    0xff, 0x0, 0xc0, 0x7f, 0xfc, 0x1e, 0xf, 0xfe,

    /* U+0062 "b" */
    0xc0, 0x60, 0x30, 0x1f, 0xec, 0x1e, 0xf, 0x7,
    0x83, 0xc1, 0xbf, 0x80,

    /* U+0063 "c" */
    0x7f, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x7f,

    /* U+0064 "d" */
    0x1, 0x80, 0xc0, 0x6f, 0xfc, 0x1e, 0xf, 0x7,
    0x83, 0xc1, 0xbf, 0x80,

    /* U+0065 "e" */
    0x7f, 0xe0, 0xf0, 0x7f, 0xfc, 0x6, 0x1, 0xfe,

    /* U+0066 "f" */
    0x3b, 0x19, 0xf6, 0x31, 0x8c, 0x63, 0x0,

    /* U+0067 "g" */
    0x7f, 0x60, 0xf0, 0x78, 0x3c, 0x1e, 0xd, 0xfe,
    0x3, 0x1, 0xff, 0x80,

    /* U+0068 "h" */
    0xc0, 0x60, 0x30, 0x1f, 0xec, 0x1e, 0xf, 0x7,
    0x83, 0xc1, 0xe0, 0xc0,

    /* U+0069 "i" */
    0xf3, 0xff, 0xf0,

    /* U+006A "j" */
    0x33, 0x3, 0x33, 0x33, 0x33, 0x33, 0xe0,

    /* U+006B "k" */
    0xc0, 0x60, 0x30, 0x18, 0x2c, 0x36, 0x33, 0xf1,
    0x8c, 0xc3, 0x60, 0xc0,

    /* U+006C "l" */
    0xff, 0xff, 0xf0,

    /* U+006D "m" */
    0x7f, 0xec, 0x63, 0xc6, 0x3c, 0x63, 0xc6, 0x3c,
    0x63, 0xc6, 0x30,

    /* U+006E "n" */
    0x7f, 0x60, 0xf0, 0x78, 0x3c, 0x1e, 0xf, 0x6,

    /* U+006F "o" */
    0x7f, 0x60, 0xf0, 0x78, 0x3c, 0x1e, 0xd, 0xfc,

    /* U+0070 "p" */
    0x7f, 0x60, 0xf0, 0x78, 0x3c, 0x1e, 0xf, 0xfd,
    0x80, 0xc0, 0x60, 0x0,

    /* U+0071 "q" */
    0x7f, 0x60, 0xf0, 0x78, 0x3c, 0x1e, 0xd, 0xfe,
    0x3, 0x1, 0x80, 0xc0,

    /* U+0072 "r" */
    0x7f, 0x83, 0x6, 0xc, 0x18, 0x30, 0x0,

    /* U+0073 "s" */
    0x7f, 0xb0, 0x18, 0xf, 0xf0, 0x18, 0xf, 0xfe,

    /* U+0074 "t" */
    0x63, 0x3e, 0xc6, 0x31, 0x8c, 0x38,

    /* U+0075 "u" */
    0xc1, 0xe0, 0xf0, 0x78, 0x3c, 0x1e, 0xd, 0xfc,

    /* U+0076 "v" */
    0xc0, 0xd8, 0x66, 0x18, 0xcc, 0x33, 0x7, 0x80,
    0xc0,

    /* U+0077 "w" */
    0xc3, 0xd, 0x1e, 0x26, 0x49, 0x99, 0x26, 0x6c,
    0xd0, 0xa3, 0xc3, 0x87, 0x0,

    /* U+0078 "x" */
    0x61, 0x99, 0x87, 0x83, 0x83, 0xe3, 0x9b, 0x6,

    /* U+0079 "y" */
    0xc0, 0x98, 0x66, 0x30, 0xcc, 0x36, 0x7, 0x81,
    0xc0, 0x30, 0x8, 0xe, 0x0,

    /* U+007A "z" */
    0xfe, 0x3, 0xe, 0x3c, 0xf0, 0xc0, 0xff,

    /* U+007B "{" */
    0x39, 0x8, 0x42, 0x13, 0x84, 0x21, 0x8, 0x42,
    0xc,

    /* U+007C "|" */
    0xff, 0xf0,

    /* U+007D "}" */
    0xe1, 0x8, 0x42, 0x10, 0xe4, 0x21, 0x8, 0x42,
    0x70,

    /* U+007E "~" */
    0xe6, 0x70
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 64, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 51, .box_w = 2, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 4, .adv_w = 94, .box_w = 4, .box_h = 4, .ofs_x = 1, .ofs_y = 6},
    {.bitmap_index = 6, .adv_w = 164, .box_w = 9, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 17, .adv_w = 157, .box_w = 9, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 29, .adv_w = 219, .box_w = 12, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 43, .adv_w = 168, .box_w = 10, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 56, .adv_w = 52, .box_w = 2, .box_h = 4, .ofs_x = 1, .ofs_y = 6},
    {.bitmap_index = 57, .adv_w = 87, .box_w = 4, .box_h = 14, .ofs_x = 2, .ofs_y = -3},
    {.bitmap_index = 64, .adv_w = 87, .box_w = 3, .box_h = 14, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 70, .adv_w = 112, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 3},
    {.bitmap_index = 77, .adv_w = 118, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 83, .adv_w = 51, .box_w = 2, .box_h = 4, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 84, .adv_w = 73, .box_w = 4, .box_h = 1, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 85, .adv_w = 51, .box_w = 2, .box_h = 2, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 86, .adv_w = 79, .box_w = 5, .box_h = 13, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 95, .adv_w = 174, .box_w = 10, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 107, .adv_w = 65, .box_w = 3, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 111, .adv_w = 166, .box_w = 9, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 122, .adv_w = 162, .box_w = 9, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 133, .adv_w = 174, .box_w = 10, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 145, .adv_w = 166, .box_w = 9, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 156, .adv_w = 167, .box_w = 9, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 167, .adv_w = 159, .box_w = 10, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 179, .adv_w = 174, .box_w = 10, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 191, .adv_w = 167, .box_w = 9, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 202, .adv_w = 51, .box_w = 2, .box_h = 6, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 204, .adv_w = 51, .box_w = 2, .box_h = 8, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 206, .adv_w = 118, .box_w = 7, .box_h = 7, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 213, .adv_w = 118, .box_w = 6, .box_h = 4, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 216, .adv_w = 118, .box_w = 7, .box_h = 7, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 223, .adv_w = 121, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 232, .adv_w = 168, .box_w = 10, .box_h = 11, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 246, .adv_w = 187, .box_w = 12, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 260, .adv_w = 176, .box_w = 10, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 272, .adv_w = 164, .box_w = 9, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 283, .adv_w = 178, .box_w = 10, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 295, .adv_w = 163, .box_w = 9, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 306, .adv_w = 160, .box_w = 9, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 317, .adv_w = 178, .box_w = 10, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 329, .adv_w = 174, .box_w = 9, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 340, .adv_w = 55, .box_w = 2, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 343, .adv_w = 150, .box_w = 9, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 354, .adv_w = 164, .box_w = 9, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 365, .adv_w = 154, .box_w = 9, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 376, .adv_w = 229, .box_w = 13, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 391, .adv_w = 188, .box_w = 10, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 403, .adv_w = 189, .box_w = 10, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 415, .adv_w = 169, .box_w = 9, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 426, .adv_w = 189, .box_w = 10, .box_h = 12, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 441, .adv_w = 174, .box_w = 9, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 452, .adv_w = 165, .box_w = 9, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 463, .adv_w = 168, .box_w = 10, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 475, .adv_w = 188, .box_w = 10, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 487, .adv_w = 187, .box_w = 12, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 501, .adv_w = 250, .box_w = 15, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 518, .adv_w = 171, .box_w = 11, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 531, .adv_w = 172, .box_w = 10, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 543, .adv_w = 164, .box_w = 9, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 554, .adv_w = 87, .box_w = 3, .box_h = 14, .ofs_x = 2, .ofs_y = -3},
    {.bitmap_index = 560, .adv_w = 79, .box_w = 5, .box_h = 13, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 569, .adv_w = 87, .box_w = 3, .box_h = 14, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 575, .adv_w = 118, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 581, .adv_w = 123, .box_w = 8, .box_h = 1, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 582, .adv_w = 75, .box_w = 3, .box_h = 3, .ofs_x = 1, .ofs_y = 7},
    {.bitmap_index = 584, .adv_w = 151, .box_w = 9, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 592, .adv_w = 167, .box_w = 9, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 604, .adv_w = 140, .box_w = 8, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 611, .adv_w = 167, .box_w = 9, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 623, .adv_w = 151, .box_w = 9, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 631, .adv_w = 94, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 638, .adv_w = 167, .box_w = 9, .box_h = 10, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 650, .adv_w = 169, .box_w = 9, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 662, .adv_w = 52, .box_w = 2, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 665, .adv_w = 54, .box_w = 4, .box_h = 13, .ofs_x = -1, .ofs_y = -3},
    {.bitmap_index = 672, .adv_w = 152, .box_w = 9, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 684, .adv_w = 52, .box_w = 2, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 687, .adv_w = 213, .box_w = 12, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 698, .adv_w = 170, .box_w = 9, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 706, .adv_w = 167, .box_w = 9, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 714, .adv_w = 167, .box_w = 9, .box_h = 10, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 726, .adv_w = 167, .box_w = 9, .box_h = 10, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 738, .adv_w = 118, .box_w = 7, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 745, .adv_w = 150, .box_w = 9, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 753, .adv_w = 94, .box_w = 5, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 759, .adv_w = 168, .box_w = 9, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 767, .adv_w = 159, .box_w = 10, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 776, .adv_w = 224, .box_w = 14, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 789, .adv_w = 152, .box_w = 9, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 797, .adv_w = 155, .box_w = 10, .box_h = 10, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 810, .adv_w = 141, .box_w = 8, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 817, .adv_w = 87, .box_w = 5, .box_h = 14, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 826, .adv_w = 81, .box_w = 1, .box_h = 12, .ofs_x = 2, .ofs_y = -2},
    {.bitmap_index = 828, .adv_w = 87, .box_w = 5, .box_h = 14, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 837, .adv_w = 118, .box_w = 6, .box_h = 2, .ofs_x = 1, .ofs_y = 4}
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
    0, 0, -12, 0, 0, 0, 0, -8,
    -4, 0, -4, 0, 0, -23, -4, 0,
    -19, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -6, 0, 0, -6, -7,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -6, -15, -10, 0, -21,
    0, 0, -4, 0, 0, 0, 0, 0,
    -7, -6, 0, 0, -6, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 21, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -12, 0, 0,
    0, 0, -4, -6, -4, 0, -12, -4,
    0, 0, 0, -4, 0, 0, -4, 0,
    0, -19, -19, -19, 0, -23, 0, 0,
    0, -13, 10, 0, 0, -22, -4, -23,
    0, 0, -23, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -15, 0, 0,
    0, -15, 0, 0, -8, 0, 0, 0,
    -19, -15, -8, -19, -19, 0, 0, 0,
    2, 0, 0, 0, 0, 0, -6, 0,
    -10, -6, 0, 0, -13, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -29, -8, -8, -23, 0, -4, -8, 0,
    0, 0, 0, 0, 0, -8, -12, 0,
    0, -12, -12, 0, -8, 0, 0, 0,
    0, -8, 0, 0, -4, 0, 0, 0,
    -2, 0, 0, -2, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -4, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -6, 0, 0,
    0, 0, 0, 0, 0, -7, -6, -5,
    -6, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -2, 0, 0, -14,
    0, 0, 0, 0, 0, -5, 0, 0,
    -8, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -4, 0, 0, 0, -4, 0, 0, -4,
    0, -6, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -6, 0, -6,
    0, 0, 0, -4, 0, 0, -4, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -21, 0,
    0, -2, 0, -10, -3, 0, -2, -2,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -4, 0, -4, 0, 0,
    0, 0, 0, 0, -2, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -4, 0, -6, 0, 0, 0,
    0, 0, 0, -4, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -23, -2, 0, -15, 0, 0, 0,
    0, 0, 0, 0, 0, -19, 0, -8,
    0, 0, -8, 0, 0, -25, -36, -21,
    0, -33, 0, -4, -6, -10, 8, 0,
    0, -18, -6, -23, 0, 0, -20, 0,
    0, 0, 0, -8, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -4,
    0, -4, 0, -8, 0, 0, 0, 0,
    0, 0, 0, 0, -8, 0, 0, 0,
    0, 0, 0, -4, -8, -4, -6, -10,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -3, 0, 0, 0, -3,
    0, 4, -13, 0, 0, 0, 0, 0,
    0, 0, 0, -13, 0, 0, -15, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -6, 0, 0, 0, 0, 0, 0,
    0, 0, -4, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -3, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -4, 0,
    -8, 0, 0, 0, 0, 0, 0, 0,
    0, -8, 0, 0, -3, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -3, 0, 0, 0, -3, 0, -19, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -6, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -3, -8, -7,
    0, 0, 0, -10, -8, -12, 0, 0,
    -12, -2, 0, 0, 0, -12, 0, 0,
    0, 0, 0, 0, 0, 0, -15, 0,
    0, 0, 0, -3, 0, 0, -36, -35,
    -16, 0, -41, 0, 0, -2, 0, 0,
    0, 0, -13, -2, -21, -8, 0, -21,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -2, 0, 0,
    0, 0, 0, 0, 0, 0, -2, -5,
    0, -6, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -4, -13, 2, -17, 0, 0, 0,
    0, 0, 0, 0, -2, -19, 0, 0,
    -12, 0, 0, 0, 0, 0, -3, -10,
    -5, 0, 0, 0, 0, 0, -2, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -4, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -2, 0, -3, 0, 0,
    -2, 0, -2, -2, -10, -5, 0, -11,
    -4, -3, -6, 0, 0, 0, 0, 0,
    -5, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -8, 0, 0, 0, 0, 0,
    0, 0, 0, -3, 0, 0, 0, 0,
    0, 0, 0, -2, -2, -5, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -2, 0, -3, -4, 0, 0, -12, -19,
    -19, -27, 0, 0, 0, 0, 0, 0,
    0, 0, -25, -4, -4, -35, 2, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -4, 0, 0, -4, -4, 0, 0, 0,
    0, 0, 0, 0, 0, -15, -19, -15,
    -25, 0, 0, 0, 0, 0, 0, 0,
    0, -36, -8, -9, -29, 0, -2, -6,
    0, 0, 0, 0, 0, 0, -7, -18,
    0, 0, -18, -15, 0, -10, 0, 0,
    -6, 0, 0, 0, -13, -19, -8, -17,
    0, 0, 0, 0, 0, 0, 0, 0,
    -21, -4, -10, -20, 0, -6, -6, 0,
    0, 0, 0, 0, 0, -6, -12, 0,
    0, -12, -11, 0, -7, 0, 0, -4,
    0, 0, 0, -12, -23, -19, -27, 0,
    0, 0, 0, 0, 0, 0, 0, -33,
    -10, -13, -40, 0, -4, -6, 0, 0,
    0, 0, 0, 0, -13, -21, 0, 0,
    -21, -19, -6, -12, -4, 0, -6, -4,
    -6, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -4,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -4, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -8, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -6, -4, 0, -5, 0, -6,
    0, 0, 0, -8, 0, 0, 0, 0,
    0, 0, 0, -12, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -3,
    0, -5, -3, -4, -5, 0, -4, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -8, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -3, 0, 0, -3, 0, 6, -6, -23,
    4, -8, 0, 0, 0, 0, 0, 0,
    0, 6, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 4, 0, 0, 0, 4, 0, 0,
    0, 0, 0, 0, -6, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -12, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 15, 0, 0, 0, 0, -5, 0,
    0, -5, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    13, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -2, 0, -10, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -4, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -8, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 4, 0, 0,
    0, 4, 0, 0, 0, 0, 0, 0,
    -4, 0, -4, -3, -6, 0, 0, 0,
    0, 0, 0, 0, -4, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -2, 0, 0, -2, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 4, 0, 0, 0, 4,
    0, 0, 0, 0, 0, 0, 0, 0,
    -4, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -13, -23,
    -6, -17, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -11, 0, 0, -8, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -9, 0, 0,
    -10, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -2, -3,
    0, 0, -3, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -4,
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
const lv_font_t conthrax_12 = {
#else
lv_font_t conthrax_12 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 14,          /*The maximum line height required by the font*/
    .base_line = 3,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -1,
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc           /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
};



#endif /*#if CONTHRAX_12*/

