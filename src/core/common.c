#include "common.hh"

#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "defines.h"
#include "self_test.h"
#include "ui/page_common.h"

///////////////////////////////////////////////////////////////////////////////
// globals
pthread_mutex_t lvgl_mutex;
atomic_int g_key = 0;
atomic_int g_init_done = 0; // 0= init not done, 1= done, -1= dial/up/down pressed
static hw_revision_t g_hw_revsion = HW_REV_UNKNOWN;
///////////////////////////////////////////////////////////////////////////////

void setHwRevision(hw_revision_t revision) {
    if (revision > g_hw_revsion) {
        g_hw_revsion = revision;
    }
}

hw_revision_t getHwRevision() {
    return g_hw_revsion;
}
