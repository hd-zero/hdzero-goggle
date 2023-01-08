#include "app_state.h"

app_state_t g_app_state = APP_STATE_MAINMENU;

void app_state_push(app_state_t state) {
    g_app_state = state;
}