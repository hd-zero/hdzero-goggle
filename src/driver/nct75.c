#include "nct75.h"

#include <fcntl.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "../core/common.hh"
#include "mcp3021.h"
#include <log/log.h>

sys_temperature_t g_temperature;

int nct_read_temperature(nct_type_t type) {
    char buf[128];
    FILE *fp;
    int i;

    if (getHwRevision() == HW_REV_UNKNOWN) {
        return -1;
    }

    int dev_id = type + (getHwRevision() == HW_REV_1 ? 1 : 0);

    sprintf(buf, "/sys/bus/iio/devices/iio:device%d/in_voltage0_raw", dev_id);
    fp = fopen(buf, "r");
    if (!fp) {
        static bool bFirst = true;
        char *sName[3] = {"Right", "Top", "Left"};
        if (bFirst) {
            LOGI("nct_read_temperature: [%s] side error.", sName[type]);
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
