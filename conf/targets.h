#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    TARGET_UNKNOWN = 0,
    TARGET_GOGGLE,
    TARGET_BOXPRO,
} TARGET_TYPES;

#if defined(HDZGOGGLE) || defined(HDZGOGGLE2)
#define TARGET_DPI_DEFAULT  130
#define TARGET_FONT_DEFAULT &lv_font_montserrat_26
#elif defined HDZBOXPRO
#define TARGET_DPI_DEFAULT  70
#define TARGET_FONT_DEFAULT &lv_font_montserrat_16
#else

#error "Platform not defined"

#endif

#ifdef __cplusplus
}
#endif
