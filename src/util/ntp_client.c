#include <minIni.h>
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
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>

#include <log/log.h>

#include "driver/rtc.h"
#include "util/system.h"
#include "core/settings.h"
#include "ui/page_clock.h"
#include "util/ntp_client.h"

#define NTP_TIMESTAMP_DELTA 2208988800ull  // Segundos entre 1900 (NTP) y 1970 (epoch)
#define NTP_PORT 123
#define NTP_TIMEOUT_SEC 3                  // Timeout reducido a 3 segundos

// Estados para la sincronización NTP
typedef enum {
    NTP_SYNC_IDLE = 0,
    NTP_SYNC_IN_PROGRESS,
    NTP_SYNC_SUCCESS,
    NTP_SYNC_FAILED
} ntp_sync_state_t;

// Estructura del paquete NTP según RFC 5905
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
} ntp_packet_t;

// Estructura para el callback
typedef struct {
    ntp_callback_t callback_fn;
    void* user_data;
} ntp_callback_data_t;

// Variables globales
static volatile ntp_sync_state_t g_ntp_sync_state = NTP_SYNC_IDLE;
static pthread_mutex_t g_ntp_mutex = PTHREAD_MUTEX_INITIALIZER;
static char* g_ntp_servers[] = {
    "pool.ntp.org",
    "time.google.com",
    "time.cloudflare.com",
    "time.apple.com",
    "time.windows.com"
};
static const int g_ntp_server_count = sizeof(g_ntp_servers) / sizeof(g_ntp_servers[0]);

// Lista de direcciones IP estáticas como respaldo
static char* g_ntp_fallback_ips[] = {
    "162.159.200.1",    // time.cloudflare.com
    "216.239.35.4",     // time.google.com
    "17.253.114.125",   // time.apple.com
    "13.65.245.138"     // time.windows.com
};
static const int g_ntp_fallback_count = sizeof(g_ntp_fallback_ips) / sizeof(g_ntp_fallback_ips[0]);

// Función para convertir el tiempo NTP a unix time
static time_t ntp_time_to_unix_time(uint32_t ntp_time) {
    return (time_t)(ntp_time - NTP_TIMESTAMP_DELTA);
}

// Función para establecer socket no bloqueante
static int set_socket_nonblocking(int sockfd) {
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1) {
        return -1;
    }
    return fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
}

// Función para intentar resolver y conectar a un servidor NTP
static int connect_to_ntp_server(const char* server, struct sockaddr_in* serv_addr) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd < 0) {
        LOGE("Error creating socket for NTP");
        return -1;
    }

    // Configurar timeout
    struct timeval tv;
    tv.tv_sec = NTP_TIMEOUT_SEC;
    tv.tv_usec = 0;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0 ||
        setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) < 0) {
        LOGE("Error setting socket timeout");
        close(sockfd);
        return -1;
    }

    // Intentar resolver el nombre del servidor
    struct hostent *host_entry = gethostbyname(server);
    if (host_entry == NULL) {
        LOGI("Could not resolve %s", server);
        close(sockfd);
        return -1;
    }

    // Configurar dirección del servidor
    memset(serv_addr, 0, sizeof(struct sockaddr_in));
    serv_addr->sin_family = AF_INET;
    memcpy(&serv_addr->sin_addr.s_addr, host_entry->h_addr, host_entry->h_length);
    serv_addr->sin_port = htons(NTP_PORT);

    return sockfd;
}

// Función para intentar conectar a una IP de servidor NTP específica
static int connect_to_ntp_ip(const char* ip_address, struct sockaddr_in* serv_addr) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd < 0) {
        LOGE("Error creating socket for NTP");
        return -1;
    }

    // Configurar timeout
    struct timeval tv;
    tv.tv_sec = NTP_TIMEOUT_SEC;
    tv.tv_usec = 0;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0 ||
        setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) < 0) {
        LOGE("Error setting socket timeout");
        close(sockfd);
        return -1;
    }
    
    // Configurar dirección del servidor
    memset(serv_addr, 0, sizeof(struct sockaddr_in));
    serv_addr->sin_family = AF_INET;
    serv_addr->sin_port = htons(NTP_PORT);
    
    if (inet_pton(AF_INET, ip_address, &serv_addr->sin_addr) <= 0) {
        LOGE("Invalid IP address: %s", ip_address);
        close(sockfd);
        return -1;
    }
    
    return sockfd;
}

