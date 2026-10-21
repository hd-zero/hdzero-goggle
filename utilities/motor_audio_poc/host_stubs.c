#include <stdint.h>
#include <stdio.h>

/* Minimal stubs so the real msp_displayport.c can link on host. */

void load_fc_osd_font(uint8_t fhd) {
    (void)fhd;
}

void osd_signal_update(void) {}

/* log stubs used if any LOG* path is compiled in */
void log_print(int level, const char *tag, const char *fmt, ...) {
    (void)level;
    (void)tag;
    (void)fmt;
}
