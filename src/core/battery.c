#include "battery.h"

#include "core/settings.h"
#include "driver/mcp3021.h"
#include "ui/page_common.h"

sys_battery_t g_battery;

static int battery_detect_type() {
    int v = mcp_read_voltage();
    return (v * 10 / 1000 / 42 + 1);
}

void battery_init() {
    switch (g_setting.power.cell_count_mode) {
    default:
    case SETTING_POWER_CELL_COUNT_MODE_AUTO:
        g_battery.type = battery_detect_type();
        break;
    case SETTING_POWER_CELL_COUNT_MODE_MANUAL:
        g_battery.type = g_setting.power.cell_count;
        break;
    }
}

void battery_update() {
    g_battery.voltage = mcp_read_voltage();
}

bool battery_is_low() {
    if (g_battery.type == 0) {
        return true;
    }
    int cell_volt = g_battery.voltage / g_battery.type;
    return cell_volt <= g_setting.power.voltage * 100;
}

int battery_get_millivolts(bool per_cell) {
    if (per_cell && g_battery.type > 0) {
        return g_battery.voltage / g_battery.type;
    }
    return g_battery.voltage;
}
