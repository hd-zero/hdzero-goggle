/*******************************************************************************
 * Size: 8 px
 * Bpp: 1
 * Opts: 
 ******************************************************************************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#ifndef CONTHRAX_08
#define CONTHRAX_08 1
#endif

#if CONTHRAX_08

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */
    0x0,

    /* U+0021 "!" */
    0xf2,

    /* U+0022 "\"" */
    0x6d, 0x80,

    /* U+0023 "#" */
    0x4b, 0xf4, 0x92, 0xfd, 0x20,

    /* U+0024 "$" */
    0xfd, 0x29, 0xf2, 0xfc, 0x80,

    /* U+0025 "%" */
    0xf4, 0x94, 0xff, 0x15, 0x15, 0x27,

    /* U+0026 "&" */
    0x70, 0xb1, 0x43, 0x29, 0x93, 0x3d, 0x0,

    /* U+0027 "'" */
    0x54,

    /* U+0028 "(" */
    0x6a, 0xaa, 0x90,

    /* U+0029 ")" */
    0x95, 0x55, 0x60,

    /* U+002A "*" */
    0x25, 0x5c, 0xa5, 0x0,

    /* U+002B "+" */
    0x21, 0x3e, 0x42, 0x0,

    /* U+002C "," */
    0xe0,

    /* U+002D "-" */
    0xe0,

    /* U+002E "." */
    0x80,

    /* U+002F "/" */
    0x24, 0xa4, 0x94, 0x80,

    /* U+0030 "0" */
    0x7a, 0x18, 0x61, 0x87, 0xf0,

    /* U+0031 "1" */
    0xd5, 0x50,

    /* U+0032 "2" */
    0xfc, 0x1f, 0xe0, 0x83, 0xf0,

    /* U+0033 "3" */
    0xfc, 0x1f, 0xc1, 0x7, 0xf0,

    /* U+0034 "4" */
    0xc, 0x29, 0x92, 0x27, 0xe0, 0x80,

    /* U+0035 "5" */
    0xfe, 0xf, 0xc1, 0x7, 0xf0,

    /* U+0036 "6" */
    0x7e, 0xf, 0xe1, 0x85, 0xf0,

    /* U+0037 "7" */
    0xfc, 0x30, 0x84, 0x21, 0x80,

    /* U+0038 "8" */
    0xfe, 0x1f, 0xe1, 0x87, 0xf0,

    /* U+0039 "9" */
    0xfa, 0x1f, 0xc1, 0x7, 0xe0,

    /* U+003A ":" */
    0x90,

    /* U+003B ";" */
    0x9c,

    /* U+003C "<" */
    0x3, 0xcc, 0x30,

    /* U+003D "=" */
    0xf8, 0x3e,

    /* U+003E ">" */
    0xc, 0x33, 0xc0,

    /* U+003F "?" */
    0xf1, 0x36, 0x40, 0x40,

    /* U+0040 "@" */
    0xfe, 0x1b, 0xe9, 0xa6, 0xf8, 0x1f,

    /* U+0041 "A" */
    0x18, 0x38, 0x24, 0x64, 0x7e, 0xc2,

    /* U+0042 "B" */
    0xfe, 0x1f, 0xe1, 0x87, 0xf0,

    /* U+0043 "C" */
    0x7e, 0x8, 0x20, 0x81, 0xf0,

    /* U+0044 "D" */
    0xfa, 0x18, 0x61, 0x87, 0xe0,

    /* U+0045 "E" */
    0x7e, 0xf, 0xe0, 0x81, 0xf0,

    /* U+0046 "F" */
    0x7e, 0xf, 0xe0, 0x82, 0x0,

    /* U+0047 "G" */
    0x7e, 0xb, 0xe1, 0x87, 0xf0,

    /* U+0048 "H" */
    0x86, 0x1f, 0xe1, 0x86, 0x10,

    /* U+0049 "I" */
    0xfc,

    /* U+004A "J" */
    0x4, 0x10, 0x41, 0x7, 0xe0,

    /* U+004B "K" */
    0x8a, 0x6f, 0x26, 0x8a, 0x30,

    /* U+004C "L" */
    0x82, 0x8, 0x20, 0x83, 0xf0,

    /* U+004D "M" */
    0xc3, 0xe7, 0xa5, 0xa5, 0x99, 0x99,

    /* U+004E "N" */
    0xe3, 0x46, 0xcc, 0xd8, 0xb1, 0xc0,

    /* U+004F "O" */
    0x7d, 0x6, 0xc, 0x18, 0x2f, 0x80,

    /* U+0050 "P" */
    0xfe, 0x18, 0x7f, 0x82, 0x0,

    /* U+0051 "Q" */
    0x7d, 0x6, 0xc, 0x18, 0x2f, 0x83, 0x2,

    /* U+0052 "R" */
    0xfe, 0x18, 0x7f, 0x8a, 0x10,

    /* U+0053 "S" */
    0xfe, 0xf, 0xc1, 0x7, 0xf0,

    /* U+0054 "T" */
    0xfe, 0x20, 0x40, 0x81, 0x2, 0x0,

    /* U+0055 "U" */
    0x83, 0x6, 0xc, 0x18, 0x2f, 0x80,

    /* U+0056 "V" */
    0xc2, 0x42, 0x64, 0x24, 0x38, 0x18,

    /* U+0057 "W" */
    0xcc, 0x53, 0x94, 0xad, 0x4a, 0x72, 0x8c, 0x60,

    /* U+0058 "X" */
    0x46, 0x58, 0xe1, 0xc6, 0xd8, 0xc0,

    /* U+0059 "Y" */
    0xc6, 0x88, 0xa0, 0x81, 0x2, 0x0,

    /* U+005A "Z" */
    0xfc, 0x33, 0x98, 0xc3, 0xf0,

    /* U+005B "[" */
    0xea, 0xaa, 0xb0,

    /* U+005C "\\" */
    0x91, 0x24, 0x89, 0x20,

    /* U+005D "]" */
    0xd5, 0x55, 0x70,

    /* U+005E "^" */
    0x23, 0x14, 0xa9, 0x0,

    /* U+005F "_" */
    0xf8,

    /* U+0060 "`" */
    0xa0,

    /* U+0061 "a" */
    0xfc, 0x17, 0xf1, 0x7c,

    /* U+0062 "b" */
    0x82, 0xf, 0xe1, 0x86, 0x1f, 0xc0,

    /* U+0063 "c" */
    0xfe, 0x8, 0x20, 0xfc,

    /* U+0064 "d" */
    0x4, 0x1f, 0xe1, 0x86, 0x1f, 0xc0,

    /* U+0065 "e" */
    0xfa, 0x2f, 0xe0, 0xfc,

    /* U+0066 "f" */
    0x74, 0xf4, 0x44, 0x40,

    /* U+0067 "g" */
    0xfe, 0x18, 0x61, 0xfc, 0x1f, 0xc0,

    /* U+0068 "h" */
    0x82, 0xf, 0xe1, 0x86, 0x18, 0x40,

    /* U+0069 "i" */
    0xbe,

    /* U+006A "j" */
    0x20, 0x92, 0x49, 0xe0,

    /* U+006B "k" */
    0x82, 0x8, 0xa6, 0xf2, 0x68, 0x80,

    /* U+006C "l" */
    0xfe,

    /* U+006D "m" */
    0xff, 0x26, 0x4c, 0x99, 0x20,

    /* U+006E "n" */
    0xfe, 0x18, 0x61, 0x84,

    /* U+006F "o" */
    0xfe, 0x18, 0x61, 0xfc,

    /* U+0070 "p" */
    0xfe, 0x18, 0x61, 0xfe, 0x8, 0x0,

    /* U+0071 "q" */
    0xfe, 0x18, 0x61, 0xfc, 0x10, 0x40,

    /* U+0072 "r" */
    0xfc, 0x21, 0x8, 0x0,

    /* U+0073 "s" */
    0xfa, 0xf, 0x83, 0xf8,

    /* U+0074 "t" */
    0x4f, 0x44, 0x47,

    /* U+0075 "u" */
    0x86, 0x18, 0x61, 0xfc,

    /* U+0076 "v" */
    0xc4, 0x89, 0xa1, 0x43, 0x0,

    /* U+0077 "w" */
    0x8c, 0xaa, 0x55, 0x6a, 0x66, 0x30,

    /* U+0078 "x" */
    0x4c, 0xa3, 0x1e, 0xcc,

    /* U+0079 "y" */
    0x87, 0x24, 0x9c, 0x30, 0x86, 0x0,

    /* U+007A "z" */
    0xf8, 0xdd, 0x8f, 0x80,

    /* U+007B "{" */
    0x69, 0x2c, 0x92, 0x4c,

    /* U+007C "|" */
    0xff, 0x80,

    /* U+007D "}" */
    0xc9, 0x26, 0x92, 0x58,

    /* U+007E "~" */
    0xcd, 0x80
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 43, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 34, .box_w = 1, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 2, .adv_w = 63, .box_w = 3, .box_h = 3, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 4, .adv_w = 109, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 9, .adv_w = 104, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 14, .adv_w = 146, .box_w = 8, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 20, .adv_w = 112, .box_w = 7, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 27, .adv_w = 35, .box_w = 2, .box_h = 3, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 28, .adv_w = 58, .box_w = 2, .box_h = 10, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 31, .adv_w = 58, .box_w = 2, .box_h = 10, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 34, .adv_w = 74, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 38, .adv_w = 78, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 42, .adv_w = 34, .box_w = 1, .box_h = 3, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 43, .adv_w = 49, .box_w = 3, .box_h = 1, .ofs_x = 0, .ofs_y = 3},
    {.bitmap_index = 44, .adv_w = 34, .box_w = 1, .box_h = 1, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 45, .adv_w = 53, .box_w = 3, .box_h = 9, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 49, .adv_w = 116, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 54, .adv_w = 43, .box_w = 2, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 56, .adv_w = 110, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 61, .adv_w = 108, .box_w = 6, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 66, .adv_w = 116, .box_w = 7, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 72, .adv_w = 110, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 77, .adv_w = 111, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 82, .adv_w = 106, .box_w = 6, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 87, .adv_w = 116, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 92, .adv_w = 111, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 97, .adv_w = 34, .box_w = 1, .box_h = 4, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 98, .adv_w = 34, .box_w = 1, .box_h = 6, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 99, .adv_w = 78, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 102, .adv_w = 78, .box_w = 5, .box_h = 3, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 104, .adv_w = 78, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 107, .adv_w = 81, .box_w = 4, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 111, .adv_w = 112, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 117, .adv_w = 125, .box_w = 8, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 123, .adv_w = 118, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 128, .adv_w = 109, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 133, .adv_w = 119, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 138, .adv_w = 109, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 143, .adv_w = 107, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 148, .adv_w = 119, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 153, .adv_w = 116, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 158, .adv_w = 37, .box_w = 1, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 159, .adv_w = 100, .box_w = 6, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 164, .adv_w = 109, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 169, .adv_w = 103, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 174, .adv_w = 153, .box_w = 8, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 180, .adv_w = 125, .box_w = 7, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 186, .adv_w = 126, .box_w = 7, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 192, .adv_w = 113, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 197, .adv_w = 126, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 204, .adv_w = 116, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 209, .adv_w = 110, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 214, .adv_w = 112, .box_w = 7, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 220, .adv_w = 125, .box_w = 7, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 226, .adv_w = 125, .box_w = 8, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 232, .adv_w = 167, .box_w = 10, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 240, .adv_w = 114, .box_w = 7, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 246, .adv_w = 115, .box_w = 7, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 252, .adv_w = 109, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 257, .adv_w = 58, .box_w = 2, .box_h = 10, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 260, .adv_w = 53, .box_w = 3, .box_h = 9, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 264, .adv_w = 58, .box_w = 2, .box_h = 10, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 267, .adv_w = 78, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 271, .adv_w = 82, .box_w = 5, .box_h = 1, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 272, .adv_w = 50, .box_w = 2, .box_h = 2, .ofs_x = 1, .ofs_y = 5},
    {.bitmap_index = 273, .adv_w = 101, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 277, .adv_w = 111, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 283, .adv_w = 93, .box_w = 6, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 287, .adv_w = 111, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 293, .adv_w = 101, .box_w = 6, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 297, .adv_w = 63, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 301, .adv_w = 111, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 307, .adv_w = 113, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 313, .adv_w = 35, .box_w = 1, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 314, .adv_w = 36, .box_w = 3, .box_h = 9, .ofs_x = -1, .ofs_y = -2},
    {.bitmap_index = 318, .adv_w = 102, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 324, .adv_w = 35, .box_w = 1, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 325, .adv_w = 142, .box_w = 7, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 330, .adv_w = 113, .box_w = 6, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 334, .adv_w = 111, .box_w = 6, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 338, .adv_w = 111, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 344, .adv_w = 111, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 350, .adv_w = 78, .box_w = 5, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 354, .adv_w = 100, .box_w = 6, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 358, .adv_w = 63, .box_w = 4, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 361, .adv_w = 112, .box_w = 6, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 365, .adv_w = 106, .box_w = 7, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 370, .adv_w = 149, .box_w = 9, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 376, .adv_w = 101, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 380, .adv_w = 103, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 386, .adv_w = 94, .box_w = 5, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 390, .adv_w = 58, .box_w = 3, .box_h = 10, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 394, .adv_w = 54, .box_w = 1, .box_h = 9, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 396, .adv_w = 58, .box_w = 3, .box_h = 10, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 400, .adv_w = 78, .box_w = 5, .box_h = 2, .ofs_x = 0, .ofs_y = 2}
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
    0, 0, -8, 0, 0, 0, 0, -5,
    -3, 0, -3, 0, 0, -15, -3, 0,
    -13, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -4, 0, 0, -4, -5,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -4, -10, -6, 0, -14,
    0, 0, -3, 0, 0, 0, 0, 0,
    -5, -4, 0, 0, -4, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 14, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -8, 0, 0,
    0, 0, -3, -4, -3, 0, -8, -3,
    0, 0, 0, -3, 0, 0, -3, 0,
    0, -13, -13, -13, 0, -15, 0, 0,
    0, -9, 6, 0, 0, -14, -3, -15,
    0, 0, -15, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -10, 0, 0,
    0, -10, 0, 0, -5, 0, 0, 0,
    -13, -10, -5, -13, -13, 0, 0, 0,
    1, 0, 0, 0, 0, 0, -4, 0,
    -7, -4, 0, 0, -9, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -19, -5, -5, -15, 0, -3, -5, 0,
    0, 0, 0, 0, 0, -5, -8, 0,
    0, -8, -8, 0, -5, 0, 0, 0,
    0, -5, 0, 0, -3, 0, 0, 0,
    -1, 0, 0, -1, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -3, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -4, 0, 0,
    0, 0, 0, 0, 0, -5, -4, -4,
    -4, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -2, 0, 0, -9,
    0, 0, 0, 0, 0, -3, 0, 0,
    -5, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -3, 0, 0, 0, -2, 0, 0, -3,
    0, -4, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -4, 0, -4,
    0, 0, 0, -2, 0, 0, -3, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -14, 0,
    0, -1, 0, -6, -2, 0, -1, -1,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -3, 0, -3, 0, 0,
    0, 0, 0, 0, -1, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -3, 0, -4, 0, 0, 0,
    0, 0, 0, -3, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -15, -2, 0, -10, 0, 0, 0,
    0, 0, 0, 0, 0, -13, 0, -5,
    0, 0, -5, 0, 0, -17, -24, -14,
    0, -22, 0, -2, -4, -7, 5, 0,
    0, -12, -4, -15, 0, 0, -13, 0,
    0, 0, 0, -5, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -3,
    0, -3, 0, -5, 0, 0, 0, 0,
    0, 0, 0, 0, -5, 0, 0, 0,
    0, 0, 0, -3, -5, -3, -4, -6,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -2, 0, 0, 0, -2,
    0, 3, -9, 0, 0, 0, 0, 0,
    0, 0, 0, -8, 0, 0, -10, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -4, 0, 0, 0, 0, 0, 0,
    0, 0, -3, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -2, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -3, 0,
    -5, 0, 0, 0, 0, 0, 0, 0,
    0, -5, 0, 0, -2, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -2, 0, 0, 0, -2, 0, -13, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -4, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -2, -5, -5,
    0, 0, 0, -6, -5, -8, 0, 0,
    -8, -1, 0, 0, 0, -8, 0, 0,
    0, 0, 0, 0, 0, 0, -10, 0,
    0, 0, 0, -2, 0, 0, -24, -23,
    -10, 0, -27, 0, 0, -1, 0, 0,
    0, 0, -9, -1, -14, -5, 0, -14,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -1, 0, 0,
    0, 0, 0, 0, 0, 0, -2, -4,
    0, -4, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -2, -9, 1, -11, 0, 0, 0,
    0, 0, 0, 0, -1, -13, 0, 0,
    -8, 0, 0, 0, 0, 0, -2, -7,
    -4, 0, 0, 0, 0, 0, -1, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -3, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -1, 0, -2, 0, 0,
    -1, 0, -1, -2, -7, -4, 0, -7,
    -2, -2, -4, 0, 0, 0, 0, 0,
    -4, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -5, 0, 0, 0, 0, 0,
    0, 0, 0, -2, 0, 0, 0, 0,
    0, 0, 0, -1, -1, -3, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -2, 0, -2, -2, 0, 0, -8, -13,
    -13, -18, 0, 0, 0, 0, 0, 0,
    0, 0, -17, -3, -3, -23, 1, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -3, 0, 0, -3, -3, 0, 0, 0,
    0, 0, 0, 0, 0, -10, -13, -10,
    -17, 0, 0, 0, 0, 0, 0, 0,
    0, -24, -5, -6, -19, 0, -2, -4,
    0, 0, 0, 0, 0, 0, -4, -12,
    0, 0, -12, -10, 0, -6, 0, 0,
    -4, 0, 0, 0, -9, -13, -5, -12,
    0, 0, 0, 0, 0, 0, 0, 0,
    -14, -3, -7, -13, 0, -4, -4, 0,
    0, 0, 0, 0, 0, -4, -8, 0,
    0, -8, -7, 0, -5, 0, 0, -3,
    0, 0, 0, -8, -15, -13, -18, 0,
    0, 0, 0, 0, 0, 0, 0, -22,
    -6, -9, -26, 0, -3, -4, 0, 0,
    0, 0, 0, 0, -9, -14, 0, 0,
    -14, -13, -4, -8, -3, 0, -4, -3,
    -4, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -2,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -3, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -5, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -4, -2, 0, -3, 0, -4,
    0, 0, 0, -5, 0, 0, 0, 0,
    0, 0, 0, -8, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -2,
    0, -4, -2, -3, -4, 0, -3, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -5, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -2, 0, 0, -2, 0, 4, -4, -15,
    3, -5, 0, 0, 0, 0, 0, 0,
    0, 4, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 3, 0, 0, 0, 3, 0, 0,
    0, 0, 0, 0, -4, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -8, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 10, 0, 0, 0, 0, -4, 0,
    0, -4, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    9, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -2, 0, -7, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -3, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -5, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 3, 0, 0,
    0, 3, 0, 0, 0, 0, 0, 0,
    -3, 0, -3, -2, -4, 0, 0, 0,
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
    0, 0, 0, 0, 0, 0, -9, -15,
    -4, -12, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -7, 0, 0, -6, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -6, 0, 0,
    -6, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -1, -2,
    0, 0, -2, 0, 0, 0, 0, 0,
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
const lv_font_t conthrax_08 = {
#else
lv_font_t conthrax_08 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 10,          /*The maximum line height required by the font*/
    .base_line = 2,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -1,
    .underline_thickness = 0,
#endif
    .dsc = &font_dsc           /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
};



#endif /*#if CONTHRAX_08*/