// Función para enviar y recibir paquete NTP (con reintentos)
static int send_receive_ntp_packet(int sockfd, struct sockaddr_in* serv_addr, ntp_packet_t* packet) {
    int retries = 3;
    socklen_t addr_len = sizeof(struct sockaddr_in);
    
    // Inicializar paquete NTP
    memset(packet, 0, sizeof(ntp_packet_t));
    packet->li_vn_mode = 0x1b; // LI = 0, Version = 3, Mode = 3 (cliente)
    
    while (retries--) {
        // Enviar paquete
        if (sendto(sockfd, packet, sizeof(ntp_packet_t), 0, 
                  (struct sockaddr *)serv_addr, addr_len) < 0) {
            LOGE("Error sending NTP packet (retry %d): %s", 2-retries, strerror(errno));
            continue;
        }
        
        // Configurar select para timeout
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        
        struct timeval timeout;
        timeout.tv_sec = 1;  // 1 segundo por intento
        timeout.tv_usec = 0;
        
        int select_result = select(sockfd + 1, &readfds, NULL, NULL, &timeout);
        
        if (select_result <= 0) {
            if (select_result == 0) {
                LOGI("NTP response timeout (retry %d)", 2-retries);
            } else {
                LOGE("Select error (retry %d): %s", 2-retries, strerror(errno));
            }
            continue;
        }
        
        // Recibir respuesta
        if (recvfrom(sockfd, packet, sizeof(ntp_packet_t), 0, 
                    (struct sockaddr *)serv_addr, &addr_len) < 0) {
            LOGE("Error receiving NTP packet (retry %d): %s", 2-retries, strerror(errno));
            continue;
        }
        
        // Éxito
        return 0;
    }
    
    // Agotados los intentos
    return -1;
}

// Función para calcular correctamente la diferencia de zona horaria
static time_t apply_timezone_offset(time_t utc_time, int offset_seconds) {
    // Simplemente sumar el offset en segundos
    return utc_time + offset_seconds;
}

