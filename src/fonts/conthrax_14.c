/*******************************************************************************
 * Size: 14 px
 * Bpp: 1
 * Opts: 
 ******************************************************************************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#ifndef CONTHRAX_14
#define CONTHRAX_14 1
#endif

#if CONTHRAX_14

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */
    0x0,

    /* U+0021 "!" */
    0xff, 0xfc, 0xc,

    /* U+0022 "\"" */
    0xde, 0xf7, 0xb0,

    /* U+0023 "#" */
    0x33, 0xc, 0xcf, 0xfc, 0xcc, 0x33, 0xc, 0xcf,
    0xfc, 0xcc, 0x33, 0xc, 0xc0,

    /* U+0024 "$" */
    0x8, 0x2, 0xf, 0xff, 0x20, 0xc8, 0x3f, 0xe0,
    0x8c, 0x23, 0x8, 0xff, 0xe0, 0x80, 0x20,

    /* U+0025 "%" */
    0xfc, 0x22, 0x11, 0x8, 0x4c, 0x21, 0x60, 0xfd,
    0xfc, 0xa, 0x10, 0x68, 0x43, 0x21, 0x8, 0x84,
    0x43, 0xf0,

    /* U+0026 "&" */
    0x7f, 0x6, 0x30, 0x63, 0x6, 0x30, 0x3f, 0x1,
    0xc6, 0x7c, 0xce, 0x7c, 0xc3, 0x8c, 0x3c, 0xfe,
    0x60,

    /* U+0027 "'" */
    0xff,

    /* U+0028 "(" */
    0x36, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0x63,

    /* U+0029 ")" */
    0xe1, 0x86, 0x31, 0x8c, 0x63, 0x18, 0xc6, 0x31,
    0x8c, 0xdc,

    /* U+002A "*" */
    0x10, 0x22, 0x4f, 0xf1, 0x5, 0x19, 0x0,

    /* U+002B "+" */
    0x18, 0x30, 0x67, 0xf1, 0x83, 0x6, 0x0,

    /* U+002C "," */
    0xfe,

    /* U+002D "-" */
    0xf8,

    /* U+002E "." */
    0xc0,

    /* U+002F "/" */
    0xc, 0x20, 0x86, 0x10, 0x43, 0x8, 0x21, 0x86,
    0x10, 0xc3, 0x0,

    /* U+0030 "0" */
    0x7f, 0xd8, 0xf, 0x1, 0xe0, 0x3c, 0x7, 0x80,
    0xf0, 0x1e, 0x3, 0xc0, 0x6f, 0xf8,

    /* U+0031 "1" */
    0xf3, 0x33, 0x33, 0x33, 0x33,

    /* U+0032 "2" */
    0xff, 0xc0, 0xc, 0x1, 0x80, 0x37, 0xfd, 0x80,
    0x30, 0x6, 0x0, 0xc0, 0x1f, 0xfc,

    /* U+0033 "3" */
    0xff, 0x80, 0x30, 0xc, 0x3, 0xff, 0x80, 0x30,
    0xc, 0x3, 0x0, 0xff, 0xe0,

    /* U+0034 "4" */
    0x1, 0xe0, 0x1b, 0x3, 0x98, 0x78, 0xc7, 0x6,
    0x70, 0x31, 0xff, 0xe0, 0xc, 0x0, 0x60, 0x3,
    0x0,

    /* U+0035 "5" */
    0xff, 0xf8, 0x3, 0x0, 0x60, 0xf, 0xfc, 0x0,
    0xc0, 0x18, 0x3, 0x0, 0x7f, 0xf8,

    /* U+0036 "6" */
    0x7f, 0xf8, 0x3, 0x0, 0x60, 0xf, 0xfd, 0x80,
    0xf0, 0x1e, 0x3, 0xe0, 0x6f, 0xf8,

    /* U+0037 "7" */
    0xff, 0xe0, 0xc, 0x3, 0x80, 0xe0, 0x38, 0xe,
    0x3, 0x80, 0xf0, 0x1c, 0x7, 0x0,

    /* U+0038 "8" */
    0x7f, 0xd8, 0xf, 0x1, 0xe0, 0x37, 0xfd, 0x80,
    0xf0, 0x1e, 0x3, 0xc0, 0x6f, 0xf8,

    /* U+0039 "9" */
    0x7f, 0xd8, 0xf, 0x1, 0xe0, 0x37, 0xfe, 0x0,
    0xc0, 0x18, 0x3, 0x0, 0xff, 0xf8,

    /* U+003A ":" */
    0xc0, 0x30,

    /* U+003B ";" */
    0xc0, 0x3f, 0x80,

    /* U+003C "<" */
    0x3, 0xe, 0x78, 0xc0, 0xe0, 0x38, 0xe, 0x0,

    /* U+003D "=" */
    0xfe, 0x0, 0x0, 0xf, 0xe0,

    /* U+003E ">" */
    0x80, 0xf0, 0x1e, 0x7, 0x6, 0x3c, 0xe0, 0x80,

    /* U+003F "?" */
    0xfe, 0x3, 0x3, 0x7, 0xf, 0x3c, 0x30, 0x30,
    0x0, 0x0, 0x30,

    /* U+0040 "@" */
    0x7f, 0xd8, 0xf, 0x1, 0xe7, 0xfc, 0xc7, 0x98,
    0xf3, 0x1e, 0x3e, 0xc0, 0x18, 0x3, 0x80, 0x3f,
    0xf0,

    /* U+0041 "A" */
    0x7, 0x0, 0x2c, 0x3, 0x30, 0x19, 0x81, 0x8e,
    0x1c, 0x30, 0xe1, 0xcf, 0xfe, 0x60, 0x3f, 0x0,
    0xc0,

    /* U+0042 "B" */
    0xff, 0xd8, 0xf, 0x1, 0xe0, 0x3f, 0xfd, 0x80,
    0xf0, 0x1e, 0x3, 0xc0, 0x7f, 0xf8,

    /* U+0043 "C" */
    0x7f, 0xf8, 0x3, 0x0, 0x60, 0xc, 0x1, 0x80,
    0x30, 0x6, 0x0, 0xc0, 0xf, 0xfc,

    /* U+0044 "D" */
    0xff, 0x98, 0x1b, 0x1, 0xe0, 0x3c, 0x7, 0x80,
    0xf0, 0x1e, 0x3, 0xc0, 0xdf, 0xf0,

    /* U+0045 "E" */
    0x7f, 0xfc, 0x3, 0x0, 0x60, 0xf, 0xff, 0x80,
    0x30, 0x6, 0x0, 0xe0, 0xf, 0xfc,

    /* U+0046 "F" */
    0x7f, 0xf8, 0x3, 0x0, 0x60, 0xf, 0xff, 0x80,
    0x30, 0x6, 0x0, 0xc0, 0x18, 0x0,

    /* U+0047 "G" */
    0x7f, 0xf8, 0x3, 0x0, 0x60, 0xc, 0x7f, 0x80,
    0xf0, 0x1e, 0x3, 0xc0, 0x6f, 0xfc,

    /* U+0048 "H" */
    0xc0, 0x78, 0xf, 0x1, 0xe0, 0x3f, 0xff, 0x80,
    0xf0, 0x1e, 0x3, 0xc0, 0x78, 0xc,

    /* U+0049 "I" */
    0xff, 0xff, 0xf0,

    /* U+004A "J" */
    0x0, 0xc0, 0x30, 0xc, 0x3, 0x0, 0xc0, 0x30,
    0xc, 0x3, 0x0, 0xff, 0xe0,

    /* U+004B "K" */
    0xc0, 0xd8, 0x33, 0xc, 0x63, 0x8f, 0xe1, 0x8e,
    0x30, 0xc6, 0xc, 0xc1, 0xd8, 0x1c,

    /* U+004C "L" */
    0xc0, 0x30, 0xc, 0x3, 0x0, 0xc0, 0x30, 0xc,
    0x3, 0x0, 0xc0, 0x1f, 0xf0,

    /* U+004D "M" */
    0xf0, 0x1f, 0xa0, 0x2f, 0x60, 0xde, 0xc1, 0xbc,
    0xc6, 0x79, 0x8c, 0xf1, 0x11, 0xe3, 0x63, 0xc6,
    0xc7, 0x87, 0xc,

    /* U+004E "N" */
    0xf0, 0x3d, 0x83, 0xd8, 0x3c, 0xc3, 0xc6, 0x3c,
    0x63, 0xc3, 0x3c, 0x1b, 0xc1, 0xbc, 0xf,

    /* U+004F "O" */
    0x7f, 0xec, 0x3, 0xc0, 0x3c, 0x3, 0xc0, 0x3c,
    0x3, 0xc0, 0x3c, 0x3, 0xc0, 0x37, 0xfe,

    /* U+0050 "P" */
    0xff, 0xd8, 0xf, 0x1, 0xe0, 0x3c, 0x7, 0xff,
    0xb0, 0x6, 0x0, 0xc0, 0x18, 0x0,

    /* U+0051 "Q" */
    0x7f, 0xee, 0x3, 0xc0, 0x3c, 0x3, 0xc0, 0x3c,
    0x3, 0xc0, 0x3c, 0x3, 0xe0, 0x77, 0xfe, 0x1,
    0xc0, 0xe, 0x0, 0x60,

    /* U+0052 "R" */
    0xff, 0xcc, 0x6, 0xc0, 0x6c, 0x6, 0xc0, 0x6f,
    0xfc, 0xc1, 0x8c, 0xc, 0xc0, 0xec, 0x6,

    /* U+0053 "S" */
    0x7f, 0xf0, 0xc, 0x3, 0x0, 0x7f, 0x80, 0x30,
    0xc, 0x3, 0x0, 0xff, 0xe0,

    /* U+0054 "T" */
    0xff, 0xf0, 0x60, 0x6, 0x0, 0x60, 0x6, 0x0,
    0x60, 0x6, 0x0, 0x60, 0x6, 0x0, 0x60,

    /* U+0055 "U" */
    0xc0, 0x3c, 0x3, 0xc0, 0x3c, 0x3, 0xc0, 0x3c,
    0x3, 0xc0, 0x3c, 0x3, 0xc0, 0x37, 0xfe,

    /* U+0056 "V" */
    0xe0, 0x1b, 0x1, 0xdc, 0xc, 0x60, 0x61, 0x86,
    0xc, 0x30, 0x33, 0x1, 0x98, 0x5, 0x80, 0x38,
    0x0,

    /* U+0057 "W" */
    0xe1, 0xe0, 0xd8, 0x78, 0x76, 0x1a, 0x19, 0x84,
    0xc6, 0x33, 0x31, 0x8c, 0xcc, 0xc3, 0x21, 0x30,
    0xd8, 0x6c, 0x1e, 0x1a, 0x7, 0x3, 0x80,

    /* U+0058 "X" */
    0x70, 0x73, 0xe, 0x19, 0xc0, 0xf8, 0x7, 0x0,
    0xf0, 0x1d, 0x83, 0x8c, 0x70, 0xee, 0x7,

    /* U+0059 "Y" */
    0xe0, 0x76, 0x6, 0x30, 0xc3, 0x9c, 0x19, 0x80,
    0xf0, 0x6, 0x0, 0x60, 0x6, 0x0, 0x60,

    /* U+005A "Z" */
    0xff, 0xc0, 0x1c, 0xf, 0x83, 0xe1, 0xf8, 0xfc,
    0x3f, 0x7, 0x80, 0xc0, 0x1f, 0xfc,

    /* U+005B "[" */
    0xfe, 0x31, 0x8c, 0x63, 0x18, 0xc6, 0x31, 0x8c,
    0x63, 0x1f,

    /* U+005C "\\" */
    0xc3, 0x4, 0x18, 0x60, 0x82, 0xc, 0x10, 0x41,
    0x82, 0x8, 0x30,

    /* U+005D "]" */
    0xf8, 0xc6, 0x31, 0x8c, 0x63, 0x18, 0xc6, 0x31,
    0x8c, 0x7f,

    /* U+005E "^" */
    0x30, 0x51, 0xa2, 0x64, 0x58, 0xa1, 0x80,

    /* U+005F "_" */
    0xff, 0x80,

    /* U+0060 "`" */
    0xc9, 0x90,

    /* U+0061 "a" */
    0xff, 0x80, 0x30, 0xd, 0xff, 0xc0, 0xf0, 0x3c,
    0xf, 0xff,

    /* U+0062 "b" */
    0xc0, 0x18, 0x3, 0x0, 0x7f, 0xec, 0x7, 0x80,
    0xf0, 0x1e, 0x3, 0xc0, 0x78, 0xd, 0xff, 0x0,

    /* U+0063 "c" */
    0x7f, 0xe0, 0x30, 0x18, 0xc, 0x6, 0x3, 0x0,
    0xff,

    /* U+0064 "d" */
    0x0, 0x60, 0xc, 0x1, 0xbf, 0xfc, 0x7, 0x80,
    0xf0, 0x1e, 0x3, 0xc0, 0x78, 0xd, 0xff, 0x0,

    /* U+0065 "e" */
    0x7f, 0xf0, 0x3c, 0xf, 0xff, 0xc0, 0x30, 0xc,
    0x1, 0xff,

    /* U+0066 "f" */
    0x1c, 0xc3, 0x3f, 0x30, 0xc3, 0xc, 0x30, 0xc3,
    0x0,

    /* U+0067 "g" */
    0x7f, 0xd8, 0xf, 0x1, 0xe0, 0x3c, 0x7, 0x80,
    0xdf, 0xf8, 0x3, 0x0, 0x60, 0xf, 0xff, 0x0,

    /* U+0068 "h" */
    0xc0, 0x18, 0x3, 0x0, 0x7f, 0xec, 0x7, 0x80,
    0xf0, 0x1e, 0x3, 0xc0, 0x78, 0xf, 0x1, 0x80,

    /* U+0069 "i" */
    0xc3, 0xff, 0xfc,

    /* U+006A "j" */
    0x18, 0x0, 0x31, 0x8c, 0x63, 0x18, 0xc6, 0x31,
    0xf8,

    /* U+006B "k" */
    0xc0, 0x30, 0xc, 0x3, 0x7, 0xc1, 0xb0, 0xcf,
    0xe3, 0xc, 0xc1, 0xb0, 0x6c, 0xc,

    /* U+006C "l" */
    0xff, 0xff, 0xfc,

    /* U+006D "m" */
    0x7f, 0xfb, 0xc, 0x3c, 0x30, 0xf0, 0xc3, 0xc3,
    0xf, 0xc, 0x3c, 0x30, 0xf0, 0xc3,

    /* U+006E "n" */
    0x7f, 0xd8, 0xf, 0x1, 0xe0, 0x3c, 0x7, 0x80,
    0xf0, 0x1e, 0x3,

    /* U+006F "o" */
    0x7f, 0xd8, 0xf, 0x1, 0xe0, 0x3c, 0x7, 0x80,
    0xf0, 0x1b, 0xfe,

    /* U+0070 "p" */
    0x7f, 0xd8, 0xf, 0x1, 0xe0, 0x3c, 0x7, 0x80,
    0xf0, 0x1f, 0xfe, 0xc0, 0x18, 0x3, 0x0, 0x0,

    /* U+0071 "q" */
    0x7f, 0xd8, 0xf, 0x1, 0xe0, 0x3c, 0x7, 0x80,
    0xf0, 0x1b, 0xff, 0x0, 0x60, 0xc, 0x1, 0x80,

    /* U+0072 "r" */
    0x7f, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,

    /* U+0073 "s" */
    0xff, 0xf0, 0xc, 0x3, 0xfe, 0x0, 0xc0, 0x30,
    0xf, 0xfe,

    /* U+0074 "t" */
    0x30, 0xcf, 0xcc, 0x30, 0xc3, 0xc, 0x30, 0x70,

    /* U+0075 "u" */
    0xc0, 0x78, 0xf, 0x1, 0xe0, 0x3c, 0x7, 0x80,
    0xf0, 0x1b, 0xfe,

    /* U+0076 "v" */
    0xe0, 0x6c, 0xd, 0x83, 0x18, 0x61, 0x18, 0x33,
    0x2, 0xc0, 0x70,

    /* U+0077 "w" */
    0xc1, 0xc3, 0x63, 0xc3, 0x63, 0x46, 0x62, 0x66,
    0x36, 0x66, 0x36, 0x2c, 0x34, 0x3c, 0x1c, 0x3c,

    /* U+0078 "x" */
    0x60, 0xe6, 0x38, 0x6c, 0x7, 0x0, 0xf0, 0x77,
    0x1c, 0x77, 0x7,

    /* U+0079 "y" */
    0xc0, 0x7c, 0x19, 0x87, 0x18, 0xc3, 0x38, 0x36,
    0x7, 0x80, 0x70, 0xc, 0x1, 0x80, 0xe0, 0x0,

    /* U+007A "z" */
    0xff, 0x80, 0xc1, 0xe3, 0xe7, 0xc7, 0x83, 0x1,
    0xff,

    /* U+007B "{" */
    0x1c, 0xc3, 0xc, 0x30, 0xc3, 0x38, 0x30, 0xc3,
    0xc, 0x30, 0xc3, 0x7,

    /* U+007C "|" */
    0xff, 0xff, 0xff, 0xf0,

    /* U+007D "}" */
    0xe1, 0x8c, 0x63, 0x18, 0xc3, 0x31, 0x8c, 0x63,
    0x18, 0xdc,

    /* U+007E "~" */
    0xf3, 0x1c
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 75, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 60, .box_w = 2, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 4, .adv_w = 110, .box_w = 5, .box_h = 4, .ofs_x = 1, .ofs_y = 7},
    {.bitmap_index = 7, .adv_w = 191, .box_w = 10, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 20, .adv_w = 183, .box_w = 10, .box_h = 12, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 35, .adv_w = 256, .box_w = 14, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 53, .adv_w = 196, .box_w = 12, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 70, .adv_w = 61, .box_w = 2, .box_h = 4, .ofs_x = 1, .ofs_y = 7},
    {.bitmap_index = 71, .adv_w = 101, .box_w = 4, .box_h = 16, .ofs_x = 2, .ofs_y = -3},
    {.bitmap_index = 79, .adv_w = 101, .box_w = 5, .box_h = 16, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 89, .adv_w = 130, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 3},
    {.bitmap_index = 96, .adv_w = 137, .box_w = 7, .box_h = 7, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 103, .adv_w = 60, .box_w = 2, .box_h = 4, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 104, .adv_w = 86, .box_w = 5, .box_h = 1, .ofs_x = 1, .ofs_y = 5},
    {.bitmap_index = 105, .adv_w = 60, .box_w = 2, .box_h = 1, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 106, .adv_w = 93, .box_w = 6, .box_h = 14, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 117, .adv_w = 203, .box_w = 11, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 131, .adv_w = 76, .box_w = 4, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 136, .adv_w = 193, .box_w = 11, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 150, .adv_w = 189, .box_w = 10, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 163, .adv_w = 203, .box_w = 13, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 180, .adv_w = 193, .box_w = 11, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 194, .adv_w = 194, .box_w = 11, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 208, .adv_w = 186, .box_w = 11, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 222, .adv_w = 202, .box_w = 11, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 236, .adv_w = 194, .box_w = 11, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 250, .adv_w = 60, .box_w = 2, .box_h = 6, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 252, .adv_w = 60, .box_w = 2, .box_h = 9, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 255, .adv_w = 137, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 263, .adv_w = 137, .box_w = 7, .box_h = 5, .ofs_x = 1, .ofs_y = 3},
    {.bitmap_index = 268, .adv_w = 137, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 276, .adv_w = 141, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 287, .adv_w = 196, .box_w = 11, .box_h = 12, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 304, .adv_w = 218, .box_w = 13, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 321, .adv_w = 206, .box_w = 11, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 335, .adv_w = 191, .box_w = 11, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 349, .adv_w = 208, .box_w = 11, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 363, .adv_w = 190, .box_w = 11, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 377, .adv_w = 187, .box_w = 11, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 391, .adv_w = 207, .box_w = 11, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 405, .adv_w = 203, .box_w = 11, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 419, .adv_w = 64, .box_w = 2, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 422, .adv_w = 175, .box_w = 10, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 435, .adv_w = 191, .box_w = 11, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 449, .adv_w = 179, .box_w = 10, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 462, .adv_w = 267, .box_w = 15, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 481, .adv_w = 219, .box_w = 12, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 496, .adv_w = 221, .box_w = 12, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 511, .adv_w = 197, .box_w = 11, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 525, .adv_w = 221, .box_w = 12, .box_h = 13, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 545, .adv_w = 203, .box_w = 12, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 560, .adv_w = 192, .box_w = 10, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 573, .adv_w = 196, .box_w = 12, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 588, .adv_w = 219, .box_w = 12, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 603, .adv_w = 218, .box_w = 13, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 620, .adv_w = 292, .box_w = 18, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 643, .adv_w = 200, .box_w = 12, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 658, .adv_w = 201, .box_w = 12, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 673, .adv_w = 191, .box_w = 11, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 687, .adv_w = 101, .box_w = 5, .box_h = 16, .ofs_x = 2, .ofs_y = -3},
    {.bitmap_index = 697, .adv_w = 93, .box_w = 6, .box_h = 14, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 708, .adv_w = 101, .box_w = 5, .box_h = 16, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 718, .adv_w = 137, .box_w = 7, .box_h = 7, .ofs_x = 1, .ofs_y = 3},
    {.bitmap_index = 725, .adv_w = 144, .box_w = 9, .box_h = 1, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 727, .adv_w = 88, .box_w = 3, .box_h = 4, .ofs_x = 1, .ofs_y = 9},
    {.bitmap_index = 729, .adv_w = 177, .box_w = 10, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 739, .adv_w = 195, .box_w = 11, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 755, .adv_w = 163, .box_w = 9, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 764, .adv_w = 195, .box_w = 11, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 780, .adv_w = 177, .box_w = 10, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 790, .adv_w = 110, .box_w = 6, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 799, .adv_w = 195, .box_w = 11, .box_h = 11, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 815, .adv_w = 197, .box_w = 11, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 831, .adv_w = 60, .box_w = 2, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 834, .adv_w = 63, .box_w = 5, .box_h = 14, .ofs_x = -2, .ofs_y = -3},
    {.bitmap_index = 843, .adv_w = 178, .box_w = 10, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 857, .adv_w = 60, .box_w = 2, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 860, .adv_w = 248, .box_w = 14, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 874, .adv_w = 198, .box_w = 11, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 885, .adv_w = 194, .box_w = 11, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 896, .adv_w = 195, .box_w = 11, .box_h = 11, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 912, .adv_w = 195, .box_w = 11, .box_h = 11, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 928, .adv_w = 137, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 936, .adv_w = 175, .box_w = 10, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 946, .adv_w = 110, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 954, .adv_w = 196, .box_w = 11, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 965, .adv_w = 186, .box_w = 11, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 976, .adv_w = 261, .box_w = 16, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 992, .adv_w = 177, .box_w = 11, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1003, .adv_w = 181, .box_w = 11, .box_h = 11, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 1019, .adv_w = 164, .box_w = 9, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1028, .adv_w = 101, .box_w = 6, .box_h = 16, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 1040, .adv_w = 95, .box_w = 2, .box_h = 14, .ofs_x = 2, .ofs_y = -2},
    {.bitmap_index = 1044, .adv_w = 101, .box_w = 5, .box_h = 16, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 1054, .adv_w = 137, .box_w = 7, .box_h = 2, .ofs_x = 1, .ofs_y = 4}
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
    0, 0, -13, 0, 0, 0, 0, -9,
    -4, 0, -4, 0, 0, -27, -4, 0,
    -22, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -7, 0, 0, -7, -8,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -7, -18, -11, 0, -25,
    0, 0, -4, 0, 0, 0, 0, 0,
    -8, -7, 0, 0, -7, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 25, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -13, 0, 0,
    0, 0, -4, -7, -4, 0, -13, -4,
    0, 0, 0, -4, 0, 0, -4, 0,
    0, -22, -22, -22, 0, -27, 0, 0,
    0, -15, 11, 0, 0, -25, -4, -27,
    0, 0, -27, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -18, 0, 0,
    0, -18, 0, 0, -9, 0, 0, 0,
    -22, -18, -9, -22, -22, 0, 0, 0,
    2, 0, 0, 0, 0, 0, -7, 0,
    -12, -7, 0, 0, -16, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -34, -9, -9, -27, 0, -4, -9, 0,
    0, 0, 0, 0, 0, -9, -13, 0,
    0, -13, -13, 0, -9, 0, 0, 0,
    0, -9, 0, 0, -4, 0, 0, 0,
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
    0, 0, 0, 0, 0, -7, 0, 0,
    0, 0, 0, 0, 0, -8, -6, -6,
    -7, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -3, 0, 0, -16,
    0, 0, 0, 0, 0, -6, 0, 0,
    -9, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -5, 0, 0, 0, -4, 0, 0, -4,
    0, -6, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -7, 0, -7,
    0, 0, 0, -4, 0, 0, -4, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -25, 0,
    0, -2, 0, -11, -4, 0, -2, -2,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -4, 0, -4, 0, 0,
    0, 0, 0, 0, -2, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -4, 0, -7, 0, 0, 0,
    0, 0, 0, -4, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -27, -3, 0, -18, 0, 0, 0,
    0, 0, 0, 0, 0, -22, 0, -9,
    0, 0, -9, 0, 0, -29, -43, -25,
    0, -38, 0, -4, -7, -12, 9, 0,
    0, -21, -7, -27, 0, 0, -23, 0,
    0, 0, 0, -9, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -4,
    0, -4, 0, -9, 0, 0, 0, 0,
    0, 0, 0, 0, -9, 0, 0, 0,
    0, 0, 0, -4, -9, -4, -7, -11,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -4, 0, 0, 0, -4,
    0, 4, -16, 0, 0, 0, 0, 0,
    0, 0, 0, -15, 0, 0, -18, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -7, 0, 0, 0, 0, 0, 0,
    0, 0, -4, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -4, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -4, 0,
    -9, 0, 0, 0, 0, 0, 0, 0,
    0, -9, 0, 0, -3, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -4, 0, 0, 0, -4, 0, -22, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -7, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -4, -9, -9,
    0, 0, 0, -11, -9, -14, 0, 0,
    -14, -2, 0, 0, 0, -13, 0, 0,
    0, 0, 0, 0, 0, 0, -18, 0,
    0, 0, 0, -3, 0, 0, -43, -40,
    -18, 0, -47, 0, 0, -2, 0, 0,
    0, 0, -16, -2, -25, -9, 0, -25,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -2, 0, 0,
    0, 0, 0, 0, 0, 0, -3, -6,
    0, -7, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -4, -16, 2, -20, 0, 0, 0,
    0, 0, 0, 0, -2, -22, 0, 0,
    -14, 0, 0, 0, 0, 0, -4, -12,
    -6, 0, 0, 0, 0, 0, -2, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -4, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -2, 0, -4, 0, 0,
    -2, 0, -2, -3, -12, -6, 0, -13,
    -4, -3, -7, 0, 0, 0, 0, 0,
    -6, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -9, 0, 0, 0, 0, 0,
    0, 0, 0, -4, 0, 0, 0, 0,
    0, 0, 0, -2, -2, -6, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -3, 0, -4, -4, 0, 0, -13, -22,
    -22, -31, 0, 0, 0, 0, 0, 0,
    0, 0, -29, -4, -5, -40, 2, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -4, 0, 0, -4, -4, 0, 0, 0,
    0, 0, 0, 0, 0, -18, -22, -18,
    -29, 0, 0, 0, 0, 0, 0, 0,
    0, -43, -9, -11, -34, 0, -3, -7,
    0, 0, 0, 0, 0, 0, -8, -21,
    0, 0, -21, -17, 0, -11, 0, 0,
    -7, 0, 0, 0, -16, -22, -9, -20,
    0, 0, 0, 0, 0, 0, 0, 0,
    -25, -4, -12, -23, 0, -7, -7, 0,
    0, 0, 0, 0, 0, -7, -14, 0,
    0, -14, -13, 0, -9, 0, 0, -4,
    0, 0, 0, -13, -27, -22, -31, 0,
    0, 0, 0, 0, 0, 0, 0, -38,
    -11, -15, -46, 0, -5, -7, 0, 0,
    0, 0, 0, 0, -16, -25, 0, 0,
    -25, -22, -7, -13, -4, 0, -7, -4,
    -7, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -4,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -4, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -9, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -7, -4, 0, -6, 0, -7,
    0, 0, 0, -9, 0, 0, 0, 0,
    0, 0, 0, -13, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -3,
    0, -6, -4, -4, -6, 0, -4, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -9, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -3, 0, 0, -3, 0, 7, -7, -27,
    4, -9, 0, 0, 0, 0, 0, 0,
    0, 7, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 4, 0, 0, 0, 4, 0, 0,
    0, 0, 0, 0, -7, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -13, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 17, 0, 0, 0, 0, -6, 0,
    0, -6, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    16, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -3, 0, -12, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -4, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -9, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 4, 0, 0,
    0, 4, 0, 0, 0, 0, 0, 0,
    -5, 0, -4, -4, -7, 0, 0, 0,
    0, 0, 0, 0, -4, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -3, 0, 0, -3, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 4, 0, 0, 0, 4,
    0, 0, 0, 0, 0, 0, 0, 0,
    -4, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -16, -27,
    -7, -20, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -12, 0, 0, -10, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -11, 0, 0,
    -11, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -2, -4,
    0, 0, -4, 0, 0, 0, 0, 0,
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
const lv_font_t conthrax_14 = {
#else
lv_font_t conthrax_14 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 16,          /*The maximum line height required by the font*/
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



#endif /*#if CONTHRAX_14*/

