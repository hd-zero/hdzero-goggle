#include "page_wifi.h"

#include <arpa/inet.h>
#include <errno.h>
#include <linux/if.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>

#include <log/log.h>
#include <minIni.h>

#include "../conf/ui.h"

#include "core/app_state.h"
#include "core/common.hh"
#include "core/dvr.h"
#include "lang/language.h"
#include "ui/page_common.h"
#include "ui/ui_attribute.h"
#include "ui/ui_keyboard.h"
#include "ui/ui_style.h"
#include "util/filesystem.h"
#include "util/system.h"

/**
 * Types
 */
typedef struct {
    btn_group_t button;
    bool dirty;
} button_t;

typedef struct {
    lv_obj_t *label;
    lv_obj_t *input;
    char text[WIFI_MODE_COUNT][WIFI_SSID_MAX];
    bool dirty;
} ssid_t;

typedef struct {
    lv_obj_t *label;
    lv_obj_t *input;
    lv_obj_t *status;
    char text[WIFI_MODE_COUNT][WIFI_PASSWD_MAX];
    bool dirty;
} passwd_t;

typedef struct {
    lv_obj_t *label;
    lv_obj_t *input;
    lv_obj_t *status;
    char text[WIFI_PASSWD_MAX];
    bool dirty;
} root_pw_t;

typedef struct {
    slider_group_t input;
    bool active;
    bool dirty;
} rf_channel_t;

typedef struct {
    lv_obj_t *label;
    lv_obj_t *input;
    lv_obj_t *status;
    char text[16];
    bool dirty;
} network_t;

typedef struct {
    button_t enable;
    button_t mode;
    ssid_t ssid;
    passwd_t passwd;
    lv_obj_t *apply_settings;
    lv_obj_t *back;
    lv_obj_t *note;
    int row_count;
} page_1_t;

typedef struct {
    button_t dhcp;
    network_t ip_addr;
    network_t netmask;
    network_t gateway;
    network_t dns;
    rf_channel_t rf_channel;
    lv_obj_t *apply_settings;
    int row_count;
} page_2_t;

typedef struct {
    root_pw_t root_pw;
    button_t ssh;
    lv_obj_t *apply_settings;
    lv_obj_t *note;
    int row_count;
} page_3_t;

typedef struct {
    int item_select;
    button_t page_select;
    page_1_t page_1;
    page_2_t page_2;
    page_3_t page_3;
    int confirm_settings;
    bool dirty;
} page_options_t;

/**
 *  Constants
 */
static char INVALID_TOO_SHORT_STR[64];
static char INVALID_FORMAT_STR[64];

/**
 *  Globals
 */
static lv_coord_t col_dsc[] = {UI_WIFI_COLS};
static lv_coord_t row_dsc[] = {UI_WIFI_ROWS};
static page_options_t page_wifi = {0};
static lv_timer_t *page_wifi_apply_settings_timer = NULL;
static lv_timer_t *page_wifi_apply_settings_pending_timer = NULL;
static bool page_wifi_bootup_pending = true;

/**
 * Refresh WiFi service configuration parameters.
 */
static void page_wifi_update_services() {
    FILE *fp = NULL;

    if (!page_wifi_bootup_pending) {
        /**
         *  Host <-> Client switching
         *  must wipe previous settings.
         */
        unlink(WIFI_AP_ON);
        unlink(WIFI_STA_ON);
        unlink(WIFI_AP_CFG);
        unlink(WIFI_DHCP_CFG);
        unlink(WIFI_STA_CFG);
        unlink(WIFI_DNS_CFG);
        unlink(ROOT_PW_SET);
    }

    if ((fp = fopen(WIFI_AP_ON, "w"))) {
        fprintf(fp, "#!/bin/sh\n");
        fprintf(fp, "insmod /mnt/app/ko/xradio_mac.ko\n");
        fprintf(fp, "insmod /mnt/app/ko/xradio_core.ko\n");
        fprintf(fp, "insmod /mnt/app/ko/xradio_wlan.ko\n");
        fprintf(fp, "ifconfig wlan0 %s netmask %s up\n", g_setting.wifi.ip_addr, g_setting.wifi.netmask);
        fprintf(fp, "hostapd /tmp/hostapd.conf&\n");
        fprintf(fp, "udhcpd /tmp/udhcpd.conf&\n");
        fprintf(fp, "route add default gw %s\n", g_setting.wifi.gateway);
        fprintf(fp, "/mnt/app/app/record/rtspLive > /tmp/rtspLive.log 2>&1 &\n");
        fclose(fp);
        system_exec("chmod +x " WIFI_AP_ON);
    }

    if ((fp = fopen(WIFI_STA_ON, "w"))) {
        fprintf(fp, "#!/bin/sh\n");
        fprintf(fp, "insmod /mnt/app/ko/xradio_mac.ko\n");
        fprintf(fp, "insmod /mnt/app/ko/xradio_core.ko\n");
        fprintf(fp, "insmod /mnt/app/ko/xradio_wlan.ko\n");
        fprintf(fp, "ifconfig wlan0 up\n");

        if (g_setting.wifi.dhcp) {
            fprintf(fp,
                    "udhcpc -x hostname:%s -x 0x3d:%s -r %s -i wlan0 -b&\n",
                    g_setting.wifi.ssid[WIFI_MODE_AP],
                    g_setting.wifi.clientid,
                    g_setting.wifi.ip_addr);
        }

        fprintf(fp, "mkdir /var/log\n");
        fprintf(fp, "wpa_supplicant -Dnl80211 -iwlan0 -c/tmp/wpa_supplicant.conf&\n");

        if (!g_setting.wifi.dhcp) {
            fprintf(fp, "ifconfig wlan0 %s\n", g_setting.wifi.ip_addr);
            fprintf(fp, "route add default gw %s\n", g_setting.wifi.gateway);
        }

        fprintf(fp, "/mnt/app/app/record/rtspLive > /tmp/rtspLive.log 2>&1 &\n");

        fclose(fp);
        system_exec("chmod +x " WIFI_STA_ON);
    }

    if ((fp = fopen(WIFI_AP_CFG, "w"))) {
        fprintf(fp, "interface=wlan0\n");
        fprintf(fp, "driver=nl80211\n");
        fprintf(fp, "ssid=%s\n", g_setting.wifi.ssid[WIFI_MODE_AP]);
        fprintf(fp, "channel=%d\n", g_setting.wifi.rf_channel);
        fprintf(fp, "hw_mode=g\n");
        fprintf(fp, "ieee80211n=1\n");
        fprintf(fp, "wmm_enabled=1\n");
        fprintf(fp, "ignore_broadcast_ssid=0\n");
        fprintf(fp, "auth_algs=1\n");
        fprintf(fp, "wpa=3\n");
        fprintf(fp, "wpa_passphrase=%s\n", g_setting.wifi.passwd[WIFI_MODE_AP]);
        fprintf(fp, "wpa_key_mgmt=WPA-PSK\n");
        fprintf(fp, "wpa_pairwise=TKIP\n");
        fprintf(fp, "rsn_pairwise=CCMP\n");
        fclose(fp);
    }

    if ((fp = fopen(WIFI_DHCP_CFG, "w"))) {
        int ip[4];
        sscanf(g_setting.wifi.ip_addr, "%d.%d.%d.%d", &ip[3], &ip[2], &ip[1], &ip[0]);
        fprintf(fp, "start\t%d.%d.%d.100\n", ip[3], ip[2], ip[1]);
        fprintf(fp, "end\t%d.%d.%d.254\n", ip[3], ip[2], ip[1]);
        fprintf(fp, "interface\twlan0\n");
        fprintf(fp, "opt\tdns\t0.0.0.0\n");
        fprintf(fp, "option\tsubnet\t%s\n", g_setting.wifi.netmask);
        fprintf(fp, "opt\trouter\t%s\n", g_setting.wifi.gateway);
        fprintf(fp, "opt\twins\t0.0.0.0\n");
        fprintf(fp, "option\tdomain\tlocal\n");
        fprintf(fp, "option\tlease\t864000\n");
        fclose(fp);
    }

    if ((fp = fopen(WIFI_STA_CFG, "w"))) {
        fprintf(fp, "ctrl_interface=/var/log/wpa_supplicant\n");
        fprintf(fp, "update_config=1\n");
        fprintf(fp, "network={\n");
        fprintf(fp, "ssid=\"%s\"\n", g_setting.wifi.ssid[WIFI_MODE_STA]);
        fprintf(fp, "psk=\"%s\"\n", g_setting.wifi.passwd[WIFI_MODE_STA]);
        fprintf(fp, "}\n");
        fclose(fp);
    }

    if (g_setting.wifi.mode == WIFI_MODE_STA &&
        !g_setting.wifi.dhcp &&
        (fp = fopen(WIFI_DNS_CFG, "w"))) {
        fprintf(fp, "nameserver %s\n", g_setting.wifi.dns);
        fprintf(fp, "options wlan0 trust-ad\n");
        fclose(fp);
        system_exec("ln -snf /tmp/resolve.conf /etc/resolve.conf");
    }

    if ((fp = fopen(ROOT_PW_SET, "w"))) {
        fprintf(fp, "#!/bin/sh\n");
        fprintf(fp, "passwd << EOF\n");
        fprintf(fp, "%s\n", g_setting.wifi.root_pw);
        fprintf(fp, "%s\n", g_setting.wifi.root_pw);
        fprintf(fp, "EOF\n");
        fclose(fp);
        system_exec("chmod +x " ROOT_PW_SET "; " ROOT_PW_SET);
    }
}

