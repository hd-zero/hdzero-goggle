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

#ifndef ROBOTOMONO_08
#define ROBOTOMONO_08 1
#endif

#if ROBOTOMONO_08

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */
    0x0,

    /* U+0021 "!" */
    0xf8,

    /* U+0022 "\"" */
    0xf0,

    /* U+0023 "#" */
    0x53, 0xd5, 0xe6, 0x0,

    /* U+0024 "$" */
    0x4a, 0x84, 0x2a, 0x40,

    /* U+0025 "%" */
    0xc7, 0x14, 0xd3, 0x0,

    /* U+0026 "&" */
    0x6a, 0x5b, 0xf0,

    /* U+0027 "'" */
    0xc0,

    /* U+0028 "(" */
    0x5a, 0xa4,

    /* U+0029 ")" */
    0xa5, 0x58,

    /* U+002A "*" */
    0x27, 0xd4, 0x20,

    /* U+002B "+" */
    0x21, 0x3e, 0x40,

    /* U+002C "," */
    0xc0,

    /* U+002D "-" */
    0xe0,

    /* U+002E "." */
    0x80,

    /* U+002F "/" */
    0x29, 0x69, 0x0,

    /* U+0030 "0" */
    0x69, 0xf9, 0x60,

    /* U+0031 "1" */
    0xd5, 0x40,

    /* U+0032 "2" */
    0xea, 0x24, 0xf0,

    /* U+0033 "3" */
    0x71, 0x21, 0x70,

    /* U+0034 "4" */
    0x11, 0x95, 0xf1, 0x0,

    /* U+0035 "5" */
    0x76, 0x19, 0x70,

    /* U+0036 "6" */
    0x6e, 0xaa, 0x60,

    /* U+0037 "7" */
    0xf2, 0x24, 0x40,

    /* U+0038 "8" */
    0xea, 0x6a, 0xe0,

    /* U+0039 "9" */
    0xea, 0x62, 0x60,

    /* U+003A ":" */
    0x90,

    /* U+003B ";" */
    0x88,

    /* U+003C "<" */
    0x3c, 0x60,

    /* U+003D "=" */
    0xe3, 0x80,

    /* U+003E ">" */
    0x87, 0xc0,

    /* U+003F "?" */
    0xe5, 0x24,

    /* U+0040 "@" */
    0x77, 0x7b, 0x64, 0x0,

    /* U+0041 "A" */
    0x23, 0x14, 0xe9, 0x0,

    /* U+0042 "B" */
    0xea, 0xea, 0xe0,

    /* U+0043 "C" */
    0x79, 0x89, 0x70,

    /* U+0044 "D" */
    0xe9, 0x99, 0xe0,

    /* U+0045 "E" */
    0xf8, 0xe8, 0xf0,

    /* U+0046 "F" */
    0xf8, 0xe8, 0x80,

    /* U+0047 "G" */
    0x78, 0xb9, 0x70,

    /* U+0048 "H" */
    0x99, 0xf9, 0x90,

    /* U+0049 "I" */
    0xe9, 0x2e,

    /* U+004A "J" */
    0x11, 0x19, 0x60,

    /* U+004B "K" */
    0x9a, 0xca, 0x90,

    /* U+004C "L" */
    0x88, 0x88, 0xf0,

    /* U+004D "M" */
    0x9f, 0xd9, 0x90,

    /* U+004E "N" */
    0x9d, 0xdb, 0x90,

    /* U+004F "O" */
    0x69, 0x99, 0x60,

    /* U+0050 "P" */
    0xea, 0xe8, 0x80,

    /* U+0051 "Q" */
    0x69, 0x99, 0x61,

    /* U+0052 "R" */
    0xea, 0xea, 0xa0,

    /* U+0053 "S" */
    0xe8, 0x69, 0xf0,

    /* U+0054 "T" */
    0xf9, 0x8, 0x42, 0x0,

    /* U+0055 "U" */
    0x99, 0x99, 0x60,

    /* U+0056 "V" */
    0x92, 0x94, 0xc2, 0x0,

    /* U+0057 "W" */
    0xaf, 0xbc, 0xe5, 0x0,

    /* U+0058 "X" */
    0x52, 0x88, 0xa5, 0x0,

    /* U+0059 "Y" */
    0x92, 0x98, 0x42, 0x0,

    /* U+005A "Z" */
    0xf2, 0x48, 0xf0,

    /* U+005B "[" */
    0xea, 0xab,

    /* U+005C "\\" */
    0x91, 0x22, 0x40,

    /* U+005D "]" */
    0xd5, 0x57,

    /* U+005E "^" */
    0x5a, 0x80,

    /* U+005F "_" */
    0xe0,

    /* U+0060 "`" */
    0x80,

    /* U+0061 "a" */
    0xe7, 0x9f,

    /* U+0062 "b" */
    0x8e, 0xaa, 0xe0,

    /* U+0063 "c" */
    0x78, 0x86,

    /* U+0064 "d" */
    0x17, 0x99, 0x70,

    /* U+0065 "e" */
    0x6f, 0x86,

    /* U+0066 "f" */
    0x34, 0xf4, 0x44,

    /* U+0067 "g" */
    0x79, 0x97, 0xf0,

    /* U+0068 "h" */
    0x8f, 0x99, 0x90,

    /* U+0069 "i" */
    0x59, 0x2e,

    /* U+006A "j" */
    0xc, 0x93, 0xc0,

    /* U+006B "k" */
    0x8a, 0xca, 0x90,

    /* U+006C "l" */
    0xc9, 0x2e,

    /* U+006D "m" */
    0xfd, 0x6b, 0x50,

    /* U+006E "n" */
    0xf9, 0x99,

    /* U+006F "o" */
    0x69, 0x96,

    /* U+0070 "p" */
    0xe9, 0x9e, 0x80,

    /* U+0071 "q" */
    0x79, 0x97, 0x10,

    /* U+0072 "r" */
    0xf2, 0x40,

    /* U+0073 "s" */
    0xf1, 0xf0,

    /* U+0074 "t" */
    0x4f, 0x44, 0x70,

    /* U+0075 "u" */
    0x99, 0x9f,

    /* U+0076 "v" */
    0x95, 0x62,

    /* U+0077 "w" */
    0xaf, 0xbc, 0xa0,

    /* U+0078 "x" */
    0x52, 0x25,

    /* U+0079 "y" */
    0x92, 0x98, 0x44, 0x0,

    /* U+007A "z" */
    0xe2, 0x4f,

    /* U+007B "{" */
    0x29, 0x28, 0x91,

    /* U+007C "|" */
    0xfc,

    /* U+007D "}" */
    0x89, 0x22, 0x94,

    /* U+007E "~" */
    0x4d, 0x80
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 77, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 77, .box_w = 1, .box_h = 5, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 2, .adv_w = 77, .box_w = 2, .box_h = 2, .ofs_x = 1, .ofs_y = 3},
    {.bitmap_index = 3, .adv_w = 77, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 7, .adv_w = 77, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 11, .adv_w = 77, .box_w = 5, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 15, .adv_w = 77, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 18, .adv_w = 77, .box_w = 1, .box_h = 2, .ofs_x = 2, .ofs_y = 3},
    {.bitmap_index = 19, .adv_w = 77, .box_w = 2, .box_h = 8, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 21, .adv_w = 77, .box_w = 2, .box_h = 8, .ofs_x = 2, .ofs_y = -2},
    {.bitmap_index = 23, .adv_w = 77, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 26, .adv_w = 77, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 29, .adv_w = 77, .box_w = 1, .box_h = 2, .ofs_x = 2, .ofs_y = -1},
    {.bitmap_index = 30, .adv_w = 77, .box_w = 3, .box_h = 1, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 31, .adv_w = 77, .box_w = 1, .box_h = 1, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 32, .adv_w = 77, .box_w = 3, .box_h = 6, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 35, .adv_w = 77, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 38, .adv_w = 77, .box_w = 2, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 40, .adv_w = 77, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 43, .adv_w = 77, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 46, .adv_w = 77, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 50, .adv_w = 77, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 53, .adv_w = 77, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 56, .adv_w = 77, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 59, .adv_w = 77, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 62, .adv_w = 77, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 65, .adv_w = 77, .box_w = 1, .box_h = 4, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 66, .adv_w = 77, .box_w = 1, .box_h = 6, .ofs_x = 2, .ofs_y = -2},
    {.bitmap_index = 67, .adv_w = 77, .box_w = 4, .box_h = 3, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 69, .adv_w = 77, .box_w = 3, .box_h = 3, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 71, .adv_w = 77, .box_w = 4, .box_h = 3, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 73, .adv_w = 77, .box_w = 3, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 75, .adv_w = 77, .box_w = 5, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 79, .adv_w = 77, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 83, .adv_w = 77, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 86, .adv_w = 77, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 89, .adv_w = 77, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 92, .adv_w = 77, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 95, .adv_w = 77, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 98, .adv_w = 77, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 101, .adv_w = 77, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 104, .adv_w = 77, .box_w = 3, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 106, .adv_w = 77, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 109, .adv_w = 77, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 112, .adv_w = 77, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 115, .adv_w = 77, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 118, .adv_w = 77, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 121, .adv_w = 77, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 124, .adv_w = 77, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 127, .adv_w = 77, .box_w = 4, .box_h = 6, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 130, .adv_w = 77, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 133, .adv_w = 77, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 136, .adv_w = 77, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 140, .adv_w = 77, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 143, .adv_w = 77, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 147, .adv_w = 77, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 151, .adv_w = 77, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 155, .adv_w = 77, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 159, .adv_w = 77, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 162, .adv_w = 77, .box_w = 2, .box_h = 8, .ofs_x = 2, .ofs_y = -1},
    {.bitmap_index = 164, .adv_w = 77, .box_w = 3, .box_h = 6, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 167, .adv_w = 77, .box_w = 2, .box_h = 8, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 169, .adv_w = 77, .box_w = 3, .box_h = 3, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 171, .adv_w = 77, .box_w = 3, .box_h = 1, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 172, .adv_w = 77, .box_w = 1, .box_h = 1, .ofs_x = 2, .ofs_y = 4},
    {.bitmap_index = 173, .adv_w = 77, .box_w = 4, .box_h = 4, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 175, .adv_w = 77, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 178, .adv_w = 77, .box_w = 4, .box_h = 4, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 180, .adv_w = 77, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 183, .adv_w = 77, .box_w = 4, .box_h = 4, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 185, .adv_w = 77, .box_w = 4, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 188, .adv_w = 77, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 191, .adv_w = 77, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 194, .adv_w = 77, .box_w = 3, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 196, .adv_w = 77, .box_w = 3, .box_h = 6, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 199, .adv_w = 77, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 202, .adv_w = 77, .box_w = 3, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 204, .adv_w = 77, .box_w = 5, .box_h = 4, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 207, .adv_w = 77, .box_w = 4, .box_h = 4, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 209, .adv_w = 77, .box_w = 4, .box_h = 4, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 211, .adv_w = 77, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 214, .adv_w = 77, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 217, .adv_w = 77, .box_w = 3, .box_h = 4, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 219, .adv_w = 77, .box_w = 3, .box_h = 4, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 221, .adv_w = 77, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 224, .adv_w = 77, .box_w = 4, .box_h = 4, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 226, .adv_w = 77, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 228, .adv_w = 77, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 231, .adv_w = 77, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 233, .adv_w = 77, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 237, .adv_w = 77, .box_w = 4, .box_h = 4, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 239, .adv_w = 77, .box_w = 3, .box_h = 8, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 242, .adv_w = 77, .box_w = 1, .box_h = 6, .ofs_x = 2, .ofs_y = -1},
    {.bitmap_index = 243, .adv_w = 77, .box_w = 3, .box_h = 8, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 246, .adv_w = 77, .box_w = 5, .box_h = 2, .ofs_x = 0, .ofs_y = 1}
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
const lv_font_t robotomono_08 = {
#else
lv_font_t robotomono_08 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 9,          /*The maximum line height required by the font*/
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



#endif /*#if ROBOTOMONO_08*/

