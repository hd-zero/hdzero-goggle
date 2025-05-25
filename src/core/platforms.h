#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#if HDZGOGGLE

#define DISPLAY_DPI  130
#define DISPLAY_FONT &lv_font_montserrat_26

#elif HDZBOXPRO

#define DISPLAY_DPI  70
#define DISPLAY_FONT &lv_font_montserrat_16

#else

#error "Platform not defined"

#endif

#ifdef __cplusplus
}
#endif
