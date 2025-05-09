#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Sincroniza el reloj del sistema con un servidor NTP.
 * Requiere que el WiFi esté habilitado y configurado.
 *
 * @return 0 si la sincronización fue exitosa, -1 en caso de error
 */
int clock_sync_from_ntp(void);

#ifdef __cplusplus
}
#endif