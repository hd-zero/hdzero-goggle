#include "sdcard.h"

#include <sys/stat.h>

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
