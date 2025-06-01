#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#if HDZGOGGLE

#define TARGET_DPI_DEFAULT  130
#define TARGET_FONT_DEFAULT &lv_font_montserrat_26

#elif HDZBOXPRO

#define TARGET_DPI_DEFAULT  70
#define TARGET_FONT_DEFAULT &lv_font_montserrat_16

#else

#error "Platform not defined"

#endif

#ifdef __cplusplus
}
#endif