/**
 * Generate a unqiue Client ID based on a hex seed.
 */
static void page_wifi_generate_clientid(char *buffer, int size) {
    static const char *seed = "0123456789ABCDEF";
    for (int i = 0; i < size; i++) {
        buffer[i] = seed[rand() % 16];
    }
    buffer[size - 1] = '\0';
}

/**
 * Hide password text from being viewed when browsing menu.
 */
static void page_wifi_mask_password(lv_obj_t *obj, int size) {
    static char buffer[128];
    for (int i = 0; i < size; ++i) {
        buffer[i] = '*';
    }
    buffer[size] = 0;

    lv_label_set_text(obj, buffer);
}

/**
 * Update settings and apply them.
 *
 *  Note: This function will be invoked asynchronously post bootup and may
 *        require additional APP_STATE checks to ensure integrity of execution.
 */
static void page_wifi_update_settings() {
    g_setting.wifi.enable = btn_group_get_sel(&page_wifi.page_1.enable.button) == 0;
    g_setting.wifi.mode = btn_group_get_sel(&page_wifi.page_1.mode.button);
    g_setting.wifi.dhcp = btn_group_get_sel(&page_wifi.page_2.dhcp.button) == 0;
    g_setting.wifi.ssh = btn_group_get_sel(&page_wifi.page_3.ssh.button) == 0;

    snprintf(g_setting.wifi.ssid[g_setting.wifi.mode], WIFI_SSID_MAX, "%s", page_wifi.page_1.ssid.text[g_setting.wifi.mode]);
    snprintf(g_setting.wifi.passwd[g_setting.wifi.mode], WIFI_PASSWD_MAX, "%s", page_wifi.page_1.passwd.text[g_setting.wifi.mode]);

    snprintf(g_setting.wifi.ip_addr, WIFI_NETWORK_MAX, "%s", page_wifi.page_2.ip_addr.text);
    snprintf(g_setting.wifi.netmask, WIFI_NETWORK_MAX, "%s", page_wifi.page_2.netmask.text);
    snprintf(g_setting.wifi.gateway, WIFI_NETWORK_MAX, "%s", page_wifi.page_2.gateway.text);
    snprintf(g_setting.wifi.dns, WIFI_NETWORK_MAX, "%s", page_wifi.page_2.dns.text);

    snprintf(g_setting.wifi.root_pw, WIFI_PASSWD_MAX, "%s", page_wifi.page_3.root_pw.text);

    if (0 == strlen(g_setting.wifi.clientid)) {
        page_wifi_generate_clientid(g_setting.wifi.clientid, WIFI_CLIENTID_MAX);
        ini_puts("wifi", "clientid", g_setting.wifi.clientid, SETTING_INI);
    }

    settings_put_bool("wifi", "enable", g_setting.wifi.enable);
    ini_putl("wifi", "mode", g_setting.wifi.mode, SETTING_INI);
    ini_puts("wifi", "ap_ssid", g_setting.wifi.ssid[WIFI_MODE_AP], SETTING_INI);
    ini_puts("wifi", "ap_passwd", g_setting.wifi.passwd[WIFI_MODE_AP], SETTING_INI);
    ini_puts("wifi", "sta_ssid", g_setting.wifi.ssid[WIFI_MODE_STA], SETTING_INI);
    ini_puts("wifi", "sta_passwd", g_setting.wifi.passwd[WIFI_MODE_STA], SETTING_INI);
    settings_put_bool("wifi", "dhcp", g_setting.wifi.dhcp);
    ini_puts("wifi", "ip_addr", g_setting.wifi.ip_addr, SETTING_INI);
    ini_puts("wifi", "netmask", g_setting.wifi.netmask, SETTING_INI);
    ini_puts("wifi", "gateway", g_setting.wifi.gateway, SETTING_INI);
    ini_puts("wifi", "dns", g_setting.wifi.dns, SETTING_INI);
    ini_putl("wifi", "rf_channel", g_setting.wifi.rf_channel, SETTING_INI);
    ini_puts("wifi", "root_pw", g_setting.wifi.root_pw, SETTING_INI);
    settings_put_bool("wifi", "ssh", g_setting.wifi.ssh);

    // Prepare WiFi interfaces
    if (!page_wifi_bootup_pending) {
        system_script(WIFI_OFF);
    }
    page_wifi_update_services();

    // Activate WiFi services
    if (g_setting.wifi.enable) {
        if (g_app_state == APP_STATE_MAINMENU) {
#if defined(HDZGOGGLE) || defined(HDZGOGGLE2)
            dvr_update_vi_conf(VR_1080P30);
#elif defined(HDZBOXPRO)
            dvr_update_vi_conf(VR_720P60);
#endif
        }
        if (WIFI_MODE_AP == g_setting.wifi.mode) {
            system_script(WIFI_AP_ON);
        } else {
            system_script(WIFI_STA_ON);
        }

        if (g_setting.wifi.ssh) {
            system_exec("dropbear");
        }
    }
}

