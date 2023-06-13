#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <log/log.h>
#include <minIni.h>

#include "MadgwickAHRS.h"
#include "common.hh"
#include "elrs.h"
#include "ht.h"
#include "osd.h"

#include "bmi270/accel_gyro.h"
#include "core/settings.h"
#include "driver/dm6302.h"
#include "driver/beep.h"
#include "driver/hardware.h"
#include "driver/oled.h"
#include "ui/page_common.h"
#include "util/math.h"

// #define FAST_SIM
typedef enum {
    OLED_MD_DETECTING, // regular operation, oled on
    OLED_MD_PRE_OFF,   // pre-sleep, reduced brightness
    OLED_MD_OFF        // sleep, oled off
} oled_motion_detect_state_t;

///////////////////////////////////////////////////////////////////////////////
// local
static ht_data_t ht_data;
static const uint8_t frame_period = 10;
static const uint8_t sync_len = 200;

static bool has_motion_data = false;
static bool is_moving = true;

static volatile bool calibrating = false;
static int calibration_count = 0;

static const float imu_orientation[3] = {0.0 * DEG_TO_RAD, -90.0 * DEG_TO_RAD, (-90.0 + 23.0) * DEG_TO_RAD};

static const int ppmMaxPulse = 500;
static const int ppmMinPulse = -500;
static const int ppmCenter = 1500;

static void calculate_orientation();

