#ifndef _MEDIA_H
#define _MEDIA_H

#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint32_t playing_time;
    uint32_t duration;
} media_info_t;

typedef void (*notify_cb_t)(media_info_t *info);

typedef enum {
    PLAYER_START = 0,
    PLAYER_STOP,
    PLAYER_PAUSE,
    PLAYER_SEEK,
    PLAYER_QUIT,
    PLAYER_PANEL_SWITCH,
} player_opt_t;

typedef struct {
    player_opt_t opt;
    uint32_t params;
} player_cmd_t;

typedef struct {
    void *context;
    notify_cb_t notify;
    pthread_t pid;
    bool is_media_thread_exit;
} media_t;

media_t *media_instantiate(char *filename, notify_cb_t notify);
void media_exit(media_t *media);
void media_control(media_t *media, player_cmd_t *cmd);
#endif
