#include "util/wifi_status.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <linux/wireless.h>

int wifi_sta_get_connect_status(const char *interface, WIFI_STA_CONNECT_STATUS_S *status) {
    struct iwreq wreq;
    int sockfd;
    
    if (!interface || !status) {
        return -1;
    }

    // Initialize status
    memset(status, 0, sizeof(WIFI_STA_CONNECT_STATUS_S));
    
    // Create socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        return -1;
    }

    // Prepare request
    memset(&wreq, 0, sizeof(struct iwreq));
    strncpy(wreq.ifr_name, interface, IFNAMSIZ-1);

    // Get connection status
    if (ioctl(sockfd, SIOCGIWNAME, &wreq) >= 0) {
        // Check if interface is up
        struct ifreq ifr;
        memset(&ifr, 0, sizeof(ifr));
        strncpy(ifr.ifr_name, interface, IFNAMSIZ-1);
        
        if (ioctl(sockfd, SIOCGIFFLAGS, &ifr) >= 0) {
            if (ifr.ifr_flags & IFF_UP) {
                status->state = WIFI_STA_STATUS_CONNECTED;
            } else {
                status->state = WIFI_STA_STATUS_DISCONNECTED;
            }
        }
    } else {
        status->state = WIFI_STA_STATUS_FAILED;
    }

    close(sockfd);
    return 0;
}
