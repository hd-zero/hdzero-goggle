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

#ifndef CONTHRAX_16
#define CONTHRAX_16 1
#endif

#if CONTHRAX_16

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */
    0x0,

    /* U+0021 "!" */
    0xff, 0xff, 0xf,

    /* U+0022 "\"" */
    0xcf, 0x3c, 0xf3, 0xcc,

    /* U+0023 "#" */
    0x30, 0xc3, 0xc, 0xff, 0xff, 0xff, 0x30, 0xc3,
    0xc, 0x30, 0xcf, 0xff, 0xff, 0xf3, 0xc, 0x30,
    0xc0,

    /* U+0024 "$" */
    0x6, 0xf, 0xff, 0xff, 0xe3, 0xc, 0x61, 0xff,
    0x9f, 0xf8, 0x33, 0x6, 0x7f, 0xff, 0xff, 0x3,
    0x0, 0x60,

    /* U+0025 "%" */
    0x7c, 0x8, 0x7f, 0x8, 0x31, 0x8c, 0x18, 0xcc,
    0xf, 0xef, 0xf3, 0xef, 0xfc, 0x5, 0x86, 0x4,
    0xc3, 0x6, 0x61, 0x86, 0x3f, 0xc6, 0xf, 0xc0,

    /* U+0026 "&" */
    0x3f, 0x81, 0xfc, 0xc, 0x60, 0x63, 0x3, 0xb8,
    0xf, 0x1d, 0xf9, 0xdc, 0x7c, 0xc1, 0xc6, 0x1f,
    0x3f, 0xdc, 0xfc, 0x70,

    /* U+0027 "'" */
    0xff, 0xc0,

    /* U+0028 "(" */
    0x3b, 0xf9, 0x8c, 0x63, 0x18, 0xc6, 0x31, 0x8c,
    0x63, 0x18, 0x63, 0xce,

    /* U+0029 ")" */
    0xe7, 0x8c, 0x31, 0x8c, 0x63, 0x18, 0xc6, 0x31,
    0x8c, 0x63, 0x37, 0xb8,

    /* U+002A "*" */
    0x8, 0x4, 0x2, 0x1d, 0x71, 0xc0, 0xa0, 0xd8,
    0x44, 0x22, 0x0,

    /* U+002B "+" */
    0x18, 0x18, 0x18, 0xff, 0xff, 0x18, 0x18, 0x18,

    /* U+002C "," */
    0xff, 0x80,

    /* U+002D "-" */
    0xff, 0xc0,

    /* U+002E "." */
    0xf0,

    /* U+002F "/" */
    0x4, 0x18, 0x30, 0x60, 0x83, 0x6, 0x8, 0x30,
    0x60, 0x83, 0x6, 0x8, 0x30, 0x60,

    /* U+0030 "0" */
    0x3f, 0xc7, 0xfe, 0xc0, 0x3c, 0x3, 0xc0, 0x3c,
    0x3, 0xc0, 0x3c, 0x3, 0xc0, 0x37, 0xfe, 0x3f,
    0xc0,

    /* U+0031 "1" */
    0xff, 0x33, 0x33, 0x33, 0x33, 0x30,

    /* U+0032 "2" */
    0xff, 0xef, 0xff, 0x0, 0x30, 0x3, 0x7f, 0xff,
    0xfe, 0xc0, 0xc, 0x0, 0xc0, 0xf, 0xff, 0xff,
    0xf0,

    /* U+0033 "3" */
    0xff, 0xef, 0xff, 0x0, 0x30, 0x3, 0xff, 0xff,
    0xff, 0x0, 0x30, 0x3, 0x0, 0x3f, 0xff, 0xff,
    0xe0,

    /* U+0034 "4" */
    0x0, 0xf0, 0x7, 0xc0, 0x7b, 0x3, 0xcc, 0x3c,
    0x31, 0xe0, 0xc6, 0x3, 0x3f, 0xff, 0x7f, 0xfc,
    0x0, 0xc0, 0x3, 0x0,

    /* U+0035 "5" */
    0xff, 0xff, 0xff, 0xc0, 0xc, 0x0, 0xff, 0xef,
    0xff, 0x0, 0x30, 0x3, 0x0, 0x3f, 0xff, 0xff,
    0xe0,

    /* U+0036 "6" */
    0x3f, 0xf7, 0xff, 0xc0, 0xc, 0x0, 0xff, 0xef,
    0xff, 0xc0, 0x3c, 0x3, 0xc0, 0x37, 0xff, 0x3f,
    0xe0,

    /* U+0037 "7" */
    0xff, 0xf7, 0xff, 0xc0, 0xc, 0x0, 0xe0, 0xe,
    0x0, 0xe0, 0xe, 0x0, 0xe0, 0xe, 0x0, 0xe0,
    0xe, 0x0,

    /* U+0038 "8" */
    0x7f, 0xef, 0xff, 0xc0, 0x3c, 0x3, 0xff, 0xff,
    0xff, 0xc0, 0x3c, 0x3, 0xc0, 0x3f, 0xff, 0x7f,
    0xe0,

    /* U+0039 "9" */
    0x7f, 0xcf, 0xfe, 0xc0, 0x3c, 0x3, 0xff, 0xf7,
    0xff, 0x0, 0x30, 0x3, 0x0, 0x7f, 0xfe, 0xff,
    0xc0,

    /* U+003A ":" */
    0xf0, 0x3c,

    /* U+003B ";" */
    0xf0, 0x3f, 0xe0,

    /* U+003C "<" */
    0x1, 0x87, 0x8f, 0x1c, 0xc, 0x7, 0xc0, 0x78,
    0x7, 0x0, 0x0,

    /* U+003D "=" */
    0xff, 0xff, 0x0, 0x0, 0xff, 0xff,

    /* U+003E ">" */
    0xc0, 0x78, 0xf, 0x80, 0xe0, 0x38, 0x79, 0xf1,
    0xc0, 0x0, 0x0,

    /* U+003F "?" */
    0xff, 0x7f, 0xc0, 0x60, 0x30, 0x38, 0x78, 0x70,
    0x30, 0x0, 0x0, 0x6, 0x3, 0x0,

    /* U+0040 "@" */
    0x7f, 0xef, 0xff, 0xc0, 0x3c, 0x3, 0xc7, 0xfc,
    0xff, 0xcc, 0x3c, 0xc3, 0xcf, 0xfc, 0x7e, 0xc0,
    0xe, 0x0, 0x7f, 0xf3, 0xff,

    /* U+0041 "A" */
    0x3, 0x80, 0xf, 0x80, 0x1b, 0x80, 0x73, 0x0,
    0xc7, 0x3, 0x86, 0x6, 0xe, 0x1f, 0xfe, 0x7f,
    0xfc, 0xe0, 0x1f, 0x80, 0x38,

    /* U+0042 "B" */
    0xff, 0xf7, 0xff, 0xf0, 0x7, 0x80, 0x3f, 0xff,
    0xff, 0xff, 0x0, 0x78, 0x3, 0xc0, 0x1f, 0xff,
    0xff, 0xfc,

    /* U+0043 "C" */
    0x3f, 0xf7, 0xff, 0xc0, 0xc, 0x0, 0xc0, 0xc,
    0x0, 0xc0, 0xc, 0x0, 0xc0, 0x7, 0xff, 0x3f,
    0xf0,

    /* U+0044 "D" */
    0xff, 0xe7, 0xff, 0xb0, 0xf, 0x80, 0x3c, 0x1,
    0xe0, 0xf, 0x0, 0x78, 0x3, 0xc0, 0x3f, 0xff,
    0xbf, 0xf8,

    /* U+0045 "E" */
    0x3f, 0xf7, 0xff, 0xc0, 0xc, 0x0, 0xff, 0xff,
    0xff, 0xc0, 0xc, 0x0, 0xe0, 0x7, 0xff, 0x3f,
    0xf0,

    /* U+0046 "F" */
    0x3f, 0xf7, 0xff, 0xc0, 0xc, 0x0, 0xff, 0xff,
    0xff, 0xc0, 0xc, 0x0, 0xc0, 0xc, 0x0, 0xc0,
    0x0,

    /* U+0047 "G" */
    0x3f, 0xfb, 0xff, 0xf0, 0x1, 0x80, 0xc, 0x7f,
    0xe3, 0xff, 0x0, 0x78, 0x3, 0xc0, 0x1b, 0xff,
    0xcf, 0xfc,

    /* U+0048 "H" */
    0xc0, 0x3c, 0x3, 0xc0, 0x3c, 0x3, 0xff, 0xff,
    0xff, 0xc0, 0x3c, 0x3, 0xc0, 0x3c, 0x3, 0xc0,
    0x30,

    /* U+0049 "I" */
    0xff, 0xff, 0xfc,

    /* U+004A "J" */
    0x0, 0x60, 0xc, 0x1, 0x80, 0x30, 0x6, 0x0,
    0xc0, 0x18, 0x3, 0x0, 0x7f, 0xfb, 0xfe, 0x0,

    /* U+004B "K" */
    0xc0, 0xec, 0xc, 0xc1, 0xcc, 0x38, 0xff, 0xf,
    0xf0, 0xc3, 0x8c, 0x1c, 0xc1, 0xcc, 0xe, 0xc0,
    0x70,

    /* U+004C "L" */
    0xc0, 0xc, 0x0, 0xc0, 0xc, 0x0, 0xc0, 0xc,
    0x0, 0xc0, 0xc, 0x0, 0xc0, 0x7, 0xff, 0x3f,
    0xf0,

    /* U+004D "M" */
    0xf0, 0x7, 0xfc, 0x7, 0xf6, 0x3, 0x7b, 0x83,
    0xbc, 0xc1, 0x9e, 0x71, 0xcf, 0x18, 0xc7, 0x8e,
    0xe3, 0xc3, 0x71, 0xe1, 0xf0, 0xf0, 0x70, 0x60,

    /* U+004E "N" */
    0xf0, 0x1f, 0xc0, 0xf7, 0x7, 0xbc, 0x3c, 0xe1,
    0xe3, 0x8f, 0xe, 0x78, 0x7b, 0xc1, 0xde, 0x7,
    0xf0, 0x1e,

    /* U+004F "O" */
    0x3f, 0xf1, 0xff, 0xec, 0x1, 0xf0, 0x3, 0xc0,
    0xf, 0x0, 0x3c, 0x0, 0xf0, 0x3, 0xc0, 0x1d,
    0xff, 0xe3, 0xff, 0x0,

    /* U+0050 "P" */
    0xff, 0xef, 0xff, 0xc0, 0x3c, 0x3, 0xc0, 0x3f,
    0xff, 0xff, 0xec, 0x0, 0xc0, 0xc, 0x0, 0xc0,
    0x0,

    /* U+0051 "Q" */
    0x3f, 0xf1, 0xff, 0xec, 0x1, 0xf0, 0x3, 0xc0,
    0xf, 0x0, 0x3c, 0x0, 0xf0, 0x3, 0xc0, 0x1d,
    0xff, 0xe3, 0xff, 0x80, 0x1c, 0x0, 0x70, 0x0,
    0xe0,

    /* U+0052 "R" */
    0xff, 0xe7, 0xff, 0xb0, 0xd, 0x80, 0x6c, 0x3,
    0x7f, 0xfb, 0xff, 0x98, 0x1c, 0xc0, 0xe6, 0x3,
    0xb0, 0xe,

    /* U+0053 "S" */
    0x7f, 0xff, 0xff, 0xc0, 0xc, 0x0, 0xff, 0xe7,
    0xff, 0x0, 0x30, 0x3, 0x0, 0x3f, 0xff, 0xff,
    0xe0,

    /* U+0054 "T" */
    0xff, 0xff, 0xff, 0xc0, 0xc0, 0x6, 0x0, 0x30,
    0x1, 0x80, 0xc, 0x0, 0x60, 0x3, 0x0, 0x18,
    0x0, 0xc0,

    /* U+0055 "U" */
    0xc0, 0x1e, 0x0, 0xf0, 0x7, 0x80, 0x3c, 0x1,
    0xe0, 0xf, 0x0, 0x78, 0x3, 0xc0, 0x1b, 0xff,
    0x8f, 0xf8,

    /* U+0056 "V" */
    0xe0, 0xe, 0xe0, 0x1d, 0xc0, 0x71, 0xc0, 0xe1,
    0x83, 0x83, 0x86, 0x3, 0x1c, 0x7, 0x30, 0x6,
    0xe0, 0xf, 0x80, 0xf, 0x0,

    /* U+0057 "W" */
    0xe0, 0x70, 0x33, 0x7, 0xc1, 0x9c, 0x36, 0x1c,
    0xe1, 0xb0, 0xc3, 0x19, 0xc6, 0x18, 0xc6, 0x70,
    0xe6, 0x33, 0x3, 0x61, 0xd8, 0x1b, 0x6, 0xc0,
    0xf8, 0x3e, 0x3, 0x81, 0xe0,

    /* U+0058 "X" */
    0x70, 0x1c, 0xe0, 0xe1, 0xc7, 0x3, 0xb8, 0x7,
    0xc0, 0x1e, 0x0, 0xfc, 0x7, 0x38, 0x38, 0x71,
    0xc0, 0xee, 0x1, 0xc0,

    /* U+0059 "Y" */
    0xe0, 0x1d, 0xc0, 0xe3, 0x87, 0x6, 0x18, 0xc,
    0xc0, 0x3f, 0x0, 0x78, 0x0, 0xc0, 0x3, 0x0,
    0xc, 0x0, 0x30, 0x0,

    /* U+005A "Z" */
    0xff, 0xff, 0xff, 0x0, 0x70, 0x1f, 0x7, 0xc1,
    0xf8, 0x3e, 0xf, 0x80, 0xe0, 0xf, 0xff, 0xff,
    0xf0,

    /* U+005B "[" */
    0xff, 0xf1, 0x8c, 0x63, 0x18, 0xc6, 0x31, 0x8c,
    0x63, 0x18, 0xc7, 0xfe,

    /* U+005C "\\" */
    0xc1, 0x81, 0x3, 0x6, 0x4, 0xc, 0x18, 0x10,
    0x30, 0x60, 0x40, 0xc1, 0x81, 0x2,

    /* U+005D "]" */
    0xff, 0xc6, 0x31, 0x8c, 0x63, 0x18, 0xc6, 0x31,
    0x8c, 0x63, 0x1f, 0xfe,

    /* U+005E "^" */
    0x18, 0x3c, 0x3c, 0x24, 0x66, 0x66, 0xc2, 0xc3,
    0x83,

    /* U+005F "_" */
    0xff, 0xff, 0xf0,

    /* U+0060 "`" */
    0xe6, 0x63,

    /* U+0061 "a" */
    0xff, 0xdf, 0xfc, 0x1, 0xbf, 0xff, 0xff, 0x80,
    0xf0, 0x1f, 0xff, 0x7f, 0xe0,

    /* U+0062 "b" */
    0xc0, 0xc, 0x0, 0xc0, 0xf, 0xfe, 0xff, 0xfc,
    0x3, 0xc0, 0x3c, 0x3, 0xc0, 0x3c, 0x3, 0xff,
    0xf7, 0xfe,

    /* U+0063 "c" */
    0x7f, 0xff, 0xff, 0x0, 0x60, 0xc, 0x1, 0x80,
    0x30, 0x7, 0xff, 0x7f, 0xe0,

    /* U+0064 "d" */
    0x0, 0x30, 0x3, 0x0, 0x37, 0xff, 0xff, 0xfc,
    0x3, 0xc0, 0x3c, 0x3, 0xc0, 0x3c, 0x3, 0xff,
    0xf7, 0xfe,

    /* U+0065 "e" */
    0x7f, 0xdf, 0xff, 0x1, 0xff, 0xff, 0xff, 0x80,
    0x30, 0x7, 0xff, 0x7f, 0xe0,

    /* U+0066 "f" */
    0x1e, 0x7c, 0xc7, 0xff, 0xe6, 0xc, 0x18, 0x30,
    0x60, 0xc1, 0x80,

    /* U+0067 "g" */
    0x7f, 0xef, 0xff, 0xc0, 0x3c, 0x3, 0xc0, 0x3c,
    0x3, 0xc0, 0x3f, 0xff, 0x7f, 0xf0, 0x3, 0x0,
    0x3f, 0xff, 0xff, 0xe0,

    /* U+0068 "h" */
    0xc0, 0xc, 0x0, 0xc0, 0xf, 0xfe, 0xff, 0xfc,
    0x3, 0xc0, 0x3c, 0x3, 0xc0, 0x3c, 0x3, 0xc0,
    0x3c, 0x3,

    /* U+0069 "i" */
    0xf3, 0xff, 0xff,

    /* U+006A "j" */
    0x18, 0xc0, 0x31, 0x8c, 0x63, 0x18, 0xc6, 0x31,
    0x8f, 0xfe,

    /* U+006B "k" */
    0xc0, 0x18, 0x3, 0x0, 0x60, 0x7c, 0x1d, 0x87,
    0x3f, 0xc7, 0xf8, 0xc3, 0x98, 0x3b, 0x3, 0x60,
    0x70,

    /* U+006C "l" */
    0xff, 0xff, 0xff,

    /* U+006D "m" */
    0x7f, 0xfe, 0xff, 0xff, 0xc1, 0x83, 0xc1, 0x83,
    0xc1, 0x83, 0xc1, 0x83, 0xc1, 0x83, 0xc1, 0x83,
    0xc1, 0x83,

    /* U+006E "n" */
    0x7f, 0xef, 0xff, 0xc0, 0x3c, 0x3, 0xc0, 0x3c,
    0x3, 0xc0, 0x3c, 0x3, 0xc0, 0x30,

    /* U+006F "o" */
    0x7f, 0xef, 0xff, 0xc0, 0x3c, 0x3, 0xc0, 0x3c,
    0x3, 0xc0, 0x3f, 0xff, 0x7f, 0xe0,

    /* U+0070 "p" */
    0x7f, 0xef, 0xff, 0xc0, 0x3c, 0x3, 0xc0, 0x3c,
    0x3, 0xc0, 0x3f, 0xff, 0xff, 0xec, 0x0, 0xc0,
    0xc, 0x0, 0xc0, 0x0,

    /* U+0071 "q" */
    0x7f, 0xef, 0xff, 0xc0, 0x3c, 0x3, 0xc0, 0x3c,
    0x3, 0xc0, 0x3f, 0xff, 0x7f, 0xf0, 0x3, 0x0,
    0x30, 0x3, 0x0, 0x30,

    /* U+0072 "r" */
    0x7f, 0xff, 0xf0, 0x18, 0xc, 0x6, 0x3, 0x1,
    0x80, 0xc0, 0x0,

    /* U+0073 "s" */
    0x7f, 0xff, 0xff, 0x0, 0x7f, 0xe7, 0xfe, 0x0,
    0xc0, 0x1f, 0xff, 0xff, 0xc0,

    /* U+0074 "t" */
    0x30, 0x63, 0xff, 0xf3, 0x6, 0xc, 0x18, 0x30,
    0x7c, 0x78,

    /* U+0075 "u" */
    0xc0, 0x3c, 0x3, 0xc0, 0x3c, 0x3, 0xc0, 0x3c,
    0x3, 0xc0, 0x3f, 0xff, 0x7f, 0xe0,

    /* U+0076 "v" */
    0xe0, 0x3b, 0x1, 0x9c, 0x1c, 0x70, 0xc1, 0x8e,
    0xe, 0x60, 0x37, 0x1, 0xf0, 0x7, 0x0,

    /* U+0077 "w" */
    0xe1, 0xe0, 0xd8, 0x7c, 0x36, 0x1b, 0x1d, 0xce,
    0xc6, 0x33, 0x39, 0x8c, 0xc6, 0xe3, 0xf1, 0xb0,
    0x78, 0x7c, 0x1e, 0xf, 0x0,

    /* U+0078 "x" */
    0x70, 0x73, 0x8e, 0x1d, 0xc0, 0xf8, 0x7, 0x0,
    0xf8, 0x19, 0xc7, 0xe, 0xe0, 0x70,

    /* U+0079 "y" */
    0x60, 0x1b, 0x81, 0xcc, 0x1c, 0x70, 0xc1, 0x8e,
    0xe, 0x60, 0x3f, 0x1, 0xf0, 0x7, 0x80, 0x18,
    0x1, 0xc0, 0x3c, 0x1, 0xc0, 0x0,

    /* U+007A "z" */
    0xff, 0xdf, 0xfc, 0x3, 0x3, 0xc1, 0xe0, 0xf0,
    0x38, 0x7, 0xff, 0xff, 0xe0,

    /* U+007B "{" */
    0x1c, 0xf3, 0xc, 0x30, 0xc3, 0xc, 0x33, 0xcf,
    0xc, 0x30, 0xc3, 0xc, 0x30, 0xf1, 0xc0,

    /* U+007C "|" */
    0xff, 0xff, 0xff, 0xff,

    /* U+007D "}" */
    0xf1, 0xf0, 0x60, 0xc1, 0x83, 0x6, 0xc, 0x18,
    0x3c, 0x78, 0xc1, 0x83, 0x6, 0xc, 0x19, 0xf3,
    0xc0,

    /* U+007E "~" */
    0xf1, 0xff, 0x8f
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 86, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 68, .box_w = 2, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 4, .adv_w = 126, .box_w = 6, .box_h = 5, .ofs_x = 1, .ofs_y = 7},
    {.bitmap_index = 8, .adv_w = 218, .box_w = 12, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 25, .adv_w = 209, .box_w = 11, .box_h = 13, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 43, .adv_w = 292, .box_w = 17, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 67, .adv_w = 224, .box_w = 13, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 87, .adv_w = 69, .box_w = 2, .box_h = 5, .ofs_x = 1, .ofs_y = 7},
    {.bitmap_index = 89, .adv_w = 116, .box_w = 5, .box_h = 19, .ofs_x = 2, .ofs_y = -4},
    {.bitmap_index = 101, .adv_w = 116, .box_w = 5, .box_h = 19, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 113, .adv_w = 149, .box_w = 9, .box_h = 9, .ofs_x = 0, .ofs_y = 3},
    {.bitmap_index = 124, .adv_w = 157, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 132, .adv_w = 68, .box_w = 2, .box_h = 5, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 134, .adv_w = 98, .box_w = 5, .box_h = 2, .ofs_x = 1, .ofs_y = 5},
    {.bitmap_index = 136, .adv_w = 68, .box_w = 2, .box_h = 2, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 137, .adv_w = 106, .box_w = 7, .box_h = 16, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 151, .adv_w = 232, .box_w = 12, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 168, .adv_w = 87, .box_w = 4, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 174, .adv_w = 221, .box_w = 12, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 191, .adv_w = 216, .box_w = 12, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 208, .adv_w = 232, .box_w = 14, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 228, .adv_w = 221, .box_w = 12, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 245, .adv_w = 222, .box_w = 12, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 262, .adv_w = 212, .box_w = 13, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 280, .adv_w = 231, .box_w = 12, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 297, .adv_w = 222, .box_w = 12, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 314, .adv_w = 68, .box_w = 2, .box_h = 7, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 316, .adv_w = 68, .box_w = 2, .box_h = 10, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 319, .adv_w = 157, .box_w = 9, .box_h = 9, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 330, .adv_w = 157, .box_w = 8, .box_h = 6, .ofs_x = 1, .ofs_y = 3},
    {.bitmap_index = 336, .adv_w = 157, .box_w = 9, .box_h = 9, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 347, .adv_w = 162, .box_w = 9, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 361, .adv_w = 224, .box_w = 12, .box_h = 14, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 382, .adv_w = 250, .box_w = 15, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 403, .adv_w = 235, .box_w = 13, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 421, .adv_w = 218, .box_w = 12, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 438, .adv_w = 238, .box_w = 13, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 456, .adv_w = 218, .box_w = 12, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 473, .adv_w = 214, .box_w = 12, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 490, .adv_w = 237, .box_w = 13, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 508, .adv_w = 232, .box_w = 12, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 525, .adv_w = 73, .box_w = 2, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 528, .adv_w = 200, .box_w = 11, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 544, .adv_w = 219, .box_w = 12, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 561, .adv_w = 205, .box_w = 12, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 578, .adv_w = 306, .box_w = 17, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 602, .adv_w = 250, .box_w = 13, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 620, .adv_w = 252, .box_w = 14, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 640, .adv_w = 226, .box_w = 12, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 657, .adv_w = 252, .box_w = 14, .box_h = 14, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 682, .adv_w = 232, .box_w = 13, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 700, .adv_w = 219, .box_w = 12, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 717, .adv_w = 223, .box_w = 13, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 735, .adv_w = 250, .box_w = 13, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 753, .adv_w = 250, .box_w = 15, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 774, .adv_w = 334, .box_w = 21, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 803, .adv_w = 228, .box_w = 14, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 823, .adv_w = 229, .box_w = 14, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 843, .adv_w = 219, .box_w = 12, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 860, .adv_w = 116, .box_w = 5, .box_h = 19, .ofs_x = 2, .ofs_y = -4},
    {.bitmap_index = 872, .adv_w = 106, .box_w = 7, .box_h = 16, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 886, .adv_w = 116, .box_w = 5, .box_h = 19, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 898, .adv_w = 157, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 3},
    {.bitmap_index = 907, .adv_w = 164, .box_w = 10, .box_h = 2, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 910, .adv_w = 101, .box_w = 4, .box_h = 4, .ofs_x = 1, .ofs_y = 10},
    {.bitmap_index = 912, .adv_w = 202, .box_w = 11, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 925, .adv_w = 223, .box_w = 12, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 943, .adv_w = 187, .box_w = 11, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 956, .adv_w = 223, .box_w = 12, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 974, .adv_w = 202, .box_w = 11, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 987, .adv_w = 125, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 998, .adv_w = 223, .box_w = 12, .box_h = 13, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 1018, .adv_w = 225, .box_w = 12, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1036, .adv_w = 69, .box_w = 2, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1039, .adv_w = 72, .box_w = 5, .box_h = 16, .ofs_x = -2, .ofs_y = -4},
    {.bitmap_index = 1049, .adv_w = 203, .box_w = 11, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1066, .adv_w = 69, .box_w = 2, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1069, .adv_w = 283, .box_w = 16, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1087, .adv_w = 226, .box_w = 12, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1101, .adv_w = 222, .box_w = 12, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1115, .adv_w = 223, .box_w = 12, .box_h = 13, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 1135, .adv_w = 223, .box_w = 12, .box_h = 13, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 1155, .adv_w = 157, .box_w = 9, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1166, .adv_w = 200, .box_w = 11, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1179, .adv_w = 125, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1189, .adv_w = 224, .box_w = 12, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1203, .adv_w = 212, .box_w = 13, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1218, .adv_w = 298, .box_w = 18, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1239, .adv_w = 202, .box_w = 12, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1253, .adv_w = 207, .box_w = 13, .box_h = 13, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 1275, .adv_w = 188, .box_w = 11, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1288, .adv_w = 116, .box_w = 6, .box_h = 19, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 1303, .adv_w = 108, .box_w = 2, .box_h = 16, .ofs_x = 3, .ofs_y = -2},
    {.bitmap_index = 1307, .adv_w = 116, .box_w = 7, .box_h = 19, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 1324, .adv_w = 157, .box_w = 8, .box_h = 3, .ofs_x = 1, .ofs_y = 5}
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
    0, 0, -15, 0, 0, 0, 0, -10,
    -5, 0, -5, 0, 0, -31, -5, 0,
    -26, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -8, 0, 0, -8, -9,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -8, -20, -13, 0, -28,
    0, 0, -5, 0, 0, 0, 0, 0,
    -9, -8, 0, 0, -8, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 28, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -15, 0, 0,
    0, 0, -5, -8, -5, 0, -15, -5,
    0, 0, 0, -5, 0, 0, -5, 0,
    0, -26, -26, -26, 0, -31, 0, 0,
    0, -18, 13, 0, 0, -29, -5, -31,
    0, 0, -31, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -20, 0, 0,
    0, -20, 0, 0, -10, 0, 0, 0,
    -26, -20, -10, -26, -26, 0, 0, 0,
    3, 0, 0, 0, 0, 0, -8, 0,
    -14, -8, 0, 0, -18, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -38, -10, -10, -31, 0, -5, -10, 0,
    0, 0, 0, 0, 0, -10, -15, 0,
    0, -15, -15, 0, -10, 0, 0, 0,
    0, -10, 0, 0, -5, 0, 0, 0,
    -3, 0, 0, -3, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -5, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -8, 0, 0,
    0, 0, 0, 0, 0, -9, -7, -7,
    -8, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -3, 0, 0, -18,
    0, 0, 0, 0, 0, -7, 0, 0,
    -10, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -6, 0, 0, 0, -5, 0, 0, -5,
    0, -7, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -8, 0, -8,
    0, 0, 0, -5, 0, 0, -5, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -28, 0,
    0, -3, 0, -13, -4, 0, -3, -2,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -5, 0, -5, 0, 0,
    0, 0, 0, 0, -3, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -5, 0, -8, 0, 0, 0,
    0, 0, 0, -5, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -31, -3, 0, -20, 0, 0, 0,
    0, 0, 0, 0, 0, -26, 0, -10,
    0, 0, -10, 0, 0, -33, -49, -28,
    0, -44, 0, -5, -8, -13, 10, 0,
    0, -24, -8, -30, 0, 0, -26, 0,
    0, 0, 0, -10, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -5,
    0, -5, 0, -10, 0, 0, 0, 0,
    0, 0, 0, 0, -10, 0, 0, 0,
    0, 0, 0, -5, -10, -5, -8, -13,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -5, 0, 0, 0, -5,
    0, 5, -18, 0, 0, 0, 0, 0,
    0, 0, 0, -17, 0, 0, -20, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -8, 0, 0, 0, 0, 0, 0,
    0, 0, -5, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -5, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -5, 0,
    -10, 0, 0, 0, 0, 0, 0, 0,
    0, -10, 0, 0, -4, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -5, 0, 0, 0, -5, 0, -26, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -8, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -5, -10, -10,
    0, 0, 0, -13, -10, -16, 0, 0,
    -16, -2, 0, 0, 0, -15, 0, 0,
    0, 0, 0, 0, 0, 0, -20, 0,
    0, 0, 0, -4, 0, 0, -49, -46,
    -21, 0, -54, 0, 0, -3, 0, 0,
    0, 0, -18, -3, -28, -11, 0, -28,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -3, 0, 0,
    0, 0, 0, 0, 0, 0, -3, -7,
    0, -8, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -5, -18, 3, -23, 0, 0, 0,
    0, 0, 0, 0, -3, -26, 0, 0,
    -16, 0, 0, 0, 0, 0, -5, -14,
    -7, 0, 0, 0, 0, 0, -3, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -5, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -3, 0, -5, 0, 0,
    -2, 0, -3, -3, -13, -7, 0, -14,
    -5, -4, -8, 0, 0, 0, 0, 0,
    -7, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -10, 0, 0, 0, 0, 0,
    0, 0, 0, -5, 0, 0, 0, 0,
    0, 0, 0, -2, -3, -7, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -3, 0, -5, -5, 0, 0, -15, -26,
    -26, -36, 0, 0, 0, 0, 0, 0,
    0, 0, -33, -5, -5, -46, 3, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -5, 0, 0, -5, -5, 0, 0, 0,
    0, 0, 0, 0, 0, -20, -26, -20,
    -33, 0, 0, 0, 0, 0, 0, 0,
    0, -49, -10, -13, -39, 0, -3, -8,
    0, 0, 0, 0, 0, 0, -9, -24,
    0, 0, -24, -20, 0, -13, 0, 0,
    -8, 0, 0, 0, -18, -26, -10, -23,
    0, 0, 0, 0, 0, 0, 0, 0,
    -28, -5, -13, -26, 0, -8, -8, 0,
    0, 0, 0, 0, 0, -8, -16, 0,
    0, -16, -15, 0, -10, 0, 0, -5,
    0, 0, 0, -15, -31, -26, -36, 0,
    0, 0, 0, 0, 0, 0, 0, -44,
    -13, -17, -53, 0, -6, -8, 0, 0,
    0, 0, 0, 0, -18, -28, 0, 0,
    -28, -26, -8, -15, -5, 0, -8, -5,
    -8, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -5,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -5, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -10, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -8, -5, 0, -7, 0, -8,
    0, 0, 0, -10, 0, 0, 0, 0,
    0, 0, 0, -15, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -4,
    0, -7, -5, -5, -7, 0, -5, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -10, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -4, 0, 0, -4, 0, 8, -8, -31,
    5, -10, 0, 0, 0, 0, 0, 0,
    0, 8, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 5, 0, 0, 0, 5, 0, 0,
    0, 0, 0, 0, -8, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -15, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 19, 0, 0, 0, 0, -7, 0,
    0, -7, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    18, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -3, 0, -14, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -1, -5, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -10, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 5, 0, 0,
    0, 5, 0, 0, 0, 0, 0, 0,
    -6, 0, -5, -4, -8, 0, 0, 0,
    0, 0, 0, 0, -5, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -3, 0, 0, -3, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 5, 0, 0, 0, 5,
    0, 0, 0, 0, 0, 0, 0, 0,
    -5, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -18, -31,
    -8, -23, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -1,
    -14, 0, 0, -11, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -13, 0, 0,
    -13, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -3, -4,
    0, 0, -4, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -5,
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
const lv_font_t conthrax_16 = {
#else
lv_font_t conthrax_16 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 19,          /*The maximum line height required by the font*/
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



#endif /*#if CONTHRAX_16*/