/**
 * Acquire the actual address in use.
 */
static const char *page_wifi_get_real_address() {
    const char *address = NULL;
    int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);

    if (fd >= 0) {
        struct ifreq ifr;
        strcpy(ifr.ifr_name, "wlan0");

        // Try to derive the real ip address
        if (0 == ioctl(fd, SIOCGIFADDR, &ifr)) {
            address = inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
        }

        close(fd);
    }

    return address;
}

/**
 * Updates the all notes on every page.
 */
static void page_wifi_update_page_1_notes() {
    const char *address = page_wifi_get_real_address();

    if (btn_group_get_sel(&page_wifi.page_1.mode.button) == WIFI_MODE_STA &&
        btn_group_get_sel(&page_wifi.page_2.dhcp.button) == 0 &&
        address == NULL) {
        address = "x.x.x.x";
    }

    static char buf[1024];
    snprintf(buf, sizeof(buf), "%s:\n    %s,%s.\n%s:\n    1. %s.\n    2. %s:\n        rtsp://%s:8554/hdzero\n",
             _lang("Password Requirements"),
             _lang("Minimum 8 characters"),
             _lang("maximum 64 characters"),
             _lang("Live stream"),
             _lang("Connect to the WiFi network identified above"),
             _lang("Use VLC Player to open a Network Stream"),
             address ? address : page_wifi.page_2.ip_addr.text);
    lv_label_set_text(page_wifi.page_1.note, buf);
}

static void page_wifi_update_page_3_notes() {
    static char buf[256];
    snprintf(buf, sizeof(buf), "%s:\n    %s,%s.\n",
             _lang("Password Requirements"),
             _lang("Minimum 8 characters"),
             _lang("maximum 64 characters"));
    lv_label_set_text(page_wifi.page_3.note, buf);
}

/**
 * Acquire all settings and refresh UI text elements.
 */
static void page_wifi_sync_settings() {
    for (int i = 0; i < WIFI_MODE_COUNT; ++i) {
        snprintf(page_wifi.page_1.ssid.text[i], sizeof(page_wifi.page_1.ssid.text[i]), "%s", g_setting.wifi.ssid[i]);
        snprintf(page_wifi.page_1.passwd.text[i], sizeof(page_wifi.page_1.passwd.text[i]), "%s", g_setting.wifi.passwd[i]);
    }

    snprintf(page_wifi.page_2.ip_addr.text, sizeof(page_wifi.page_2.ip_addr.text), "%s", g_setting.wifi.ip_addr);
    snprintf(page_wifi.page_2.netmask.text, sizeof(page_wifi.page_2.netmask.text), "%s", g_setting.wifi.netmask);
    snprintf(page_wifi.page_2.gateway.text, sizeof(page_wifi.page_2.gateway.text), "%s", g_setting.wifi.gateway);
    snprintf(page_wifi.page_2.dns.text, sizeof(page_wifi.page_2.dns.text), "%s", g_setting.wifi.dns);

    snprintf(page_wifi.page_3.root_pw.text, sizeof(page_wifi.page_3.root_pw.text), "%s", g_setting.wifi.root_pw);
}

/**
 * Returns the max scrollable page elements for selected page.
 */
static int page_wifi_get_current_page_max() {
    switch (btn_group_get_sel(&page_wifi.page_select.button)) {
    default:
    case 0:
        return page_wifi.page_1.row_count;
    case 1:
        return page_wifi.page_2.row_count;
    case 2:
        return page_wifi.page_3.row_count;
    }
}

/**
 * Update UI to reflect current wifi page options.
 */
