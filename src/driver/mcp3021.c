#include "mcp3021.h"

#include <fcntl.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <log/log.h>

#include "core/common.hh"

int mcp_read_voltage(void) {
    char buf[128];
    FILE *fp;
    int i;

    fp = fopen("/sys/bus/iio/devices/iio:device0/in_voltage0_raw", "r");
    if (!fp) {
        static bool bFirst = true;
        if (bFirst) {
            LOGE("mcp_read_voltage: open error.");
            bFirst = false;
        }
        return -1;
    }

    if (fgets(buf, 60, fp) != NULL)
        i = atoi(buf);
    else
        i = -1;
    fclose(fp);
    return i;
}
