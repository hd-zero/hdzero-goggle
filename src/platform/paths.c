#include "platform/paths.h"

// On-device defaults. The goggle firmware keeps exactly these (see paths_init).
static char s_app[256] = "/mnt/app";
static char s_extsd[256] = "/mnt/extsd";

#if defined(EMULATOR_BUILD)
#include <stdlib.h>
#include <string.h>

#include <minIni.h>

static void set_root(const char *value, char *dst, size_t n) {
    if (value && *value) {
        strncpy(dst, value, n - 1);
        dst[n - 1] = '\0';
    }
}
#endif

void paths_init(void) {
#if defined(EMULATOR_BUILD)
    // Emulator only: resolve the roots from an optional minIni config file
    // (HDZ_CONFIG) plus env overrides. The goggle build compiles this away, so it
    // does no file I/O and never links minIni for paths — zero cost on-device.
    const char *cfg = getenv("HDZ_CONFIG");
    if (cfg && *cfg) {
        char buf[256];
        if (ini_gets("paths", "app_root", "", buf, sizeof(buf), cfg) > 0)
            set_root(buf, s_app, sizeof(s_app));
        if (ini_gets("paths", "extsd_root", "", buf, sizeof(buf), cfg) > 0)
            set_root(buf, s_extsd, sizeof(s_extsd));
    }
    // Env wins over the file (quick per-run tweaks).
    set_root(getenv("HDZ_APP_ROOT"), s_app, sizeof(s_app));
    set_root(getenv("HDZ_EXTSD_ROOT"), s_extsd, sizeof(s_extsd));
#endif
}

const char *path_app(void) {
    return s_app;
}

const char *path_extsd(void) {
    return s_extsd;
}