///////////////////////////////////////////////////////////////////////////////
// no motion to disable OLED display
static void detect_motion(bool is_moving) {
    static oled_motion_detect_state_t state = OLED_MD_DETECTING;
    static int cnt = 0;

    switch (state) {
    case OLED_MD_DETECTING:
        if (g_setting.image.auto_off == 4) {
            // auto_off disabled, bail
            break;
        }
        if (is_moving) {
            // moving, bail
            cnt = 0;
            break;
        }

        cnt++;

#ifdef FAST_SIM
        if (cnt > (MOTION_DUR_1MINUTE * (g_setting.image.auto_off + 1))) {
#else
        if (cnt > (MOTION_DUR_1MINUTE * (g_setting.image.auto_off * 2 + 1))) {
#endif
            LOGI("OLED pre-OFF for protection.");
            OLED_Brightness(0);
            state = OLED_MD_PRE_OFF;
            cnt = 0;
        }
#ifdef FAST_SIM
        LOGI("IDLE %d", cnt);
#endif
        break;

    case OLED_MD_PRE_OFF:
#ifdef FAST_SIM
        LOGI("PRE OFF %d", cnt);
#endif
        if (is_moving) {
            // we got motion, turn oled back on, start over
            OLED_Brightness(g_setting.image.oled);
            state = OLED_MD_DETECTING;
            cnt = 0;
        }

        cnt++;

        if (cnt == MOTION_DUR_1MINUTE) { // 1-min
            LOGI("OLED OFF for protection.");
            beep();

            OLED_ON(0); // Turn off OLED
            if (g_hw_stat.source_mode == HW_SRC_MODE_HDZERO) {
                HDZero_Close(); // Turn off RF
            }

            state = OLED_MD_OFF;
            cnt = 0;
        }
        break;

    case OLED_MD_OFF:
#ifdef FAST_SIM
        LOGI("OFF %d", cnt);
#endif
        if (!is_moving) {
            // no motion, stay off
            cnt = 0;
            break;
        }

        cnt++;

        if (cnt == 2) {
            if (g_hw_stat.source_mode == HW_SRC_MODE_HDZERO) {
                uint8_t ch = g_setting.scan.channel - 1;
                HDZero_open((ch >> 7) & 1);
                DM6302_SetChannel(ch & 0xF);
            }
            LOGI("OLED ON from protection.");
            OLED_Brightness(g_setting.image.oled);
            OLED_ON(1);
            state = OLED_MD_DETECTING;
            cnt = 0;
        }
        break;

    default:
        // wat?
        state = OLED_MD_DETECTING;
        break;
    }
}

void ht_detect_motion() {
    if (has_motion_data) {
        detect_motion(is_moving);
        has_motion_data = false;
    }
}

static void get_imu_data() {
    static int dec_cnt = 0;

#ifndef EMULATOR_BUILD
    get_bmi270(&ht_data.sensor_data);
#endif

    dec_cnt++;
    if (dec_cnt != AHRS_UPDATE_FREQUENCY)
        return; // calibrate dec_cnt to make sure the following code runs at 1Hz
    dec_cnt = 0;

    static struct bmi2_sens_axes_data gyr_last;

    const int16_t dx = ht_data.sensor_data.gyr.x - gyr_last.x;
    const int16_t dy = ht_data.sensor_data.gyr.y - gyr_last.y;
    const int16_t dz = ht_data.sensor_data.gyr.z - gyr_last.z;
    const uint32_t diff = dx * dx + dy * dy + dz * dz;

    is_moving = (diff > MOTION_GYRO_THR) || g_key > 0;
    // LOGD("diff: %d g_key: %d is_moving: %d", diff, g_key, is_moving);

    g_key = 0;
    gyr_last = ht_data.sensor_data.gyr;

    has_motion_data = true;
}

static void timer_callback_imu(union sigval timer_data) {
    get_imu_data();
    calculate_orientation();
}

/////////////////////////////////////////////////////////////////////////////////
// HT function
void ht_init() {
    ht_data.tiltAngle = 0;
    ht_data.rollAngle = 0;
    ht_data.panAngle = 0;

    ht_data.tiltInverse = 1;
    ht_data.rollInverse = -1;
    ht_data.panInverse = -1;

    ht_data.htChannels[0] = 0;
    ht_data.htChannels[1] = 0;
    ht_data.htChannels[2] = 0;

    ht_data.enable = 0;
    ht_set_maxangle(g_setting.ht.max_angle);
    ht_data.acc_offset[0] = g_setting.ht.acc_x;
    ht_data.acc_offset[1] = g_setting.ht.acc_y;
    ht_data.acc_offset[2] = g_setting.ht.acc_z;
    ht_data.gyr_offset[0] = g_setting.ht.gyr_x;
    ht_data.gyr_offset[1] = g_setting.ht.gyr_y;
    ht_data.gyr_offset[2] = g_setting.ht.gyr_z;

    // start timer
    timer_t timerId = 0;
    struct sigevent sev = {0};
    struct itimerspec its = {.it_value.tv_sec = 1,
                             .it_value.tv_nsec = 0,
                             .it_interval.tv_sec = 0,
                             .it_interval.tv_nsec = 1000000000 / AHRS_UPDATE_FREQUENCY};

    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_notify_function = &timer_callback_imu;

    int res = timer_create(CLOCK_REALTIME, &sev, &timerId);
    if (res != 0) {
        LOGE("Error timer_create: %s\n", strerror(errno));
        return;
    }

    res = timer_settime(timerId, 0, &its, NULL);
    if (res != 0) {
        LOGE("Error timer_settime: %s\n", strerror(errno));
    }
}

void ht_set_maxangle(int angle) {
    ht_data.tiltFactor = 1000.0 / angle;
    ht_data.rollFactor = 1000.0 / angle;
    ht_data.panFactor = 1000.0 / angle;
}

static void calc_gyr(float *gyrAngle) // in degree
{
    // convert gyro readings to degrees/sec (with calibration offsets)
    gyrAngle[0] = gyr_to_dps(ht_data.sensor_data.gyr.x - ht_data.gyr_offset[0]);
    gyrAngle[1] = gyr_to_dps(ht_data.sensor_data.gyr.y - ht_data.gyr_offset[1]);
    gyrAngle[2] = gyr_to_dps(ht_data.sensor_data.gyr.z - ht_data.gyr_offset[2]);
    rotate(gyrAngle, imu_orientation);
}

static void calc_acc(float *accAngle) // in G
{
    // convert accelerometer readings to G forces
    accAngle[0] = acc_to_g(ht_data.sensor_data.acc.x);
    accAngle[1] = acc_to_g(ht_data.sensor_data.acc.y);
    accAngle[2] = acc_to_g(ht_data.sensor_data.acc.z);
    rotate(accAngle, imu_orientation);
}

void ht_calibrate() {
    LOGI("HT calibration...");
    ht_data.acc_offset[0] = ht_data.acc_offset[1] = ht_data.acc_offset[2] = 0;
    ht_data.gyr_offset[0] = ht_data.gyr_offset[1] = ht_data.gyr_offset[2] = 0;

    calibration_count = 0;
    calibrating = true;
    // Check if finished calibrating
    while (calibrating) {
        usleep(100000);
    }
    ht_data.acc_offset[0] >>= CALIBRATION_BCNT;
    ht_data.acc_offset[1] >>= CALIBRATION_BCNT;
    ht_data.acc_offset[2] >>= CALIBRATION_BCNT;
    ht_data.gyr_offset[0] >>= CALIBRATION_BCNT;
    ht_data.gyr_offset[1] >>= CALIBRATION_BCNT;
    ht_data.gyr_offset[2] >>= CALIBRATION_BCNT;

    ini_putl("ht", "acc_x", ht_data.acc_offset[0], SETTING_INI);
    ini_putl("ht", "acc_y", ht_data.acc_offset[1], SETTING_INI);
    ini_putl("ht", "acc_z", ht_data.acc_offset[2], SETTING_INI);
    ini_putl("ht", "gyr_x", ht_data.gyr_offset[0], SETTING_INI);
    ini_putl("ht", "gyr_y", ht_data.gyr_offset[1], SETTING_INI);
    ini_putl("ht", "gyr_z", ht_data.gyr_offset[2], SETTING_INI);

    LOGI("done!");
}

static void calculate_orientation() {
    float gyrAngle[3], accAngle[3];
    float tmp;

    if (!calibrating && !ht_data.enable)
        return;

    if (calibrating) {
        ht_data.acc_offset[0] += ht_data.sensor_data.acc.x;
        ht_data.acc_offset[1] += ht_data.sensor_data.acc.y;
        ht_data.acc_offset[2] += ht_data.sensor_data.acc.z;
        ht_data.gyr_offset[0] += ht_data.sensor_data.gyr.x;
        ht_data.gyr_offset[1] += ht_data.sensor_data.gyr.y;
        ht_data.gyr_offset[2] += ht_data.sensor_data.gyr.z;
        calibration_count++;
        if (calibration_count == 1 << CALIBRATION_BCNT)
            calibrating = false;
    }

    calc_gyr(gyrAngle);
    calc_acc(accAngle);
    // LOGI("ACC=%.2f,%.2f,%.2f\tGYR=%.2f,%.2f,%.2f", accAngle[0], accAngle[1], accAngle[2],
    //     gyrAngle[0], gyrAngle[1], gyrAngle[2]);

    MadgwickAHRSupdateIMU(gyrAngle[0] * DEG_TO_RAD, gyrAngle[1] * DEG_TO_RAD, gyrAngle[2] * DEG_TO_RAD,
                          accAngle[0], accAngle[1], accAngle[2]);

    // Adjust PTR relatice to user specified home position
    ht_data.panAngle = getYaw() - ht_data.panAngleHome;
    ht_data.tiltAngle = getPitch() - ht_data.tiltAngleHome;
    ht_data.rollAngle = getRoll() - ht_data.rollAngleHome;

    tmp = normalize(ht_data.panAngle, -180.0, 180.0) * ht_data.panInverse * ht_data.panFactor + 0.5;
    ht_data.htChannels[0] = constrain(tmp, ppmMinPulse, ppmMaxPulse) + ppmCenter;

    tmp = normalize(ht_data.tiltAngle, -180.0, 180.0) * ht_data.tiltInverse * ht_data.tiltFactor + 0.5;
    ht_data.htChannels[1] = constrain(tmp, ppmMinPulse, ppmMaxPulse) + ppmCenter;

    tmp = normalize(ht_data.rollAngle, -180.0, 180.0) * ht_data.rollInverse * ht_data.rollFactor + 0.5;
    ht_data.htChannels[2] = constrain(tmp, ppmMinPulse, ppmMaxPulse) + ppmCenter;

    Set_HT_dat(ht_data.htChannels[0], ht_data.htChannels[1], ht_data.htChannels[2]);

    if (elrs_headtracking_enabled()) {
        uint16_t ptrCRSF[3];
        ptrCRSF[0] = fmap(ht_data.htChannels[0], ppmMinPulse + ppmCenter, ppmMaxPulse + ppmCenter, 191.0, 1792.0) + 0.5;
        ptrCRSF[1] = fmap(ht_data.htChannels[1], ppmMinPulse + ppmCenter, ppmMaxPulse + ppmCenter, 191.0, 1792.0) + 0.5;
        ptrCRSF[2] = fmap(ht_data.htChannels[2], ppmMinPulse + ppmCenter, ppmMaxPulse + ppmCenter, 191.0, 1792.0) + 0.5;
        msp_ht_update(ptrCRSF[0], ptrCRSF[1], ptrCRSF[2]);
    }
}

void ht_set_center_position() {
    ht_data.panAngleHome += ht_data.panAngle;
    ht_data.tiltAngleHome += ht_data.tiltAngle;
    ht_data.rollAngleHome += ht_data.rollAngle;
}

void ht_enable() {
    ht_data.enable = 1;
    Set_HT_status(ht_data.enable, frame_period, sync_len);
}

void ht_disable() {
    ht_data.enable = 0;
    Set_HT_status(ht_data.enable, frame_period, sync_len);
}

int16_t *ht_get_channels() {
    return ht_data.htChannels;
}