static void page_wifi_update_current_page(int which) {
    for (size_t i = 0; i < MAX_PANELS; i++) {
        lv_obj_add_flag(pp_wifi.p_arr.panel[i], FLAG_SELECTABLE);
    }

    // Page 1
    btn_group_show(&page_wifi.page_1.enable.button, false);
    btn_group_show(&page_wifi.page_1.mode.button, false);
    lv_obj_add_flag(page_wifi.page_1.ssid.label, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(page_wifi.page_1.ssid.input, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(page_wifi.page_1.passwd.label, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(page_wifi.page_1.passwd.input, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(page_wifi.page_1.passwd.status, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(page_wifi.page_1.apply_settings, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(page_wifi.page_1.back, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(page_wifi.page_1.note, LV_OBJ_FLAG_HIDDEN);

    // Page 2
    btn_group_show(&page_wifi.page_2.dhcp.button, false);
    lv_obj_add_flag(page_wifi.page_2.ip_addr.label, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(page_wifi.page_2.ip_addr.input, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(page_wifi.page_2.netmask.label, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(page_wifi.page_2.gateway.label, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(page_wifi.page_2.gateway.input, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(page_wifi.page_2.netmask.input, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(page_wifi.page_2.dns.label, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(page_wifi.page_2.dns.input, LV_OBJ_FLAG_HIDDEN);
    slider_show(&page_wifi.page_2.rf_channel.input, false);
    lv_obj_add_flag(page_wifi.page_2.apply_settings, LV_OBJ_FLAG_HIDDEN);

    // Page 3
    lv_obj_add_flag(page_wifi.page_3.root_pw.label, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(page_wifi.page_3.root_pw.input, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(page_wifi.page_3.root_pw.status, LV_OBJ_FLAG_HIDDEN);
    btn_group_show(&page_wifi.page_3.ssh.button, false);
    lv_obj_add_flag(page_wifi.page_3.note, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(page_wifi.page_3.apply_settings, LV_OBJ_FLAG_HIDDEN);

    switch (which) {
    case 0:
        pp_wifi.p_arr.max = page_wifi.page_1.row_count;
        btn_group_show(&page_wifi.page_1.enable.button, true);
        btn_group_show(&page_wifi.page_1.mode.button, true);
        lv_obj_clear_flag(page_wifi.page_1.ssid.label, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(page_wifi.page_1.ssid.input, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(page_wifi.page_1.passwd.label, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(page_wifi.page_1.passwd.input, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(page_wifi.page_1.passwd.status, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(page_wifi.page_1.apply_settings, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(page_wifi.page_1.back, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(page_wifi.page_1.note, LV_OBJ_FLAG_HIDDEN);
        page_wifi_update_page_1_notes();
        break;
    case 1:
        pp_wifi.p_arr.max = page_wifi.page_2.row_count;
        btn_group_show(&page_wifi.page_2.dhcp.button, true);
        btn_group_enable(&page_wifi.page_2.dhcp.button, page_wifi.page_1.mode.button.current == WIFI_MODE_STA);
        if (page_wifi.page_1.mode.button.current != WIFI_MODE_STA) {
            lv_obj_clear_flag(pp_wifi.p_arr.panel[1], FLAG_SELECTABLE);
        }

        lv_obj_clear_flag(page_wifi.page_2.ip_addr.label, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(page_wifi.page_2.ip_addr.input, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(page_wifi.page_2.netmask.label, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(page_wifi.page_2.netmask.input, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(page_wifi.page_2.gateway.label, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(page_wifi.page_2.gateway.input, LV_OBJ_FLAG_HIDDEN);

        if (page_wifi.page_1.mode.button.current == WIFI_MODE_AP ||
            (page_wifi.page_1.mode.button.current == WIFI_MODE_STA &&
             page_wifi.page_2.dhcp.button.current == 1)) {
            lv_obj_clear_state(page_wifi.page_2.netmask.label, STATE_DISABLED);
            lv_obj_clear_state(page_wifi.page_2.netmask.input, STATE_DISABLED);
            lv_obj_clear_state(page_wifi.page_2.gateway.label, STATE_DISABLED);
            lv_obj_clear_state(page_wifi.page_2.gateway.input, STATE_DISABLED);
        } else {
            lv_obj_add_state(page_wifi.page_2.netmask.label, STATE_DISABLED);
            lv_obj_add_state(page_wifi.page_2.netmask.input, STATE_DISABLED);
            lv_obj_add_state(page_wifi.page_2.gateway.label, STATE_DISABLED);
            lv_obj_add_state(page_wifi.page_2.gateway.input, STATE_DISABLED);
            lv_obj_clear_flag(pp_wifi.p_arr.panel[3], FLAG_SELECTABLE);
            lv_obj_clear_flag(pp_wifi.p_arr.panel[4], FLAG_SELECTABLE);
        }

        lv_obj_clear_flag(page_wifi.page_2.dns.label, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(page_wifi.page_2.dns.input, LV_OBJ_FLAG_HIDDEN);

        if (page_wifi.page_1.mode.button.current == WIFI_MODE_STA &&
            page_wifi.page_2.dhcp.button.current == 1) {
            lv_obj_clear_state(page_wifi.page_2.dns.label, STATE_DISABLED);
            lv_obj_clear_state(page_wifi.page_2.dns.input, STATE_DISABLED);
        } else {
            lv_obj_add_state(page_wifi.page_2.dns.label, STATE_DISABLED);
            lv_obj_add_state(page_wifi.page_2.dns.input, STATE_DISABLED);
            lv_obj_clear_flag(pp_wifi.p_arr.panel[5], FLAG_SELECTABLE);
        }

        slider_show(&page_wifi.page_2.rf_channel.input, true);
        slider_enable(&page_wifi.page_2.rf_channel.input, page_wifi.page_1.mode.button.current == WIFI_MODE_AP);
        if (page_wifi.page_1.mode.button.current != WIFI_MODE_AP) {
            lv_obj_clear_flag(pp_wifi.p_arr.panel[6], FLAG_SELECTABLE);
        }

        lv_obj_clear_flag(page_wifi.page_2.apply_settings, LV_OBJ_FLAG_HIDDEN);
        break;
    case 2:
        pp_wifi.p_arr.max = page_wifi.page_3.row_count;
        lv_obj_clear_flag(page_wifi.page_3.root_pw.label, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(page_wifi.page_3.root_pw.input, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(page_wifi.page_3.root_pw.status, LV_OBJ_FLAG_HIDDEN);
        btn_group_show(&page_wifi.page_3.ssh.button, true);
        lv_obj_clear_flag(page_wifi.page_3.apply_settings, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(page_wifi.page_3.note, LV_OBJ_FLAG_HIDDEN);
        page_wifi_update_page_3_notes();
        break;
    }
}

/**
 * Revert the 'Apply Settings' text back to it's initial text and colored state.
 */
static void page_wifi_apply_settings_reset() {
    lv_label_set_text(page_wifi.page_1.apply_settings, _lang("Apply Settings"));
    lv_label_set_text(page_wifi.page_2.apply_settings, _lang("Apply Settings"));
    lv_label_set_text(page_wifi.page_3.apply_settings, _lang("Apply Settings"));
    page_wifi.confirm_settings = 0;
}

/**
 * Reset dirty flags across all objects.
 */
static void page_wifi_dirty_flag_reset() {
    page_wifi.dirty =
        page_wifi.page_1.enable.dirty =
            page_wifi.page_1.mode.dirty =
                page_wifi.page_1.ssid.dirty =
                    page_wifi.page_1.passwd.dirty =
                        page_wifi.page_2.dhcp.dirty =
                            page_wifi.page_2.ip_addr.dirty =
                                page_wifi.page_2.netmask.dirty =
                                    page_wifi.page_2.gateway.dirty =
                                        page_wifi.page_2.dns.dirty =
                                            page_wifi.page_2.rf_channel.dirty =
                                                page_wifi.page_3.root_pw.dirty =
                                                    page_wifi.page_3.ssh.dirty = false;
}

/**
 * Callback timer when changes are pending, `Apply Settings` will glow RED.
 */
static void page_wifi_apply_settings_pending_cb(struct _lv_timer_t *timer) {
    if (page_wifi.dirty && !page_wifi.confirm_settings) {
        static int dir = 20;
        static char buf[128];
        static uint8_t red = 150;
        snprintf(buf, sizeof(buf), "#%02x0000 %s#", red, _lang("Apply Settings"));
        lv_label_set_text(page_wifi.page_1.apply_settings, buf);
        lv_label_set_text(page_wifi.page_2.apply_settings, buf);
        lv_label_set_text(page_wifi.page_3.apply_settings, buf);
        if (red >= 250) {
            dir = -20;
            red = 250;
        } else if (red <= 150) {
            dir = 20;
            red = 150;
        }
        red += dir;
    }
}

/**
 * Callback invoked once `Apply Settings` is triggered and confirmed via the menu.
 */
static void page_wifi_apply_settings_timer_cb(struct _lv_timer_t *timer) {
    page_wifi_update_settings();
    page_wifi_dirty_flag_reset();
    page_wifi_apply_settings_reset();
}

/**
 * Verify if user entered text contains a valid network adresses.
 */
static bool page_wifi_is_network_address_valid(const char *address) {
    struct sockaddr_in sa;
    return 1 == inet_pton(AF_INET, address, &(sa.sin_addr));
}

/**
 * Marks the overall dirty bit and sets the callback if dirty.
 */
static void page_wifi_update_dirty_flag() {
    page_wifi.dirty =
        page_wifi.page_1.enable.dirty ||
        page_wifi.page_1.mode.dirty ||
        page_wifi.page_1.ssid.dirty ||
        page_wifi.page_1.passwd.dirty ||
        page_wifi.page_2.dhcp.dirty ||
        page_wifi.page_2.ip_addr.dirty ||
        page_wifi.page_2.netmask.dirty ||
        page_wifi.page_2.gateway.dirty ||
        page_wifi.page_2.dns.dirty ||
        page_wifi.page_2.rf_channel.dirty ||
        page_wifi.page_3.root_pw.dirty ||
        page_wifi.page_3.ssh.dirty;

    if (page_wifi.dirty) {
        if (!page_wifi_apply_settings_pending_timer) {
            page_wifi_apply_settings_pending_timer = lv_timer_create(page_wifi_apply_settings_pending_cb, 50, NULL);
            lv_timer_set_repeat_count(page_wifi_apply_settings_pending_timer, -1);
        }
    } else {
        page_wifi_apply_settings_reset();
    }
}

/**
 * Page 1 contains basic settings.
 */
static void page_wifi_create_page_1(lv_obj_t *parent) {
    char buf[64];
    create_btn_group_item(&page_wifi.page_1.enable.button, parent, 2, _lang("Enable"), _lang("On"), _lang("Off"), "", "", 1);
    btn_group_set_sel(&page_wifi.page_1.enable.button, !g_setting.wifi.enable);

    create_btn_group_item(&page_wifi.page_1.mode.button, parent, 2, _lang("Mode"), _lang("Host"), _lang("Client"), "", "", 2);
    btn_group_set_sel(&page_wifi.page_1.mode.button, g_setting.wifi.mode);

    page_wifi.page_1.ssid.label = create_label_item(parent, "SSID", 1, 3, 1);
    page_wifi.page_1.ssid.input = create_label_item(parent, g_setting.wifi.ssid[g_setting.wifi.mode], 2, 3, 2);

    page_wifi.page_1.passwd.label = create_label_item(parent, _lang("Password"), 1, 4, 1);
    page_wifi.page_1.passwd.input = create_label_item(parent, "", 2, 4, 2);
    page_wifi.page_1.passwd.status = create_label_item(parent, "", 4, 4, 2);
    page_wifi_mask_password(page_wifi.page_1.passwd.input, strlen(g_setting.wifi.passwd[g_setting.wifi.mode]));

    page_wifi.page_1.apply_settings = create_label_item(parent, _lang("Apply Settings"), 1, 5, 3);
    snprintf(buf, sizeof(buf), "< %s", _lang("Back"));
    page_wifi.page_1.back = create_label_item(parent, buf, 1, 6, 3);

    page_wifi.page_1.note = lv_label_create(parent);
    lv_obj_set_style_text_font(page_wifi.page_1.note, UI_PAGE_LABEL_FONT, 0);
    lv_obj_set_style_text_align(page_wifi.page_1.note, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_set_style_text_color(page_wifi.page_1.note, lv_color_hex(TEXT_COLOR_DEFAULT), 0);
    lv_obj_set_style_pad_top(page_wifi.page_1.note, UI_PAGE_TEXT_PAD, 0);
    lv_label_set_long_mode(page_wifi.page_1.note, LV_LABEL_LONG_WRAP);
    lv_obj_set_grid_cell(page_wifi.page_1.note, LV_GRID_ALIGN_START, 1, 4, LV_GRID_ALIGN_START, 7, 2);

    page_wifi.page_1.row_count = 7;
}

/**
 * Page 2 contains network addressing settings.
 */
static void page_wifi_create_page_2(lv_obj_t *parent) {
    create_btn_group_item(&page_wifi.page_2.dhcp.button, parent, 2, "DHCP", _lang("On"), _lang("Off"), "", "", 1);
    btn_group_set_sel(&page_wifi.page_2.dhcp.button, !g_setting.wifi.dhcp);

    page_wifi.page_2.ip_addr.label = create_label_item(parent, _lang("Address"), 1, 2, 1);
    page_wifi.page_2.ip_addr.input = create_label_item(parent, g_setting.wifi.ip_addr, 2, 2, 2);
    page_wifi.page_2.ip_addr.status = create_label_item(parent, "", 4, 2, 2);
    page_wifi.page_2.netmask.label = create_label_item(parent, _lang("Netmask"), 1, 3, 1);
    page_wifi.page_2.netmask.input = create_label_item(parent, g_setting.wifi.netmask, 2, 3, 2);
    page_wifi.page_2.netmask.status = create_label_item(parent, "", 4, 3, 2);
    page_wifi.page_2.gateway.label = create_label_item(parent, _lang("Gateway"), 1, 4, 1);
    page_wifi.page_2.gateway.input = create_label_item(parent, g_setting.wifi.gateway, 2, 4, 2);
    page_wifi.page_2.gateway.status = create_label_item(parent, "", 4, 4, 2);
    page_wifi.page_2.dns.label = create_label_item(parent, "DNS", 1, 5, 1);
    page_wifi.page_2.dns.input = create_label_item(parent, g_setting.wifi.gateway, 2, 5, 2);
    page_wifi.page_2.dns.status = create_label_item(parent, "", 4, 5, 2);
    create_slider_item(&page_wifi.page_2.rf_channel.input, parent, _lang("RF Channel"), WIFI_RF_CHANNELS - 1, g_setting.wifi.rf_channel, 6);
    lv_slider_set_value(page_wifi.page_2.rf_channel.input.slider, g_setting.wifi.rf_channel - 1, LV_ANIM_OFF);
    page_wifi.page_2.apply_settings = create_label_item(parent, "Apply Settings", 1, 7, 3);

    page_wifi.page_2.row_count = 8;
}

/**
 * Page 3 contains service settings.
 */
static void page_wifi_create_page_3(lv_obj_t *parent) {
    page_wifi.page_3.root_pw.label = create_label_item(parent, _lang("Root PW"), 1, 1, 1);
    page_wifi.page_3.root_pw.input = create_label_item(parent, "", 2, 1, 2);
    page_wifi.page_3.root_pw.status = create_label_item(parent, "", 4, 1, 2);
    page_wifi_mask_password(page_wifi.page_3.root_pw.input, strlen(g_setting.wifi.root_pw));

    create_btn_group_item(&page_wifi.page_3.ssh.button, parent, 2, "SSH", _lang("On"), _lang("Off"), "", "", 2);
    btn_group_set_sel(&page_wifi.page_3.ssh.button, !g_setting.wifi.ssh);

    page_wifi.page_3.apply_settings = create_label_item(parent, "Apply Settings", 1, 3, 3);

    page_wifi.page_3.note = lv_label_create(parent);
    lv_obj_set_style_text_font(page_wifi.page_3.note, UI_PAGE_LABEL_FONT, 0);
    lv_obj_set_style_text_align(page_wifi.page_3.note, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_set_style_text_color(page_wifi.page_3.note, lv_color_hex(TEXT_COLOR_DEFAULT), 0);
    lv_obj_set_style_pad_top(page_wifi.page_3.note, UI_PAGE_TEXT_PAD, 0);
    lv_label_set_long_mode(page_wifi.page_3.note, LV_LABEL_LONG_WRAP);
    lv_obj_set_grid_cell(page_wifi.page_3.note, LV_GRID_ALIGN_START, 1, 4, LV_GRID_ALIGN_START, 7, 2);
    page_wifi_update_page_3_notes();

    page_wifi.page_3.row_count = 4;
}

/**
 * Main allocation routine for this page.
 */
static lv_obj_t *page_wifi_create(lv_obj_t *parent, panel_arr_t *arr) {
    snprintf(INVALID_TOO_SHORT_STR, sizeof(INVALID_TOO_SHORT_STR), "#FF0000 %s#", _lang("Invalid Too Short"));
    snprintf(INVALID_FORMAT_STR, sizeof(INVALID_FORMAT_STR), "#FF0000 %s#", _lang("Invalid Format"));

    char buf[128];
    lv_obj_t *page = lv_menu_page_create(parent, NULL);
    lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(page, UI_PAGE_VIEW_SIZE);
    lv_obj_add_style(page, &style_subpage, LV_PART_MAIN);

    lv_obj_t *section = lv_menu_section_create(page);
    lv_obj_add_style(section, &style_submenu, LV_PART_MAIN);
    lv_obj_set_size(section, UI_PAGE_VIEW_SIZE);

    snprintf(buf, sizeof(buf), "%s:", _lang("WiFi Module"));
    create_text(NULL, section, false, buf, LV_MENU_ITEM_BUILDER_VARIANT_2);

    lv_obj_t *cont = lv_obj_create(section);
    lv_obj_set_size(cont, UI_PAGE_VIEW_SIZE);
    lv_obj_set_pos(cont, 0, 0);
    lv_obj_set_layout(cont, LV_LAYOUT_GRID);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(cont, &style_context, LV_PART_MAIN);

    lv_obj_set_style_grid_column_dsc_array(cont, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(cont, row_dsc, 0);

    create_select_item(arr, cont);

    create_btn_group_item(&page_wifi.page_select.button, cont, 3, _lang("Page"), _lang("Basic"), _lang("Advanced"), _lang("System"), "", 0);
    page_wifi_create_page_1(cont);
    page_wifi_create_page_2(cont);
    page_wifi_create_page_3(cont);
    page_wifi_sync_settings();
    page_wifi_update_current_page(0);

    return page;
}

/**
 * Main entry routine for this page.
 */
static void page_wifi_enter() {
    app_state_push(APP_STATE_WIFI);
    keyboard_clear_text();
    page_wifi_update_current_page(0);
    page_wifi.item_select = 0;
}

/**
 * Main exit routine for this page.
 */
static void page_wifi_exit() {
    btn_group_set_sel(&page_wifi.page_select.button, 0);
    page_wifi_update_current_page(0);

    if (page_wifi.page_1.enable.dirty) {
        btn_group_toggle_sel(&page_wifi.page_1.enable.button);
    }

    if (page_wifi.page_1.mode.dirty) {
        btn_group_toggle_sel(&page_wifi.page_1.mode.button);
    }

    int mode = btn_group_get_sel(&page_wifi.page_1.mode.button);
    snprintf(page_wifi.page_1.ssid.text[mode], WIFI_SSID_MAX, "%s", g_setting.wifi.ssid[mode]);
    lv_label_set_text(page_wifi.page_1.ssid.input, page_wifi.page_1.ssid.text[mode]);
    snprintf(page_wifi.page_1.passwd.text[mode], WIFI_PASSWD_MAX, "%s", g_setting.wifi.passwd[mode]);
    page_wifi_mask_password(page_wifi.page_1.passwd.input, strlen(page_wifi.page_1.passwd.text[mode]));
    lv_label_set_text(page_wifi.page_1.passwd.status, "");
    snprintf(page_wifi.page_3.root_pw.text, WIFI_PASSWD_MAX, "%s", g_setting.wifi.root_pw);
    page_wifi_mask_password(page_wifi.page_3.root_pw.input, strlen(page_wifi.page_3.root_pw.text));
    lv_label_set_text(page_wifi.page_3.root_pw.status, "");

    page_wifi_dirty_flag_reset();
    page_wifi_apply_settings_reset();

    keyboard_close();
    page_wifi.item_select = 0;
}

/**
 *  Invoked periodically.
 */
static void page_wifi_on_update(uint32_t delta_ms) {
    static uint32_t elapsed = -1;

    // Check immediately after running, then every 5 minutes.
    if (g_setting.wifi.enable && (elapsed == -1 || (elapsed += delta_ms) > 300000)) {
        switch (g_setting.wifi.mode) {
        case WIFI_MODE_STA:
            if (page_wifi_get_real_address()) {
                if (!fs_file_exists("/tmp/hdz_goggle_fw.latest") &&
                    !fs_file_exists("/tmp/hdz_vtx_fw.latest")) {
                    system_script(WIFI_DOWNLOAD);
                }
            }
            break;
        }

        elapsed = 0;
    }
}

/**
 * Main navigation routine for this page.
 */
static void page_wifi_on_roller(uint8_t key) {
    // Ignore commands until timer has expired before allowing user to proceed.
    if (page_wifi.confirm_settings == 2) {
        return;
    }

    // If a click was not previous pressed to confirm, then update is canceled.
    page_wifi.confirm_settings = 0;

    if (!page_wifi.dirty) {
        page_wifi_apply_settings_reset();
    }

    // Focused Scroll Wheel
    if (keyboard_active()) {
        keyboard_scroll(key);
    } else if (page_wifi.page_2.rf_channel.active) {
        int32_t value = lv_slider_get_value(page_wifi.page_2.rf_channel.input.slider);
        if (key == DIAL_KEY_DOWN) {
            if (value > 0) {
                value -= 1;
            }
        } else {
            if (value < WIFI_RF_CHANNELS - 1) {
                value += 1;
            }
        }
        char buf[12];
        snprintf(buf, sizeof(buf), "%d", value + 1);
        lv_label_set_text(page_wifi.page_2.rf_channel.input.label, buf);
        lv_slider_set_value(page_wifi.page_2.rf_channel.input.slider, value, LV_ANIM_OFF);
    }
}

/**
 * Common handling method of the three "apply settings" buttons.
 */
static void page_wifi_handle_apply_button(lv_obj_t *apply_button) {
    uint8_t buf[256];
    if (page_wifi.confirm_settings) {
        snprintf(buf, sizeof(buf), "#FF0000 %s...#", _lang("Updating WiFi"));
        lv_label_set_text(apply_button, buf);
        page_wifi_apply_settings_timer = lv_timer_create(page_wifi_apply_settings_timer_cb, 1000, NULL);
        lv_timer_set_repeat_count(page_wifi_apply_settings_timer, 1);
        page_wifi.confirm_settings = 2;
    } else {
        snprintf(buf, sizeof(buf), "#FFFF00 %s...#", _lang("Click to confirm or Scroll to cancel"));
        lv_label_set_text(apply_button, buf);
        page_wifi.confirm_settings = 1;
    }
}

/**
 * Main input selection routine for this page.
 */
static void page_wifi_on_click(uint8_t key, int sel) {
    char buf[128];
    page_wifi.item_select = sel;

    switch (page_wifi.item_select) {
    case 0: // Page Toggle
        btn_group_toggle_sel(&page_wifi.page_select.button);
        page_wifi_update_current_page(btn_group_get_sel(&page_wifi.page_select.button));
        break;
    case 1:
        switch (btn_group_get_sel(&page_wifi.page_select.button)) {
        case 0: // Page Basic: Enable
            btn_group_toggle_sel(&page_wifi.page_1.enable.button);
            page_wifi.page_1.enable.dirty =
                (btn_group_get_sel(&page_wifi.page_1.enable.button) != !g_setting.wifi.enable);
            break;
        case 1: // Page Advanced: DHCP
            btn_group_toggle_sel(&page_wifi.page_2.dhcp.button);
            page_wifi.page_2.dhcp.dirty =
                (btn_group_get_sel(&page_wifi.page_2.dhcp.button) != !g_setting.wifi.dhcp);
            page_wifi_update_current_page(page_wifi.page_select.button.current);
            break;
        case 2: // Page System: Root PW
            if (!keyboard_active()) {
                keyboard_set_text(page_wifi.page_3.root_pw.text);
                keyboard_open();
            } else {
                keyboard_press();
            }
            break;
        }
        break;
    case 2:
        switch (btn_group_get_sel(&page_wifi.page_select.button)) {
        case 0: // Page Basic: Mode
            btn_group_toggle_sel(&page_wifi.page_1.mode.button);
            int mode = btn_group_get_sel(&page_wifi.page_1.mode.button);
            lv_label_set_text(page_wifi.page_1.ssid.input, page_wifi.page_1.ssid.text[mode]);
            page_wifi_mask_password(page_wifi.page_1.passwd.input, strlen(page_wifi.page_1.passwd.text[mode]));
            page_wifi.page_1.mode.dirty =
                (btn_group_get_sel(&page_wifi.page_1.mode.button) != g_setting.wifi.mode);
            break;
        case 1: // Page Advance: Address
            if (!keyboard_active()) {
                keyboard_set_text(page_wifi.page_2.ip_addr.text);
                keyboard_open();
            } else {
                keyboard_press();
            }
            break;
        case 2: // Page System: SSH
            btn_group_toggle_sel(&page_wifi.page_3.ssh.button);
            page_wifi.page_3.ssh.dirty =
                (btn_group_get_sel(&page_wifi.page_3.ssh.button) != !g_setting.wifi.ssh);
            break;
        }
        break;
    case 3:
        switch (btn_group_get_sel(&page_wifi.page_select.button)) {
        case 0: // Page Basic: SSID
            if (!keyboard_active()) {
                int mode = btn_group_get_sel(&page_wifi.page_1.mode.button);
                keyboard_set_text(page_wifi.page_1.ssid.text[mode]);
                keyboard_open();
            } else {
                keyboard_press();
            }
            break;
        case 1: // Page Advance: Netmask
            if (!keyboard_active()) {
                keyboard_set_text(page_wifi.page_2.netmask.text);
                keyboard_open();
            } else {
                keyboard_press();
            }
            break;
        case 2: // Page System: Apply Settings
            page_wifi_handle_apply_button(page_wifi.page_3.apply_settings);
            break;
        }
        break;
    case 4:
        switch (btn_group_get_sel(&page_wifi.page_select.button)) {
        case 0: // Page Basic: Password
            if (!keyboard_active()) {
                int mode = btn_group_get_sel(&page_wifi.page_1.mode.button);
                keyboard_set_text(page_wifi.page_1.passwd.text[mode]);
                keyboard_open();
            } else {
                keyboard_press();
            }
            break;
        case 1: // Page Advanced: Gateway
            if (!keyboard_active()) {
                keyboard_set_text(page_wifi.page_2.gateway.text);
                keyboard_open();
            } else {
                keyboard_press();
            }
            break;
        }
        break;
    case 5:
        switch (btn_group_get_sel(&page_wifi.page_select.button)) {
        case 0: // Page Basic: Apply Settings
            page_wifi_handle_apply_button(page_wifi.page_1.apply_settings);
            break;
        case 1: // Page Advanced: DNS
            if (!keyboard_active()) {
                keyboard_set_text(page_wifi.page_2.dns.text);
                keyboard_open();
            } else {
                keyboard_press();
            }
            break;
        }
        break;
    case 6:
        switch (btn_group_get_sel(&page_wifi.page_select.button)) {
        case 0: // Page Basic: Back
            submenu_exit();
            break;
        case 1: // Page Advanced: RF Channel
            if (!page_wifi.page_2.rf_channel.active) {
                page_wifi.page_2.rf_channel.active = true;
            } else {
                page_wifi.page_2.rf_channel.active = false;
                page_wifi.page_2.rf_channel.dirty =
                    lv_slider_get_value(page_wifi.page_2.rf_channel.input.slider) != g_setting.wifi.rf_channel - 1;
            }
            break;
        }
        break;
    case 7:
        switch (btn_group_get_sel(&page_wifi.page_select.button)) {
        case 1: // Page Advanced: Apply Settings
            page_wifi_handle_apply_button(page_wifi.page_2.apply_settings);
            break;
        }
        break;
    }

    // Enable/Disable panel scrolling when elements are in focus
    pp_wifi.p_arr.max =
        keyboard_active() || page_wifi.page_2.rf_channel.active
            ? 0
            : page_wifi_get_current_page_max();

    page_wifi_update_dirty_flag();
}

static void page_wifi_on_right_button(bool is_short) {
    if (is_short) {
        if (keyboard_active()) {
            switch (page_wifi.item_select) {
            case 1: {
                int written = keyboard_get_text(page_wifi.page_3.root_pw.text, WIFI_PASSWD_MAX);
                if (0 < written) {
                    page_wifi_mask_password(page_wifi.page_3.root_pw.input, strlen(page_wifi.page_3.root_pw.text));
                    if (written < 8) {
                        lv_label_set_text(page_wifi.page_3.root_pw.status, INVALID_TOO_SHORT_STR);
                    } else {
                        lv_label_set_text(page_wifi.page_3.root_pw.status, "");
                        page_wifi.page_3.root_pw.dirty =
                            (0 != strcmp(page_wifi.page_3.root_pw.text, g_setting.wifi.root_pw));
                    }
                }
                break;
            }
            case 2:
                switch (btn_group_get_sel(&page_wifi.page_select.button)) {
                case 1:
                    if (keyboard_get_text(page_wifi.page_2.ip_addr.text, WIFI_NETWORK_MAX)) {
                        lv_label_set_text(page_wifi.page_2.ip_addr.input, page_wifi.page_2.ip_addr.text);
                        if (!page_wifi_is_network_address_valid(page_wifi.page_2.ip_addr.text)) {
                            lv_label_set_text(page_wifi.page_2.ip_addr.status, INVALID_FORMAT_STR);
                        } else {
                            lv_label_set_text(page_wifi.page_2.ip_addr.status, "");
                        }
                        page_wifi.page_2.ip_addr.dirty =
                            (0 != strcmp(page_wifi.page_2.ip_addr.text, g_setting.wifi.ip_addr));
                    }
                    break;
                }
                break;
            case 3:
                switch (btn_group_get_sel(&page_wifi.page_select.button)) {
                case 0: {
                    int mode = btn_group_get_sel(&page_wifi.page_1.mode.button);
                    if (keyboard_get_text(page_wifi.page_1.ssid.text[mode], WIFI_SSID_MAX)) {
                        lv_label_set_text(page_wifi.page_1.ssid.input, page_wifi.page_1.ssid.text[mode]);
                        page_wifi.page_1.ssid.dirty =
                            (0 != strcmp(page_wifi.page_1.ssid.text[mode], g_setting.wifi.ssid[mode]));
                    }
                    break;
                }
                case 1:
                    if (keyboard_get_text(page_wifi.page_2.netmask.text, WIFI_NETWORK_MAX)) {
                        lv_label_set_text(page_wifi.page_2.netmask.input, page_wifi.page_2.netmask.text);
                        if (!page_wifi_is_network_address_valid(page_wifi.page_2.netmask.text)) {
                            lv_label_set_text(page_wifi.page_2.netmask.status, INVALID_FORMAT_STR);
                        } else {
                            lv_label_set_text(page_wifi.page_2.netmask.status, "");
                        }
                        page_wifi.page_2.netmask.dirty =
                            (0 != strcmp(page_wifi.page_2.netmask.text, g_setting.wifi.netmask));
                    }
                    break;
                }
                break;
            case 4:
                switch (btn_group_get_sel(&page_wifi.page_select.button)) {
                case 0: {
                    int mode = btn_group_get_sel(&page_wifi.page_1.mode.button);
                    int written = keyboard_get_text(page_wifi.page_1.passwd.text[mode], WIFI_PASSWD_MAX);
                    if (0 < written) {
                        page_wifi_mask_password(page_wifi.page_1.passwd.input, strlen(page_wifi.page_1.passwd.text[mode]));
                        if (written < 8) {
                            lv_label_set_text(page_wifi.page_1.passwd.status, INVALID_TOO_SHORT_STR);
                        } else {
                            lv_label_set_text(page_wifi.page_1.passwd.status, "");
                            page_wifi.page_1.passwd.dirty =
                                (0 != strcmp(page_wifi.page_1.passwd.text[mode], g_setting.wifi.passwd[mode]));
                        }
                    }
                    break;
                }
                case 1:
                    if (keyboard_get_text(page_wifi.page_2.gateway.text, WIFI_NETWORK_MAX)) {
                        lv_label_set_text(page_wifi.page_2.gateway.input, page_wifi.page_2.gateway.text);
                        if (!page_wifi_is_network_address_valid(page_wifi.page_2.gateway.text)) {
                            lv_label_set_text(page_wifi.page_2.gateway.status, INVALID_FORMAT_STR);
                        } else {
                            lv_label_set_text(page_wifi.page_2.gateway.status, "");
                        }
                        page_wifi.page_2.gateway.dirty =
                            (0 != strcmp(page_wifi.page_2.gateway.text, g_setting.wifi.gateway));
                    }
                    break;
                }
                break;
            case 5:
                switch (btn_group_get_sel(&page_wifi.page_select.button)) {
                case 1:
                    if (keyboard_get_text(page_wifi.page_2.dns.text, WIFI_NETWORK_MAX)) {
                        lv_label_set_text(page_wifi.page_2.dns.input, page_wifi.page_2.dns.text);
                        if (!page_wifi_is_network_address_valid(page_wifi.page_2.dns.text)) {
                            lv_label_set_text(page_wifi.page_2.dns.status, INVALID_FORMAT_STR);
                        } else {
                            lv_label_set_text(page_wifi.page_2.dns.status, "");
                        }
                        page_wifi.page_2.dns.dirty =
                            (0 != strcmp(page_wifi.page_2.dns.text, g_setting.wifi.dns));
                    }
                    break;
                }
                break;
            }

            // Restore panel scrolling when elements are in focus
            pp_wifi.p_arr.max = page_wifi_get_current_page_max();
            page_wifi_update_dirty_flag();
            keyboard_close();
        }
    } else {
        if (keyboard_active()) {
            keyboard_clear_text();
        }
    }
}

void page_wifi_post_bootup_action(void (*complete_callback)()) {
    page_wifi_update_settings();

    if (complete_callback != NULL) {
        page_wifi_bootup_pending = false;
        complete_callback();
    }
}

/**
 * Main Menu page data structure, notice max is set to zero
 * in order to allow us to override default user input logic.
 */
page_pack_t pp_wifi = {
    .p_arr = {
        .cur = 0,
        .max = 0,
    },
    .name = "WiFi Module",
    .create = page_wifi_create,
    .enter = page_wifi_enter,
    .exit = page_wifi_exit,
    .on_created = NULL,
    .on_update = page_wifi_on_update,
    .on_roller = page_wifi_on_roller,
    .on_click = page_wifi_on_click,
    .on_right_button = page_wifi_on_right_button,
    .post_bootup_run_priority = 100,
    .post_bootup_run_function = page_wifi_post_bootup_action,
};

/**
 * Provides the WiFi status string referenced by ui_statusbar.
 */
void page_wifi_get_statusbar_text(char *buffer, int size) {
    if (!g_setting.wifi.enable) {
        snprintf(buffer, size, "WiFi: %s", _lang("Off"));
    } else if (page_wifi_bootup_pending) {
        snprintf(buffer, size, "WiFi: %s", _lang("Pending"));
    } else {
        switch (g_setting.wifi.mode) {
        case WIFI_MODE_AP:
            snprintf(buffer, size, "WiFi: %s", g_setting.wifi.ssid[WIFI_MODE_AP]);
            break;
        case WIFI_MODE_STA:
            if (page_wifi_get_real_address()) {
                snprintf(buffer, size, "WiFi: %s", g_setting.wifi.ssid[WIFI_MODE_STA]);
            } else {
                snprintf(buffer, size, "WiFi: %s", _lang("Searching"));
            }
            break;
        }
    }
}
