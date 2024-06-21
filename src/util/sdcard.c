#include "sdcard.h"

#include <sys/stat.h>
#include <sys/vfs.h>
#include <unistd.h>

static int g_sdcard_free_size = 0;

bool sdcard_mounted() {
    struct stat mountpoint;
    struct stat mountpoint_parent;

    // Fetch mountpoint and mountpoint parent dev_id
    if (stat("/mnt/extsd", &mountpoint) == 0 &&
        stat("/mnt", &mountpoint_parent) == 0) {
        // iff the dev ids _do not_ match there is a filesystem mounted
        return (mountpoint.st_dev != mountpoint_parent.st_dev);
    }

    return false;
}

bool sdcard_inserted() {
    return access(SD_BLOCK_DEVICE, F_OK) == 0;
}

void sdcard_update_free_size() {
    struct statfs info;
    if (statfs("/mnt/extsd", &info) == 0)
        g_sdcard_free_size = (info.f_bsize * info.f_bavail) >> 20; // in MB
    else
        g_sdcard_free_size = 0;
}

bool sdcard_is_full() {
    return g_sdcard_free_size < 103;
}

/*
return in MB
*/
int sdcard_free_size() {
    return g_sdcard_free_size;
}