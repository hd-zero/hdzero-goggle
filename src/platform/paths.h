#pragma once
#ifdef __cplusplus
extern "C" {
#endif

// Configurable filesystem roots.
//
// Defaults are the on-device paths, so the goggle firmware build is byte-for-byte
// unchanged (no config present -> "/mnt/app" and "/mnt/extsd"). The desktop
// emulator overrides them so it can load resources/media from anywhere:
//
//   * a minIni config file pointed to by the HDZ_CONFIG env var, section [paths]:
//         [paths]
//         app_root   = /path/to/app     ; replaces /mnt/app
//         extsd_root = /path/to/sdcard  ; replaces /mnt/extsd
//   * or the HDZ_APP_ROOT / HDZ_EXTSD_ROOT env vars (these win over the file),
//     handy for quick per-run tweaks.
//
// Call paths_init() once, early in startup, before anything reads resources.
void paths_init(void);

const char *path_app(void);   // app install root   (default "/mnt/app")
const char *path_extsd(void); // sd-card mount root (default "/mnt/extsd")

#ifdef __cplusplus
}
#endif
