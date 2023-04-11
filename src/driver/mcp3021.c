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
#include "i2c.h"

static hw_revision_t g_hw_revision = HW_REV_UNKNOWN;

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

int aduc_read_voltage() {
    int vl, vh;
    double vol;

    vl = I2C_Read(ADDR_FPGA, 0x12);
    vh = I2C_Read(ADDR_FPGA, 0x13);
    //  LOGI("ADUC: %x %x", vh,vl);

    if (g_hw_revision == HW_REV_UNKNOWN) {
        if ((vl == 0) && (vh == 0)) {
            g_hw_revision = HW_REV_1;
        } else {
            g_hw_revision = HW_REV_2;
        }

        LOGI("Voltage ADC is %s", g_hw_revision == HW_REV_1 ? "mcp" : "aduc");

        if (g_hw_revision == HW_REV_1) {
            return mcp_read_voltage();
        }
    }

    vl = (vh << 8) | vl;
    vol = (double)vl * 6.50 + 270;
    return (int)vol; // in mV
}

int read_voltage() {
    int vol;
    if (g_hw_revision == HW_REV_1)
        vol = mcp_read_voltage();
    else
        vol = aduc_read_voltage();
    // LOGI("Voltage = %dmV",vol);
    return vol;
}

hw_revision_t hwRevision() {
    return g_hw_revision;
}
