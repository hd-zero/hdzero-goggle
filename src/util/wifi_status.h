#ifndef WIFI_STATUS_H
#define WIFI_STATUS_H

typedef enum {
    WIFI_STA_STATUS_DISCONNECTED = 0,
    WIFI_STA_STATUS_CONNECTING,
    WIFI_STA_STATUS_CONNECTED,
    WIFI_STA_STATUS_FAILED
} wifi_sta_status_t;

typedef struct {
    wifi_sta_status_t state;
    char ssid[32];
    int signal_strength;
} WIFI_STA_CONNECT_STATUS_S;

int wifi_sta_get_connect_status(const char *interface, WIFI_STA_CONNECT_STATUS_S *status);

#endif // WIFI_STATUS_H
