#ifndef NTP_CLIENT_H
#define NTP_CLIENT_H

#ifdef __cplusplus
extern "C" {
#endif

// Error codes
#define NTP_SUCCESS 0
#define NTP_ERR_CONNECT -1 
#define NTP_ERR_TIMEOUT -2
#define NTP_ERR_INVALID -3

/**
 * @brief Callback function type for asynchronous NTP synchronization
 * 
 * @param result Synchronization result (0 = success, -1 = error)
 * @param user_data User data provided in initial call
 */
typedef void (*ntp_callback_t)(int result, void* user_data);

/**
 * @brief Clock sync status values 
 */
typedef enum {
    CLOCK_SYNC_NONE = 0,
    CLOCK_SYNC_IN_PROGRESS,
    CLOCK_SYNC_SUCCESS,
    CLOCK_SYNC_FAILED
} clock_sync_status_t;

/**
 * @brief Inicia la sincronización del reloj con un servidor NTP (bloqueante)
 * 
 * Esta función intenta sincronizar el reloj con un servidor NTP.
 * Es bloqueante pero tiene un timeout interno de 1 segundo.
 * 
 * @return 0 en caso de éxito, -1 en caso de error
 */
int clock_sync_from_ntp(void);

/**
 * @brief Inicia la sincronización del reloj con un servidor NTP (asíncrona)
 * 
 * Esta función inicia la sincronización en un hilo separado y retorna inmediatamente.
 * El resultado se comunicará a través del callback proporcionado.
 * 
 * @param callback_fn Función que será llamada al completar la sincronización (puede ser NULL)
 * @param user_data Datos que serán pasados al callback (puede ser NULL)
 * @return 0 si se inició correctamente, -1 si hubo un error al iniciar
 */
int clock_sync_from_ntp_async(ntp_callback_t callback_fn, void* user_data);

/**
 * @brief Verifica si hay una sincronización NTP en progreso
 * 
 * @return 1 si hay una sincronización en progreso, 0 en caso contrario
 */
int clock_is_syncing_from_ntp(void);

/**
 * @brief Obtiene el estado de la última sincronización
 * 
 * @return Estado de la última sincronización usando clock_sync_status_t
 */
clock_sync_status_t clock_get_last_sync_status(void);

#ifdef __cplusplus
}
#endif

#endif /* NTP_CLIENT_H */