// Función que realiza la sincronización NTP (ejecutada en un hilo)
static void* ntp_sync_thread(void* arg) {
    ntp_callback_data_t* callback_data = (ntp_callback_data_t*)arg;
    int result = -1;
    int sockfd = -1;
    ntp_packet_t packet;
    struct sockaddr_in serv_addr;
    
    LOGI("NTP sync thread started");
    
    // Verificar si WiFi está habilitado
    if (!g_setting.wifi.enable) {
        LOGE("WiFi disabled, cannot sync time from NTP");
        goto cleanup;
    }
    
    // Asegurarse de que WiFi esté activo
    system_exec("ifconfig wlan0 up");
    usleep(500000); // Espera reducida a 0.5 segundos
    
    // Intentar varios servidores NTP
    for (int i = 0; i < g_ntp_server_count; i++) {
        LOGI("Trying NTP server: %s", g_ntp_servers[i]);
        
        sockfd = connect_to_ntp_server(g_ntp_servers[i], &serv_addr);
        if (sockfd < 0) {
            continue;
        }
        
        if (send_receive_ntp_packet(sockfd, &serv_addr, &packet) == 0) {
            result = 0;
            break;
        }
        
        close(sockfd);
        sockfd = -1;
    }
    
    // Si ningún servidor funciona, intentar con las IPs de respaldo
    if (result != 0) {
        LOGI("Trying fallback NTP server IPs");
        for (int i = 0; i < g_ntp_fallback_count; i++) {
            LOGI("Trying NTP fallback IP: %s", g_ntp_fallback_ips[i]);
            
            sockfd = connect_to_ntp_ip(g_ntp_fallback_ips[i], &serv_addr);
            if (sockfd < 0) {
                continue;
            }
            
            if (send_receive_ntp_packet(sockfd, &serv_addr, &packet) == 0) {
                result = 0;
                break;
            }
            
            close(sockfd);
            sockfd = -1;
        }
    }
    
    if (result == 0) {
        // Convertir tiempo recibido
        uint32_t txTm = ntohl(packet.xmitts_sec);
        time_t utc_time = ntp_time_to_unix_time(txTm);
        
        LOGI("NTP time received (UTC): %u", (unsigned int)utc_time);
        
        // Usar el offset configurado por el usuario 
        int timezone_offset = g_setting.clock.utc_offset;
        LOGI("Using configured timezone offset: %d seconds", timezone_offset);
        
        // Aplicar la zona horaria para obtener la hora local
        time_t local_time = apply_timezone_offset(utc_time, timezone_offset);
        LOGI("Local time after timezone adjustment: %u", (unsigned int)local_time);
        
        // Actualizar RTC con el tiempo recibido ajustado a zona horaria local
        struct timeval tv_now;
        tv_now.tv_sec = local_time;
        tv_now.tv_usec = 0;
        
        struct rtc_date rd;
        rtc_tv2rd(&tv_now, &rd);
        
        // Validar la fecha
        if (rtc_has_valid_date(&rd) != 0) {
            LOGE("NTP returned invalid date: %04d-%02d-%02d %02d:%02d:%02d", 
                 rd.year, rd.month, rd.day, rd.hour, rd.min, rd.sec);
            result = -1;
        } else if (rd.year < 2023) {  // Validación adicional para asegurar fecha razonable
            LOGE("NTP returned unreasonable date: %04d", rd.year);
            result = -1;
        } else {
            // Actualizar el RTC
            rtc_set_clock(&rd);
            LOGI("NTP time sync successful: %04d-%02d-%02d %02d:%02d:%02d", 
                 rd.year, rd.month, rd.day, rd.hour, rd.min, rd.sec);
            
            // Guardar en settings
            g_setting.clock.year = rd.year;
            g_setting.clock.month = rd.month;
            g_setting.clock.day = rd.day;
            g_setting.clock.hour = rd.hour;
            g_setting.clock.min = rd.min;
            g_setting.clock.sec = rd.sec;
            
            // Actualizar archivo de configuración
            ini_putl("clock", "year", g_setting.clock.year, SETTING_INI);
            ini_putl("clock", "month", g_setting.clock.month, SETTING_INI);
            ini_putl("clock", "day", g_setting.clock.day, SETTING_INI);
            ini_putl("clock", "hour", g_setting.clock.hour, SETTING_INI);
            ini_putl("clock", "min", g_setting.clock.min, SETTING_INI);
            ini_putl("clock", "sec", g_setting.clock.sec, SETTING_INI);
        }
    }
    
cleanup:
    if (sockfd >= 0) {
        close(sockfd);
    }
    
    // Actualizar estado
    pthread_mutex_lock(&g_ntp_mutex);
    g_ntp_sync_state = (result == 0) ? NTP_SYNC_SUCCESS : NTP_SYNC_FAILED;
    pthread_mutex_unlock(&g_ntp_mutex);
    
    // Llamar al callback si existe
    if (callback_data != NULL) {
        if (callback_data->callback_fn != NULL) {
            callback_data->callback_fn(result, callback_data->user_data);
        }
        free(callback_data);
    }
    
    LOGI("NTP sync thread finished with result: %d", result);
    return NULL;
}

// Función pública para verificar si hay una sincronización en progreso
int clock_is_syncing_from_ntp(void) {
    int is_syncing = 0;
    
    pthread_mutex_lock(&g_ntp_mutex);
    is_syncing = (g_ntp_sync_state == NTP_SYNC_IN_PROGRESS);
    pthread_mutex_unlock(&g_ntp_mutex);
    
    return is_syncing;
}

