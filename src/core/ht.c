#include <errno.h>
#include <math.h>
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
#include "core/sleep_mode.h"
#include "driver/beep.h"
#include "driver/dm6302.h"
#include "driver/hardware.h"
#include "driver/rtc6715.h"
#include "driver/screen.h"
#include "ui/page_common.h"
#include "util/math.h"

uint8_t oled_is_sleep = 0;

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

#if defined(HDZGOGGLE) || defined(HDZGOGGLE2)
static const float imu_orientation[3] = {0.0 * DEG_TO_RAD, -90.0 * DEG_TO_RAD, (-90.0 + 23.0) * DEG_TO_RAD};
#elif defined HDZBOXPRO
static const float imu_orientation[3] = {0.0 * DEG_TO_RAD, -90.0 * DEG_TO_RAD, -90.0 * DEG_TO_RAD};
#endif

static const int ppmMaxPulse = 500;
static const int ppmMinPulse = -500;
static const int ppmCenter = 1500;

static pthread_t head_alarm_handle;

static void calculate_orientation();
static void *head_alarm_thread(void *arg);

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
            screen.brightness(0);
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
            screen.brightness(g_setting.image.oled);
            state = OLED_MD_DETECTING;
            cnt = 0;
        }

        cnt++;

        if (cnt == MOTION_DUR_1MINUTE) { // 1-min
            oled_is_sleep = 1;
            LOGI("OLED OFF for protection.");
            beep();
            hw_screen_on(0); // Turn of display

            if (g_hw_stat.source_mode == SOURCE_MODE_HDZERO) {
                HDZero_Close(); // Turn off RF
            }
            if (g_hw_stat.source_mode == SOURCE_MODE_AV) {
                rtc6715.init(0, 0);
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
            if (g_hw_stat.source_mode == SOURCE_MODE_HDZERO) {
                uint8_t ch = g_setting.scan.channel - 1;
                HDZero_open(g_setting.source.hdzero_bw);
                DM6302_SetChannel(g_setting.source.hdzero_band, ch & 0x7F);
            }
            if (g_hw_stat.source_mode == SOURCE_MODE_AV) {
                if (g_setting.source.analog_module == SETTING_SOURCES_ANALOG_MODULE_INTERNAL) {
                    rtc6715.init(1, g_setting.record.audio_source == SETTING_RECORD_AUDIO_SOURCE_AV_IN);
                    rtc6715.set_ch(g_setting.source.analog_channel - 1);
                } else {
                }
            }

            oled_is_sleep = 0;
            LOGI("OLED ON from protection.");

            screen.brightness(g_setting.image.oled);

            hw_screen_on(1);
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
    if (!isSleeping && has_motion_data) {
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

#if defined(HDZGOGGLE) || defined(HDZGOGGLE2)
    ht_data.tiltInverse = 1;
    ht_data.rollInverse = -1;
    ht_data.panInverse = -1;
#elif defined HDZBOXPRO
    ht_data.tiltInverse = -1;
    ht_data.rollInverse = -1;
    ht_data.panInverse = -1;
#endif

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

#ifndef EMULATOR_BUILD
    // start timer (not supported in emulator)
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
#endif
}

void ht_set_maxangle(int angle) {
    ht_data.tiltFactor = 1000.0 / angle;
    ht_data.rollFactor = 1000.0 / angle;
    ht_data.panFactor = 1000.0 / angle;
}

void ht_set_alarm_angle() {
    g_setting.ht.alarm_angle = ht_data.tiltAngle;
    ini_putl("ht", "alarm_angle", g_setting.ht.alarm_angle, SETTING_INI);
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

#if defined(HDZGOGGLE) || defined(HDZGOGGLE2)
    tmp = normalize(ht_data.panAngle, -180.0, 180.0) * ht_data.panInverse * ht_data.panFactor + 0.5;
    ht_data.htChannels[0] = constrain(tmp, ppmMinPulse, ppmMaxPulse) + ppmCenter;
    tmp = normalize(ht_data.tiltAngle, -180.0, 180.0) * ht_data.tiltInverse * ht_data.tiltFactor + 0.5;
    ht_data.htChannels[1] = constrain(tmp, ppmMinPulse, ppmMaxPulse) + ppmCenter;
    tmp = normalize(ht_data.rollAngle, -180.0, 180.0) * ht_data.rollInverse * ht_data.rollFactor + 0.5;
    ht_data.htChannels[2] = constrain(tmp, ppmMinPulse, ppmMaxPulse) + ppmCenter;
#elif defined HDZBOXPRO
    tmp = normalize(ht_data.panAngle, -180.0, 180.0) * ht_data.panInverse * ht_data.panFactor + 0.5;
    ht_data.htChannels[0] = constrain(tmp, ppmMinPulse, ppmMaxPulse) + ppmCenter;
    tmp = normalize(ht_data.tiltAngle, -180.0, 180.0) * ht_data.tiltInverse * ht_data.tiltFactor + 0.5;
    ht_data.htChannels[2] = constrain(tmp, ppmMinPulse, ppmMaxPulse) + ppmCenter;
    tmp = normalize(ht_data.rollAngle, -180.0, 180.0) * ht_data.rollInverse * ht_data.rollFactor + 0.5;
    ht_data.htChannels[1] = constrain(tmp, ppmMinPulse, ppmMaxPulse) + ppmCenter;
#endif
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

void head_alarm_init() {
    pthread_create(&head_alarm_handle, NULL, head_alarm_thread, NULL);
}

void *head_alarm_thread(void *arg) {
    while (1) {
        bool sounding_alarm = false;
        if (ht_data.enable && (g_setting.ht.alarm_state != SETTING_HT_ALARM_STATE_OFF)) {                                                                                                             // user settings
            if ((g_setting.ht.alarm_on_arm && g_setting.ht.alarm_state == SETTING_HT_ALARM_STATE_ARM) || (g_setting.ht.alarm_on_video && g_setting.ht.alarm_state == SETTING_HT_ALARM_STATE_VIDEO)) { // system enabling alarm (when armed or has video signal)

                if (ht_data.tiltAngle < g_setting.ht.alarm_angle) {
                    beep();
                    usleep(100000);
                    beep();
                    for (int i = 0; i < 30; i++) { // delay of 3 seconds split into 30x100ms to allow for a quicker response to the alarm
                        if (ht_data.tiltAngle >= g_setting.ht.alarm_angle) {
                            break;
                        }
                        usleep(100000);
                    }
                }

                usleep(150000); // prevent resource occupation (when armed or video)
            } else {
                usleep(250000); // prevent resource occupation (when !armed and/or !video)
            }
        } else {
            sleep(3); // prevent resource occupation (when ht and/or alarm is disabled)
        }
    }
    pthread_exit(NULL);
}

///////////////////////////////////////////////////////////////////////////////
// Antenna Tracker Functions

float ht_get_pan_angle() {
    return ht_data.panAngle;
}

float ht_get_tilt_angle() {
    return ht_data.tiltAngle;
}

float ht_get_pan_offset() {
    return ht_data.antenna_tracker.pan_offset;
}

void ht_antenna_tracker_update_gps(double latitude, double longitude, float altitude, bool valid) {
    ht_data.gps_data.latitude = latitude;
    ht_data.gps_data.longitude = longitude;
    ht_data.gps_data.altitude = altitude;
    // Once we receive valid GPS data, keep using it indefinitely
    // The drone doesn't teleport - stale GPS is better than no GPS
    if (valid) {
        ht_data.gps_data.valid = true;
        ht_data.gps_data.last_update_time = time(NULL);
    }
    // Note: We don't set valid=false when OSD blanks during arm
    // This allows antenna tracker to work through OSD blanking events
}

bool ht_antenna_tracker_is_gps_valid() {
    // For antenna tracking, we keep the last known GPS position indefinitely
    // This handles Betaflight OSD blanking during arm events and other temporary signal loss
    // The drone's position doesn't change drastically during brief GPS interruptions
    return ht_data.gps_data.valid;
}

void ht_antenna_tracker_calibrate() {
    if (!ht_data.gps_data.valid) {
        LOGW("Cannot calibrate antenna tracker: GPS data not valid");
        return;
    }

    // This is now a simplified manual calibration - just sets user and takeoff to same position
    // For proper calibration, use automatic two-arm method via ht_antenna_tracker_on_arm_event()
    ht_data.antenna_tracker.user_latitude = ht_data.gps_data.latitude;
    ht_data.antenna_tracker.user_longitude = ht_data.gps_data.longitude;
    ht_data.antenna_tracker.user_altitude = ht_data.gps_data.altitude;

    ht_data.antenna_tracker.takeoff_latitude = ht_data.gps_data.latitude;
    ht_data.antenna_tracker.takeoff_longitude = ht_data.gps_data.longitude;
    ht_data.antenna_tracker.takeoff_altitude = ht_data.gps_data.altitude;

    // Store current head tracker angles as offsets
    ht_data.antenna_tracker.pan_offset = ht_data.panAngle;
    ht_data.antenna_tracker.tilt_offset = ht_data.tiltAngle;

    ht_data.antenna_tracker.arm_count = 2; // Mark as calibrated
    ht_data.antenna_tracker.is_calibrated = true;
    ht_data.antenna_tracker.legacy_mode = true; // Single-point mode

    LOGI("Antenna tracker manual calibration (legacy mode): lat=%.6f, lon=%.6f, alt=%.1fm",
         ht_data.antenna_tracker.user_latitude,
         ht_data.antenna_tracker.user_longitude,
         ht_data.antenna_tracker.user_altitude);
}

bool ht_antenna_tracker_is_calibrated() {
    return ht_data.antenna_tracker.is_calibrated;
}

// Calculate great-circle distance between two GPS points in meters
double ht_calculate_distance(double lat1, double lon1, double lat2, double lon2) {
    double lat1_rad = lat1 * DEG_TO_RAD;
    double lon1_rad = lon1 * DEG_TO_RAD;
    double lat2_rad = lat2 * DEG_TO_RAD;
    double lon2_rad = lon2 * DEG_TO_RAD;

    double dlat = lat2_rad - lat1_rad;
    double dlon = lon2_rad - lon1_rad;

    double a = sin(dlat / 2) * sin(dlat / 2) +
               cos(lat1_rad) * cos(lat2_rad) *
                   sin(dlon / 2) * sin(dlon / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));

    return 6371000.0 * c; // Earth radius in meters
}

// Calculate angle at user position between user→point_a and user→point_b
// Returns angle in degrees (-180 to 180)
// Positive = counter-clockwise from reference line, Negative = clockwise
float ht_calculate_angle_at_user(double user_lat, double user_lon,
                                 double point_a_lat, double point_a_lon,
                                 double point_b_lat, double point_b_lon) {
    // Calculate three sides of triangle
    double side_a = ht_calculate_distance(user_lat, user_lon, point_b_lat, point_b_lon);       // user→drone
    double side_b = ht_calculate_distance(user_lat, user_lon, point_a_lat, point_a_lon);       // user→takeoff
    double side_c = ht_calculate_distance(point_a_lat, point_a_lon, point_b_lat, point_b_lon); // takeoff→drone

    // Handle degenerate cases
    if (side_a < 0.1 || side_b < 0.1) {
        return 0.0f; // Too close to calculate angle
    }

    // Law of cosines: cos(angle) = (b² + a² - c²) / (2*b*a)
    double cos_angle = (side_b * side_b + side_a * side_a - side_c * side_c) /
                       (2.0 * side_b * side_a);

    // Clamp to [-1, 1] for numerical stability
    if (cos_angle > 1.0)
        cos_angle = 1.0;
    if (cos_angle < -1.0)
        cos_angle = -1.0;

    double angle = acos(cos_angle) * RAD_TO_DEG;

    // Determine sign using cross product (is drone left or right of reference line?)
    // Calculate bearings for cross product
    double lat1 = user_lat * DEG_TO_RAD;
    double lon1 = user_lon * DEG_TO_RAD;
    double lat_a = point_a_lat * DEG_TO_RAD;
    double lon_a = point_a_lon * DEG_TO_RAD;
    double lat_b = point_b_lat * DEG_TO_RAD;
    double lon_b = point_b_lon * DEG_TO_RAD;

    // Vector from user to takeoff (reference line)
    double dlon_a = lon_a - lon1;
    double y_a = sin(dlon_a) * cos(lat_a);
    double x_a = cos(lat1) * sin(lat_a) - sin(lat1) * cos(lat_a) * cos(dlon_a);

    // Vector from user to drone
    double dlon_b = lon_b - lon1;
    double y_b = sin(dlon_b) * cos(lat_b);
    double x_b = cos(lat1) * sin(lat_b) - sin(lat1) * cos(lat_b) * cos(dlon_b);

    // Cross product z-component (positive = counter-clockwise, negative = clockwise)
    double cross_z = x_a * y_b - y_a * x_b;

    if (cross_z < 0) {
        angle = -angle; // Drone is clockwise from reference
    }

    return (float)angle;
}

// Called when drone arms (detect from OSD or ELRS telemetry)
void ht_antenna_tracker_on_arm_event() {
    if (!ht_data.gps_data.valid) {
        LOGW("Arm event ignored: No valid GPS data");
        return;
    }

    if (ht_data.antenna_tracker.arm_count == 0) {
        // First arm: Save user position
        ht_data.antenna_tracker.user_latitude = ht_data.gps_data.latitude;
        ht_data.antenna_tracker.user_longitude = ht_data.gps_data.longitude;
        ht_data.antenna_tracker.user_altitude = ht_data.gps_data.altitude;
        ht_data.antenna_tracker.arm_count = 1;
        ht_data.antenna_tracker.legacy_mode = false;
        ht_data.antenna_tracker.is_calibrated = false;

        LOGI("Antenna tracker: User position set on first arm");
        LOGI("  Position: lat=%.6f, lon=%.6f, alt=%.1fm",
             ht_data.antenna_tracker.user_latitude,
             ht_data.antenna_tracker.user_longitude,
             ht_data.antenna_tracker.user_altitude);
        LOGI("  Waiting for second arm at takeoff location...");

    } else if (ht_data.antenna_tracker.arm_count == 1) {
        // Second arm: Save takeoff position and calibrate
        ht_data.antenna_tracker.takeoff_latitude = ht_data.gps_data.latitude;
        ht_data.antenna_tracker.takeoff_longitude = ht_data.gps_data.longitude;
        ht_data.antenna_tracker.takeoff_altitude = ht_data.gps_data.altitude;

        // CRITICAL: Set user altitude to 0 because Betaflight resets altitude to 0 at arm
        // When disarmed: GPS altitude is absolute (above sea level)
        // When armed: GPS altitude is relative to takeoff position (0m at ground)
        // Since we calibrate at the second arm (takeoff), all future altitudes will be
        // relative to this point, so user altitude must be 0
        ht_data.antenna_tracker.user_altitude = 0.0f;

        ht_data.antenna_tracker.arm_count = 2;
        ht_data.antenna_tracker.is_calibrated = true;
        ht_data.antenna_tracker.legacy_mode = false;

        // Save current tilt angle BEFORE resetting center position
        // This offset represents how far from horizontal the user was looking during calibration
        ht_data.antenna_tracker.tilt_offset = ht_data.tiltAngle;

        LOGI("Pre-calibration angles: pan=%.1f°, tilt=%.1f°, roll=%.1f°",
             ht_data.panAngle, ht_data.tiltAngle, ht_data.rollAngle);

        // Reset compass center position to align with calibration
        // This sets the current heading/tilt as the home position (0°)
        ht_set_center_position();

        LOGI("Post-calibration angles: pan=%.1f°, tilt=%.1f°, roll=%.1f°",
             ht_data.panAngle, ht_data.tiltAngle, ht_data.rollAngle);

        // Pan offset is now 0 since we reset the center
        ht_data.antenna_tracker.pan_offset = 0.0f;

        double distance = ht_calculate_distance(
            ht_data.antenna_tracker.user_latitude,
            ht_data.antenna_tracker.user_longitude,
            ht_data.antenna_tracker.takeoff_latitude,
            ht_data.antenna_tracker.takeoff_longitude);

        LOGI("Antenna tracker: Fully calibrated on second arm");
        LOGI("  User position: lat=%.6f, lon=%.6f, alt=%.1fm",
             ht_data.antenna_tracker.user_latitude,
             ht_data.antenna_tracker.user_longitude,
             ht_data.antenna_tracker.user_altitude);
        LOGI("  Takeoff position: lat=%.6f, lon=%.6f, alt=%.1fm",
             ht_data.antenna_tracker.takeoff_latitude,
             ht_data.antenna_tracker.takeoff_longitude,
             ht_data.antenna_tracker.takeoff_altitude);
        LOGI("  Distance: %.1fm, Pan offset: %.1f°, Tilt offset: %.1f°",
             distance,
             ht_data.antenna_tracker.pan_offset,
             ht_data.antenna_tracker.tilt_offset);
    } else {
        // 3rd+ arm: Already calibrated, ignore
        LOGD("Antenna tracker: Already calibrated, ignoring arm event");
    }
}

// Reset calibration (called when new session starts)
void ht_antenna_tracker_reset_calibration() {
    ht_data.antenna_tracker.arm_count = 0;
    ht_data.antenna_tracker.is_calibrated = false;
    ht_data.antenna_tracker.legacy_mode = false;
    ht_data.antenna_tracker.pan_offset = 0.0f;
    ht_data.antenna_tracker.tilt_offset = 0.0f;
    // Note: Don't reset GPS data - it may still be valid

    // Reset the armed state detection in OSD thread
    // This ensures the next arm event will be properly detected
    osd_reset_armed_state();

    LOGI("Antenna tracker calibration reset - ready for new calibration");
}

// Get current arm count for UI display
uint8_t ht_antenna_tracker_get_arm_count() {
    return ht_data.antenna_tracker.arm_count;
}

// Test calibration with dummy coordinates for development
void ht_antenna_tracker_test_calibrate() {
    // Simulate two-point calibration for testing

    // Set user position (San Francisco)
    ht_data.antenna_tracker.user_latitude = 37.7749;
    ht_data.antenna_tracker.user_longitude = -122.4194;
    ht_data.antenna_tracker.user_altitude = 0.0f;

    // Set takeoff position (100m north of user)
    ht_data.antenna_tracker.takeoff_latitude = 37.7749 + (100.0 / 111111.0);
    ht_data.antenna_tracker.takeoff_longitude = -122.4194;
    ht_data.antenna_tracker.takeoff_altitude = 0.0f;

    // Store current head tracker angles as offsets
    ht_data.antenna_tracker.pan_offset = ht_data.panAngle;
    ht_data.antenna_tracker.tilt_offset = ht_data.tiltAngle;
    ht_data.antenna_tracker.arm_count = 2;
    ht_data.antenna_tracker.is_calibrated = true;
    ht_data.antenna_tracker.legacy_mode = false;

    // Now set dummy drone position 100m north and 50m up from takeoff
    ht_data.gps_data.latitude = 37.7749 + (200.0 / 111111.0); // 200m north total
    ht_data.gps_data.longitude = -122.4194;
    ht_data.gps_data.altitude = 50.0f;
    ht_data.gps_data.valid = true;

    LOGI("Test calibration: user at %.6f,%.6f, takeoff at %.6f,%.6f, drone at %.6f,%.6f,%.1fm",
         ht_data.antenna_tracker.user_latitude,
         ht_data.antenna_tracker.user_longitude,
         ht_data.antenna_tracker.takeoff_latitude,
         ht_data.antenna_tracker.takeoff_longitude,
         ht_data.gps_data.latitude,
         ht_data.gps_data.longitude,
         ht_data.gps_data.altitude);
}

// Calculate azimuth (bearing) to point at drone using triangle method
// Returns angle in degrees relative to the calibrated heading (where user looked during second arm)
// The direction user was looking during second arm = 0°
float ht_get_drone_azimuth() {
    if (!ht_data.antenna_tracker.is_calibrated || !ht_data.gps_data.valid) {
        return 0.0f; // Return 0 if not calibrated or no GPS
    }

    // Calculate angle at user position between takeoff and drone
    float angle = ht_calculate_angle_at_user(
        ht_data.antenna_tracker.user_latitude,
        ht_data.antenna_tracker.user_longitude,
        ht_data.antenna_tracker.takeoff_latitude,
        ht_data.antenna_tracker.takeoff_longitude,
        ht_data.gps_data.latitude,
        ht_data.gps_data.longitude);

    // Subtract the pan offset to make angles relative to calibration heading
    // When drone is at takeoff, angle should be ~0° (where user was looking during arm)
    float target_angle = angle - ht_data.antenna_tracker.pan_offset;

    return target_angle;
}

// Calculate elevation angle from user to drone
float ht_get_drone_elevation() {
    if (!ht_data.antenna_tracker.is_calibrated || !ht_data.gps_data.valid) {
        return 0.0f; // Return 0 if not calibrated or no GPS
    }

    // Calculate horizontal distance from user to drone
    double horizontal_distance = ht_calculate_distance(
        ht_data.antenna_tracker.user_latitude,
        ht_data.antenna_tracker.user_longitude,
        ht_data.gps_data.latitude,
        ht_data.gps_data.longitude);

    // Calculate vertical distance (altitude difference)
    double vertical_distance = ht_data.gps_data.altitude -
                               ht_data.antenna_tracker.user_altitude;

    // Calculate elevation angle
    double elevation = atan2(vertical_distance, horizontal_distance) * RAD_TO_DEG;

    // Subtract tilt offset from calibration to convert to IMU reference frame
    // tilt_offset is the angle the user's head was at during second arm calibration
    // After calling ht_set_center_position(), that viewing angle became the new 0° reference
    // The geometric elevation (atan2) is relative to true horizontal
    // We subtract the offset to convert from true horizontal to the IMU's shifted coordinate system
    // Example: User looked down -20° during calib (tilt_offset=-20°), this became new 0°
    // If drone is at +10° from true horizontal, in IMU coords: +10° - (-20°) = +30°
    // If drone is at -30° from true horizontal, in IMU coords: -30° - (-20°) = -10°
    double elevation_before_offset = elevation;
    elevation -= ht_data.antenna_tracker.tilt_offset;

    // Detailed logging for debugging elevation calculation
    static int log_counter = 0;
    if (++log_counter % 50 == 0) { // Log every 50th call to avoid spam
        LOGI("Elevation calc: drone_alt=%.1fm, user_alt=%.1fm, vert_dist=%.1fm",
             ht_data.gps_data.altitude,
             ht_data.antenna_tracker.user_altitude,
             vertical_distance);
        LOGI("  horiz_dist=%.1fm, raw_elev=%.1f°, tilt_offset=%.1f°, final_elev=%.1f°",
             horizontal_distance,
             elevation_before_offset,
             ht_data.antenna_tracker.tilt_offset,
             elevation);
        LOGI("  GPS: lat=%.6f, lon=%.6f | User: lat=%.6f, lon=%.6f",
             ht_data.gps_data.latitude,
             ht_data.gps_data.longitude,
             ht_data.antenna_tracker.user_latitude,
             ht_data.antenna_tracker.user_longitude);
    }

    return (float)elevation;
}