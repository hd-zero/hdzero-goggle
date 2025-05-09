#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <arpa/inet.h>

#include <log/log.h>

#include "driver/rtc.h"
#include "util/system.h"
#include "core/settings.h"
#include "ui/page_clock.h"

#define NTP_TIMESTAMP_DELTA 2208988800ull

// Estructura del paquete NTP
typedef struct {
    uint8_t li_vn_mode;      /* leap indicator, version and mode */
    uint8_t stratum;         /* stratum level */
    uint8_t poll;            /* poll interval */
    uint8_t precision;       /* precision */
    uint32_t rootdelay;      /* root delay */
    uint32_t rootdispersion; /* root dispersion */
    uint32_t refid;          /* reference ID */
    uint32_t refts_sec;      /* reference timestamp seconds */
    uint32_t refts_frac;     /* reference timestamp fraction */
    uint32_t origts_sec;     /* originate timestamp seconds */
    uint32_t origts_frac;    /* originate timestamp fraction */
    uint32_t recvts_sec;     /* receive timestamp seconds */
    uint32_t recvts_frac;    /* receive timestamp fraction */
    uint32_t xmitts_sec;     /* transmit timestamp seconds */
    uint32_t xmitts_frac;    /* transmit timestamp fraction */
} ntp_packet;

int clock_sync_from_ntp(void) {
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    ntp_packet packet = {0};
    
    // Verificar si WiFi está habilitado
    if (!g_setting.wifi.enable) {
        LOGE("WiFi disabled, cannot sync time from NTP");
        return -1;
    }
    
    // Habilitar WiFi si es necesario
    system_exec("ifconfig wlan0 up");
    usleep(1000000); // Esperar 1 segundo para que se conecte
    
    // Inicializar paquete NTP
    packet.li_vn_mode = 0x1b; // Leap = 0, Version = 3, Mode = 3 (cliente)
    
    // Crear socket
    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd < 0) {
        LOGE("Error opening socket for NTP");
        return -1;
    }
    
    // Configurar timeout
    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        LOGE("Error setting socket timeout");
        close(sockfd);
        return -1;
    }
    
    // Configurar servidor NTP - intentar conexión por nombre
    server = gethostbyname("pool.ntp.org");
    if (server == NULL) {
        LOGI("Could not resolve pool.ntp.org, trying with IP address");
        // Si falla, intentar con IPs directas
        memset((char *)&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(123); // Puerto NTP
        
        // Intentar con una IP estática de un servidor NTP conocido
        if (inet_pton(AF_INET, "162.159.200.1", &serv_addr.sin_addr) <= 0) {
            LOGE("Invalid NTP server address");
            close(sockfd);
            return -1;
        }
    } else {
        // Configuración normal si la resolución de nombres funciona
        memset((char *)&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        memcpy((char *)&serv_addr.sin_addr.s_addr, (char *)server->h_addr, server->h_length);
        serv_addr.sin_port = htons(123); // Puerto NTP
    }
    
    // Enviar paquete
    n = sendto(sockfd, (char *)&packet, sizeof(ntp_packet), 0, 
              (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (n < 0) {
        LOGE("Error writing to socket for NTP");
        close(sockfd);
        return -1;
    }
    
    // Recibir respuesta
    n = recvfrom(sockfd, (char *)&packet, sizeof(ntp_packet), 0, NULL, NULL);
    if (n < 0) {
        LOGE("Error reading from socket for NTP");
        close(sockfd);
        return -1;
    }
    
    close(sockfd);
    
    // Convertir tiempo recibido
    uint32_t txTm = ntohl(packet.xmitts_sec);
    time_t time_ntp = txTm - NTP_TIMESTAMP_DELTA;
    
    LOGI("NTP time received: %u", (unsigned int)time_ntp);
    
    // Actualizar RTC con el tiempo recibido
    struct timeval tv_now;
    tv_now.tv_sec = time_ntp;
    tv_now.tv_usec = 0;
    
    struct rtc_date rd;
    rtc_tv2rd(&tv_now, &rd);
    
    // Validar la fecha
    if (rtc_has_valid_date(&rd) != 0) {
        LOGE("NTP returned invalid date: %04d-%02d-%02d %02d:%02d:%02d", 
             rd.year, rd.month, rd.day, rd.hour, rd.min, rd.sec);
        return -1;
    }
    
    // Actualizar el RTC
    rtc_set_clock(&rd);
    LOGI("NTP time sync successful: %04d-%02d-%02d %02d:%02d:%02d", 
         rd.year, rd.month, rd.day, rd.hour, rd.min, rd.sec);
    
    return 0;
}