// Función pública para iniciar la sincronización NTP (asíncrona con callback)
int clock_sync_from_ntp_async(ntp_callback_t callback_fn, void* user_data) {
    int ret = -1;
    
    pthread_mutex_lock(&g_ntp_mutex);
    
    // Verificar si ya hay una sincronización en progreso
    if (g_ntp_sync_state == NTP_SYNC_IN_PROGRESS) {
        LOGI("NTP sync already in progress");
        pthread_mutex_unlock(&g_ntp_mutex);
        return -1;
    }
    
    // Actualizar estado
    g_ntp_sync_state = NTP_SYNC_IN_PROGRESS;
    
    // Preparar datos para el callback
    ntp_callback_data_t* callback_data = malloc(sizeof(ntp_callback_data_t));
    if (callback_data == NULL) {
        LOGE("Failed to allocate memory for callback data");
        g_ntp_sync_state = NTP_SYNC_IDLE;
        pthread_mutex_unlock(&g_ntp_mutex);
        return -1;
    }
    
    callback_data->callback_fn = callback_fn;
    callback_data->user_data = user_data;
    
    // Crear hilo para sincronización
    pthread_t thread_id;
    if (pthread_create(&thread_id, NULL, ntp_sync_thread, callback_data) != 0) {
        LOGE("Error creating NTP thread");
        g_ntp_sync_state = NTP_SYNC_IDLE;
        free(callback_data);
        ret = -1;
    } else {
        pthread_detach(thread_id);
        ret = 0;
    }
    
    pthread_mutex_unlock(&g_ntp_mutex);
    return ret;
}

// Función pública para compatibilidad con el código existente (bloqueante)
int clock_sync_from_ntp(void) {
    // Verificar si ya hay una sincronización en progreso
    if (clock_is_syncing_from_ntp()) {
        LOGI("NTP sync already in progress");
        return -1;
    }
    
    // Intentar iniciar sincronización asíncrona sin callback
    if (clock_sync_from_ntp_async(NULL, NULL) != 0) {
        return -1;
    }
    
    // Esperar resultado (pero con timeout)
    int result = -1;
    int timeout_count = 0;
    const int max_timeout = 10; // 10 * 100ms = 1 segundo máximo
    
    while (timeout_count < max_timeout) {
        usleep(100000); // 100ms
        
        pthread_mutex_lock(&g_ntp_mutex);
        if (g_ntp_sync_state == NTP_SYNC_SUCCESS) {
            result = 0;
            g_ntp_sync_state = NTP_SYNC_IDLE;
            pthread_mutex_unlock(&g_ntp_mutex);
            break;
        } else if (g_ntp_sync_state == NTP_SYNC_FAILED) {
            result = -1;
            g_ntp_sync_state = NTP_SYNC_IDLE;
            pthread_mutex_unlock(&g_ntp_mutex);
            break;
        }
        pthread_mutex_unlock(&g_ntp_mutex);
        
        timeout_count++;
    }
    
    // Si aún está en progreso después del timeout, asumimos que continuará en segundo plano
    if (timeout_count >= max_timeout) {
        LOGI("NTP sync still in progress, continuing in background");
    }
    
    return result;
}

// Función pública para obtener el estado de la última sincronización
clock_sync_status_t clock_get_last_sync_status(void) {
    clock_sync_status_t status;
    
    pthread_mutex_lock(&g_ntp_mutex);
    
    switch (g_ntp_sync_state) {
        case NTP_SYNC_IDLE:
            status = CLOCK_SYNC_NONE;
            break;
        case NTP_SYNC_IN_PROGRESS:
            status = CLOCK_SYNC_IN_PROGRESS; 
            break;
        case NTP_SYNC_SUCCESS:
            status = CLOCK_SYNC_SUCCESS;
            break;
        case NTP_SYNC_FAILED:
            status = CLOCK_SYNC_FAILED;
            break;
        default:
            status = CLOCK_SYNC_NONE;
            break;
    }
    
    pthread_mutex_unlock(&g_ntp_mutex);
    
    return status;
}