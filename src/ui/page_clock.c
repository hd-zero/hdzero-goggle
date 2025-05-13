#include "page_clock.h"

#include <errno.h>
#include <stdlib.h>
#include <time.h>

#include <log/log.h>
#include <minIni.h>

#include "core/common.hh"
#include "core/settings.h"
#include "driver/rtc.h"
#include "lang/language.h"
#include "ui/page_common.h"
#include "ui/ui_attribute.h"
#include "ui/ui_style.h"
#include "util/ntp_client.h"
#include "ui/page_wifi.h"

/**
 * Various data types used to
 * simplify clock management.
 */
typedef union {
    btn_group_t btn;
    lv_obj_t *obj;
} item_data_t;

typedef enum {
    ITEM_TYPE_OBJ = 0,
    ITEM_TYPE_BTN
} item_type_t;

typedef struct {
    item_type_t type;
    item_data_t data;
    lv_obj_t *panel;
    int last_option;
} item_t;

typedef enum {
    ITEM_YEAR,
    ITEM_MONTH,
    ITEM_DAY,
    ITEM_HOUR,
    ITEM_MINUTE,
    ITEM_SECOND,
    ITEM_FORMAT,
    ITEM_UTC,          // Nuevo elemento para seleccionar UTC
    ITEM_SET_CLOCK,
    ITEM_SYNC_NTP,
    ITEM_BACK,

    ITEM_LIST_TOTAL
} ITEM_LIST;

typedef struct {
    char list[1024];
    char entry[32];
} dropdown_list_t;

typedef struct {
    lv_obj_t *date;
    lv_obj_t *time;
    lv_obj_t *format;
} date_time_t;

/**
 *  Globals
 */
static const int MAX_YEARS_DROPDOWN = 300; // 2023 + 300 == 2323
static lv_coord_t col_dsc[] = {160, 160, 160, 160, 160, 160, LV_GRID_TEMPLATE_LAST};
static lv_coord_t row_dsc[] = {60, 60, 60, 60, 60, 60, 15, 10, 60, 60, 60, LV_GRID_TEMPLATE_LAST};
static item_t page_clock_items[ITEM_LIST_TOTAL] = {0};
static int page_clock_item_selected = ITEM_YEAR;
static int page_clock_item_focused = 0;
static date_time_t page_clock_datetime = {0};
static dropdown_list_t page_clock_options[ITEM_SECOND + 1];
static lv_timer_t *page_clock_set_clock_pending_timer = NULL;
static lv_timer_t *page_clock_refresh_ui_timer = NULL;
static lv_timer_t *page_clock_set_clock_timer = NULL;
static int page_clock_set_clock_confirm = 0;
static int page_clock_is_dirty = 0;
static struct rtc_date page_clock_rtc_date = {0};

// Opciones de zona horaria UTC
static char* utc_options[] = {
    "UTC-12:00", "UTC-11:00", "UTC-10:00", "UTC-09:00", "UTC-08:00",
    "UTC-07:00", "UTC-06:00", "UTC-05:00", "UTC-04:00", "UTC-03:00",
    "UTC-02:00", "UTC-01:00", "UTC 00:00", "UTC+01:00", "UTC+02:00",
    "UTC+03:00", "UTC+04:00", "UTC+05:00", "UTC+06:00", "UTC+07:00",
    "UTC+08:00", "UTC+09:00", "UTC+10:00", "UTC+11:00", "UTC+12:00",
    "UTC+13:00", "UTC+14:00"
};

// Valores de offset en segundos correspondientes a las opciones
static const int utc_seconds[] = {
-43200,  // UTC−12:00
-39600,  // UTC−11:00
-36000,  // UTC−10:00
-32400,  // UTC−09:00
-28800,  // UTC−08:00
-25200,  // UTC−07:00
-21600,  // UTC−06:00
-18000,  // UTC−05:00
-14400,  // UTC−04:00
-10800,  // UTC−03:00
 -7200,  // UTC−02:00
 -3600,  // UTC−01:00
     0,  // UTC±00:00
  3600,  // UTC+01:00
  7200,  // UTC+02:00
 10800,  // UTC+03:00
 14400,  // UTC+04:00
 18000,  // UTC+05:00
 21600,  // UTC+06:00
 25200,  // UTC+07:00
 28800,  // UTC+08:00
 32400,  // UTC+09:00
 36000,  // UTC+10:00
 39600,  // UTC+11:00
 43200,  // UTC+12:00
 46800,  // UTC+13:00
 50400   // UTC+14:00
};

// Convertir offset en segundos a índice en el array
int utc_offset_to_index(int offset_seconds) {
    // Buscar el offset más cercano
    int index = 12; // Por defecto UTC±00:00
    int min_diff = abs(offset_seconds);
    
    for (int i = 0; i < sizeof(utc_seconds)/sizeof(utc_seconds[0]); i++) {
        int diff = abs(offset_seconds - utc_seconds[i]);
        if (diff < min_diff) {
            min_diff = diff;
            index = i;
        }
    }
    
    return index;
}

// Convertir índice a offset en segundos
int index_to_utc_offset(int index) {
    if (index >= 0 && index < sizeof(utc_seconds)/sizeof(utc_seconds[0])) {
        return utc_seconds[index];
    }
    
    return 0; // Por defecto UTC±00:00
}

/**
 * Callback para sincronización NTP asíncrona
 */
static void ntp_sync_callback(int result, void* user_data) {
    if (result == 0) {
        // Éxito, actualizar la interfaz en el hilo principal
        // Usamos lv_async_call o enviamos un mensaje al hilo principal
        // Esto dependerá de la implementación específica del sistema
        
        // En este caso, el UI se actualizará automáticamente en el próximo ciclo
        // porque refrescamos la fecha/hora cada 250ms
    }
    
    // El mensaje de éxito/error se mostrará directamente en la interfaz
    // cuando el usuario inicia la sincronización
}

/**
 * Acquire index from selected dropdown option as a string.
 */
static int page_clock_get_dropdown_index(ITEM_LIST item, const char *sel_opt) {
    const char *start = &page_clock_options[item].list[0];
    const char *match = strstr(start, sel_opt);

    if (!match) {
        return -1;
    } else {
        int option = 0;
        for (int i = 0; i < (match - start); ++i) {
            if (start[i] == '\n') {
                ++option;
            }
        }

        return option;
    }
}

/**
 * Generic util to generate the content (options) of a dropdown type.
 */
static void page_clock_build_options(dropdown_list_t *options, int total_entries, int offset, int selected) {
    for (int i = 0, j = 0; i < total_entries; ++i) {
        if (i != 0) {
            j += snprintf(&options->list[j], sizeof(options->list) - j, "\n");
        }
        j += snprintf(&options->list[j], sizeof(options->list) - j, "%02d", i + offset);
    }

    snprintf(options->entry, sizeof(options->entry), "%d", selected);
}

/**
 * Populates all dropdown fields. Leap Year is automatically derived
 * and when Month is manipulated, the number of days are recalculated.
 */
static void page_clock_build_options_from_date(struct rtc_date *date) {
    int ndays = rtc_days_per_month(date->year, date->month - 1);
    date->day = ndays < date->day ? ndays : date->day;

    page_clock_build_options(&page_clock_options[ITEM_YEAR], MAX_YEARS_DROPDOWN, 2023, date->year);
    page_clock_build_options(&page_clock_options[ITEM_MONTH], 12, 1, date->month);
    page_clock_build_options(&page_clock_options[ITEM_DAY], ndays, 1, date->day);
    page_clock_build_options(&page_clock_options[ITEM_HOUR], 24, 0, date->hour);
    page_clock_build_options(&page_clock_options[ITEM_MINUTE], 60, 0, date->min);
    page_clock_build_options(&page_clock_options[ITEM_SECOND], 60, 0, date->sec);

    for (int i = 0; i < ITEM_FORMAT; ++i) {
        if (page_clock_items[i].data.obj) {
            lv_dropdown_set_options(page_clock_items[i].data.obj, page_clock_options[i].list);
            lv_dropdown_set_selected(page_clock_items[i].data.obj, page_clock_get_dropdown_index(i, page_clock_options[i].entry));
        }
    }
}

/**
 * Create a dropdown object and populate options.
 */
static void page_clock_create_dropdown(lv_obj_t *parent,
                                       ITEM_LIST item,
                                       int option,
                                       int col,
                                       int row) {
    char text[512];
    snprintf(text, sizeof(text), "%d", option);

    page_clock_items[item].data.obj = create_dropdown_item(parent, page_clock_options[item].list, col, row, 160, 40, 1, 4, LV_GRID_ALIGN_START, &lv_font_montserrat_26);
    page_clock_items[item].type = ITEM_TYPE_OBJ;

    int index = page_clock_get_dropdown_index(item, text);
    if (index != -1) {
        lv_dropdown_set_selected(page_clock_items[item].data.obj, index);
    }
}

/**
 * Util used for initializing objects held within the datetime_t structure.
 */
static lv_obj_t *page_clock_create_datetime_item_attr(lv_obj_t *parent, int row) {
    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, "");
    lv_obj_set_style_text_font(label, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_RIGHT, 0);
    lv_obj_set_style_text_color(label, lv_color_make(255, 255, 255), 0);
    lv_obj_set_style_pad_top(label, 8, 0);
    lv_obj_set_style_pad_left(label, 0, 0);
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_obj_set_grid_cell(label, LV_GRID_ALIGN_START, 1, 4, LV_GRID_ALIGN_START, row, 2);
    return label;
}

/**
 * Create a datetime_t object and initialize their respective fields.
 */
static void page_clock_create_datetime_item(lv_obj_t *parent, int row) {
    page_clock_datetime.date = page_clock_create_datetime_item_attr(parent, row);
    lv_obj_set_style_pad_left(page_clock_datetime.date, 0, 0);
    page_clock_datetime.time = page_clock_create_datetime_item_attr(parent, row);
    lv_obj_set_style_pad_left(page_clock_datetime.time, 100, 0);
    page_clock_datetime.format = page_clock_create_datetime_item_attr(parent, row);
    lv_obj_set_style_pad_left(page_clock_datetime.format, 170, 0);
}

/**
 * Clear all visible fields from datetime_t objects.
 */
static void page_clock_clear_datetime() {
    lv_label_set_text(page_clock_datetime.date, "");
    lv_label_set_text(page_clock_datetime.time, "");
    lv_label_set_text(page_clock_datetime.format, "");
}

/**
 * Display the YYYY/MM/DD HH:MM:SS FMT within the menu.
 */
static void page_clock_refresh_datetime() {
    char text[128];
    struct rtc_date date;
    rtc_get_clock(&date);

    snprintf(text, sizeof(text), "%04d/%02d/%02d", date.year, date.month, date.day);
    lv_label_set_text(page_clock_datetime.date, text);

    int hour = date.hour;
    if (g_setting.clock.format == 0) {
        if (hour > 12) {
            hour -= 12;
        }
        hour = hour == 0 ? 12 : hour;
    }
    snprintf(text, sizeof(text), "%02d:%02d:%02d", hour, date.min, date.sec);
    lv_label_set_text(page_clock_datetime.time, text);
    snprintf(text, sizeof(text), "%s", g_setting.clock.format == 0 ? (date.hour > 11 ? _lang("PM") : _lang("AM")) : "");
    lv_label_set_text(page_clock_datetime.format, text);
}

/**
 * Dynamically update the UI elements as the user manipulates the page.
 */
static void page_clock_refresh_styles() {
    for (int i = 0; i < ITEM_LIST_TOTAL; ++i) {
        if (page_clock_items[i].panel) {
            lv_obj_add_flag(page_clock_items[i].panel, LV_OBJ_FLAG_HIDDEN);
        }

        switch (page_clock_items[i].type) {
        case ITEM_TYPE_OBJ:
            lv_obj_remove_style(page_clock_items[i].data.obj, &style_dropdown, LV_PART_MAIN);
            break;
        }
    }
}

/**
 * Set the 'Set Clock' text back to it's initial text and colored state.
 */
static void page_clock_set_clock_reset() {
    lv_label_set_text(page_clock_items[ITEM_SET_CLOCK].data.obj, _lang("Set Clock"));
    page_clock_set_clock_confirm = 0;
}

/**
 * Reset the 'Sync from Internet' text back to its initial state.
 */
static void page_clock_sync_reset_cb(struct _lv_timer_t *timer) {
    lv_label_set_text(page_clock_items[ITEM_SYNC_NTP].data.obj, _lang("Sync from Internet"));
}

/**
 * Determine if the type of object selected is a lv_obj_t.
 */
static int page_clock_selected_item_is_obj(int selected_item) {
    switch (selected_item) {
    case ITEM_YEAR:
    case ITEM_MONTH:
    case ITEM_DAY:
    case ITEM_HOUR:
    case ITEM_MINUTE:
    case ITEM_SECOND:
    case ITEM_UTC:
        return 1;
    default:
        return 0;
    }
}

/**
 * Populate rtc_date_t based on user selections.
 */
static void page_clock_build_date_from_selected(struct rtc_date *date) {
    char text[512];

    lv_dropdown_get_selected_str(page_clock_items[ITEM_YEAR].data.obj, text, sizeof(text));
    date->year = atoi(text);
    lv_dropdown_get_selected_str(page_clock_items[ITEM_MONTH].data.obj, text, sizeof(text));
    date->month = atoi(text);
    lv_dropdown_get_selected_str(page_clock_items[ITEM_DAY].data.obj, text, sizeof(text));
    date->day = atoi(text);
    lv_dropdown_get_selected_str(page_clock_items[ITEM_HOUR].data.obj, text, sizeof(text));
    date->hour = atoi(text);
    lv_dropdown_get_selected_str(page_clock_items[ITEM_MINUTE].data.obj, text, sizeof(text));
    date->min = atoi(text);
    lv_dropdown_get_selected_str(page_clock_items[ITEM_SECOND].data.obj, text, sizeof(text));
    date->sec = atoi(text);
}

/**
 * Callback timer when changes are pending, `Set Clock` will glow RED.
 */
static void page_clock_set_clock_pending_cb(struct _lv_timer_t *timer) {
    if (page_clock_is_dirty && !page_clock_set_clock_confirm) {
        static int dir = 20;
        static char text[128];
        static uint8_t red = 150;
        snprintf(text, sizeof(text), "#%02x0000 %s#", red, _lang("Set Clock"));
        lv_label_set_text(page_clock_items[ITEM_SET_CLOCK].data.obj, text);
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
 * Callback timer for refreshing the date/time/format wihtin the UI.
 */
static void page_clock_refresh_ui_timer_cb(struct _lv_timer_t *timer) {
    page_clock_refresh_datetime();
    
    // Actualizar estado del botón Sync from Internet
    bool wifi_connected = page_wifi_is_sta_connected();
    if (wifi_connected) {
        lv_obj_clear_state(page_clock_items[ITEM_SYNC_NTP].data.obj, LV_STATE_DISABLED);
        lv_obj_set_style_text_color(page_clock_items[ITEM_SYNC_NTP].data.obj, lv_color_make(255, 255, 255), 0);
    } else {
        lv_obj_add_state(page_clock_items[ITEM_SYNC_NTP].data.obj, LV_STATE_DISABLED);
        lv_obj_set_style_text_color(page_clock_items[ITEM_SYNC_NTP].data.obj, lv_color_make(128, 128, 128), 0);
    }
}

/**
 * Callback invoked once `Set Clock` is triggered and confirmed via the menu.
 */
static void page_clock_set_clock_timer_cb(struct _lv_timer_t *timer) {
    page_clock_build_date_from_selected(&page_clock_rtc_date);

    g_setting.clock.year = page_clock_rtc_date.year;
    g_setting.clock.month = page_clock_rtc_date.month;
    g_setting.clock.day = page_clock_rtc_date.day;
    g_setting.clock.hour = page_clock_rtc_date.hour;
    g_setting.clock.min = page_clock_rtc_date.min;
    g_setting.clock.sec = page_clock_rtc_date.sec;

    // Update settings
    ini_putl("clock", "year", g_setting.clock.year, SETTING_INI);
    ini_putl("clock", "month", g_setting.clock.month, SETTING_INI);
    ini_putl("clock", "day", g_setting.clock.day, SETTING_INI);
    ini_putl("clock", "hour", g_setting.clock.hour, SETTING_INI);
    ini_putl("clock", "min", g_setting.clock.min, SETTING_INI);
    ini_putl("clock", "sec", g_setting.clock.sec, SETTING_INI);

    rtc_set_clock(&page_clock_rtc_date);
    page_clock_refresh_datetime();
    page_clock_set_clock_reset();
    page_clock_is_dirty = 0;
}

/**
 * Main allocation routine for this page.
 */
static lv_obj_t *page_clock_create(lv_obj_t *parent, panel_arr_t *arr) {
    char buf[256];
    rtc_get_clock(&page_clock_rtc_date);
    page_clock_build_options_from_date(&page_clock_rtc_date);

    // Incrementar el tamaño de las filas para evitar solapamiento
    static lv_coord_t col_dsc[] = {160, 200, 160, 200, 160, 160, LV_GRID_TEMPLATE_LAST};
    static lv_coord_t row_dsc[] = {60, 60, 60, 60, 60, 60, 60, 15, 10, 60, 60, LV_GRID_TEMPLATE_LAST};

    lv_obj_t *page = lv_menu_page_create(parent, NULL);
    lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(page, 1053, 900);
    lv_obj_add_style(page, &style_subpage, LV_PART_MAIN);
    lv_obj_set_style_pad_top(page, 94, 0);

    lv_obj_t *section = lv_menu_section_create(page);
    lv_obj_add_style(section, &style_submenu, LV_PART_MAIN);
    lv_obj_set_size(section, 1053, 894);

    snprintf(buf, sizeof(buf), "%s:", _lang("Clock"));
    create_text(NULL, section, false, buf, LV_MENU_ITEM_BUILDER_VARIANT_2);

    lv_obj_t *cont = lv_obj_create(section);
    // Aumentar el tamaño del contenedor
    lv_obj_set_size(cont, 1280, 900);
    lv_obj_set_pos(cont, 0, 0);
    lv_obj_set_layout(cont, LV_LAYOUT_GRID);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(cont, &style_context, LV_PART_MAIN);

    lv_obj_set_style_grid_column_dsc_array(cont, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(cont, row_dsc, 0);

    create_select_item(arr, cont);

    // Current date/time or last saved setting.
    page_clock_create_dropdown(cont, ITEM_YEAR, page_clock_rtc_date.year, 1, 0);
    page_clock_create_dropdown(cont, ITEM_MONTH, page_clock_rtc_date.month, 2, 0);
    page_clock_create_dropdown(cont, ITEM_DAY, page_clock_rtc_date.day, 3, 0);
    page_clock_create_dropdown(cont, ITEM_HOUR, page_clock_rtc_date.hour, 1, 1);
    page_clock_create_dropdown(cont, ITEM_MINUTE, page_clock_rtc_date.min, 2, 1);
    page_clock_create_dropdown(cont, ITEM_SECOND, page_clock_rtc_date.sec, 3, 1);

    // Format selection (AM/PM or 24H) - Ahora en una fila dedicada
    snprintf(buf, sizeof(buf), "%s/%s", _lang("AM"), _lang("PM"));
    create_btn_group_item(&page_clock_items[ITEM_FORMAT].data.btn, cont, 2, _lang("Format"), buf, _lang("24 Hour"), "", "", 2);
    page_clock_items[ITEM_FORMAT].type = ITEM_TYPE_BTN;
    page_clock_items[ITEM_FORMAT].panel = arr->panel[2];
    btn_group_set_sel(&page_clock_items[ITEM_FORMAT].data.btn, g_setting.clock.format);

    // Time Zone label y dropdown - En una nueva fila
    create_label_item(cont, _lang("Time Zone"), 1, 3, 1);
    
    lv_obj_t* utc_dropdown = lv_dropdown_create(cont);
    lv_dropdown_clear_options(utc_dropdown);
    for (int i = 0; i < sizeof(utc_options)/sizeof(utc_options[0]); i++) {
        lv_dropdown_add_option(utc_dropdown, utc_options[i], LV_DROPDOWN_POS_LAST);
    }
    lv_obj_set_size(utc_dropdown, 200, 40);
    lv_obj_set_grid_cell(utc_dropdown, LV_GRID_ALIGN_START, 2, 2, LV_GRID_ALIGN_START, 3, 1);
    lv_dropdown_set_selected(utc_dropdown, utc_offset_to_index(g_setting.clock.utc_offset));

    page_clock_items[ITEM_UTC].data.obj = utc_dropdown;
    page_clock_items[ITEM_UTC].type = ITEM_TYPE_OBJ;
    page_clock_items[ITEM_UTC].panel = arr->panel[3];

    // Set Clock (ahora en fila 4)
    page_clock_items[ITEM_SET_CLOCK].data.obj = create_label_item(cont, _lang("Set Clock"), 1, 4, 3);
    page_clock_items[ITEM_SET_CLOCK].type = ITEM_TYPE_BTN;
    page_clock_items[ITEM_SET_CLOCK].panel = arr->panel[4];

    // Sync from Internet (ahora en fila 5)
    page_clock_items[ITEM_SYNC_NTP].data.obj = create_label_item(cont, _lang("Sync from Internet"), 1, 5, 3);
    page_clock_items[ITEM_SYNC_NTP].type = ITEM_TYPE_BTN;
    page_clock_items[ITEM_SYNC_NTP].panel = arr->panel[5];

    // Back (ahora en fila 6)
    snprintf(buf, sizeof(buf), "< %s", _lang("Back"));
    page_clock_items[ITEM_BACK].data.obj = create_label_item(cont, buf, 1, 6, 1);
    page_clock_items[ITEM_BACK].type = ITEM_TYPE_BTN;
    page_clock_items[ITEM_BACK].panel = arr->panel[6];

    // Fecha/hora actual (ahora en fila 7)
    page_clock_create_datetime_item(cont, 7);

    if (rtc_has_battery() != 0) {
        lv_obj_t *note = lv_label_create(cont);
        snprintf(buf, sizeof(buf), "*%s.", _lang("Battery not installed or clock not configured"));
        lv_label_set_text(note, buf);
        lv_obj_set_style_text_font(note, &lv_font_montserrat_16, 0);
        lv_obj_set_style_text_align(note, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_style_text_color(note, lv_color_make(255, 255, 255), 0);
        lv_obj_set_style_pad_top(note, 12, 0);
        lv_label_set_long_mode(note, LV_LABEL_LONG_WRAP);
        lv_obj_set_grid_cell(note, LV_GRID_ALIGN_START, 1, 4, LV_GRID_ALIGN_START, 8, 2);
    }

    page_clock_clear_datetime();

    return page;
}

/**
 * Main entry routine for this page.
 */
static void page_clock_enter() {
    page_clock_build_options_from_date(&page_clock_rtc_date);
    page_clock_refresh_datetime();
    lv_obj_add_style(page_clock_items[ITEM_YEAR].data.obj, &style_dropdown, LV_PART_MAIN);
    page_clock_refresh_ui_timer = lv_timer_create(page_clock_refresh_ui_timer_cb, 250, NULL);
    lv_timer_set_repeat_count(page_clock_refresh_ui_timer, -1);
}

/**
 * Main exit routine for this page.
 */
static void page_clock_exit() {
    if (page_clock_refresh_ui_timer) {
        lv_timer_del(page_clock_refresh_ui_timer);
        page_clock_refresh_ui_timer = NULL;
    }

    page_clock_set_clock_reset();
    page_clock_refresh_styles();
    page_clock_clear_datetime();
    page_clock_build_options_from_date(&page_clock_rtc_date);

    for (int i = 0; i < ITEM_LIST_TOTAL; ++i) {
        switch (page_clock_items[i].type) {
        case ITEM_TYPE_OBJ:
            lv_dropdown_close(page_clock_items[i].data.obj);
        }
    }

    page_clock_is_dirty = 0;
    page_clock_item_focused = 0;
    page_clock_set_clock_confirm = 0;
    page_clock_item_selected = ITEM_YEAR;
}

/**
 * Main navigation routine for this page.
 */
static void page_clock_on_roller(uint8_t key) {
    // Ignore commands until timer has expired before allowing user to proceed.
    if (page_clock_set_clock_confirm == 2) {
        return;
    }

    // If a click was not previous pressed to confirm, then update is canceled.
    page_clock_set_clock_confirm = 0;

    page_clock_refresh_styles();

    if (!page_clock_is_dirty) {
        page_clock_set_clock_reset();
    }

    if (key == DIAL_KEY_UP) {
        if (!page_clock_item_focused) {
            if (++page_clock_item_selected >= ITEM_LIST_TOTAL) {
                page_clock_item_selected = ITEM_YEAR;
            }
        } else if (page_clock_selected_item_is_obj(page_clock_item_selected)) {
            uint32_t evt = LV_KEY_DOWN;
            lv_event_send(page_clock_items[page_clock_item_selected].data.obj, LV_EVENT_KEY, &evt);
        }
    } else if (key == DIAL_KEY_DOWN) {
        if (!page_clock_item_focused) {
            if (--page_clock_item_selected < ITEM_YEAR) {
                page_clock_item_selected = ITEM_LIST_TOTAL - 1;
            }
        } else if (page_clock_selected_item_is_obj(page_clock_item_selected)) {
            uint32_t evt = LV_KEY_UP;
            lv_event_send(page_clock_items[page_clock_item_selected].data.obj, LV_EVENT_KEY, &evt);
        }
    }

    if (page_clock_items[page_clock_item_selected].panel) {
        lv_obj_clear_flag(page_clock_items[page_clock_item_selected].panel, LV_OBJ_FLAG_HIDDEN);
    }

    switch (page_clock_items[page_clock_item_selected].type) {
    case ITEM_TYPE_OBJ:
        if (page_clock_items[page_clock_item_selected].data.obj->class_p == &lv_dropdown_class) {
            lv_obj_add_style(page_clock_items[page_clock_item_selected].data.obj, &style_dropdown, LV_PART_MAIN);
        }
        break;
    }
}

/**
 * Main input selection routine for this page.
 */
static void ntp_sync_check_timer_cb(lv_timer_t* timer) {
    if (!clock_is_syncing_from_ntp()) {
        clock_sync_status_t sync_status = clock_get_last_sync_status();
        
        switch(sync_status) {
            case CLOCK_SYNC_SUCCESS: {
                // Get updated time after successful sync
                struct rtc_date date;
                rtc_get_clock(&date);
                
                // Update screen date
                page_clock_rtc_date = date;
                page_clock_build_options_from_date(&page_clock_rtc_date);
                page_clock_refresh_datetime();
                
                // Save to settings
                g_setting.clock.year = date.year;
                g_setting.clock.month = date.month;
                g_setting.clock.day = date.day;
                g_setting.clock.hour = date.hour;
                g_setting.clock.min = date.min;
                g_setting.clock.sec = date.sec;
                
                // Update configuration file
                ini_putl("clock", "year", g_setting.clock.year, SETTING_INI);
                ini_putl("clock", "month", g_setting.clock.month, SETTING_INI);
                ini_putl("clock", "day", g_setting.clock.day, SETTING_INI);
                ini_putl("clock", "hour", g_setting.clock.hour, SETTING_INI);
                ini_putl("clock", "min", g_setting.clock.min, SETTING_INI);
                ini_putl("clock", "sec", g_setting.clock.sec, SETTING_INI);
                
                lv_label_set_text(page_clock_items[ITEM_SYNC_NTP].data.obj, "#00FF00 Sync Complete#");
                break;
            }
            
            case CLOCK_SYNC_FAILED:
                lv_label_set_text(page_clock_items[ITEM_SYNC_NTP].data.obj, "#FF0000 Sync Failed#");
                break;
                
            default:
                lv_label_set_text(page_clock_items[ITEM_SYNC_NTP].data.obj, "#FF0000 Sync Error#");
                break;
        }
        
        // Create timer to restore text
        lv_timer_t *reset_timer = lv_timer_create(page_clock_sync_reset_cb, 2000, NULL);
        lv_timer_set_repeat_count(reset_timer, 1);
        
        // Delete this timer
        lv_timer_del(timer);
    }
}

static void page_clock_on_click(uint8_t key, int sel) {
    char buf[128];
    // Ignore commands until timer has expired before allowing user to proceed.
    if (page_clock_set_clock_confirm == 2) {
        return;
    }

    switch (page_clock_item_selected) {
    case ITEM_FORMAT:
        btn_group_toggle_sel(&page_clock_items[ITEM_FORMAT].data.btn);
        g_setting.clock.format = btn_group_get_sel(&page_clock_items[ITEM_FORMAT].data.btn);
        ini_putl("clock", "format", g_setting.clock.format, SETTING_INI);
        break;
    case ITEM_UTC:
        if (!page_clock_item_focused) {
            page_clock_items[ITEM_UTC].last_option =
                lv_dropdown_get_selected(page_clock_items[ITEM_UTC].data.obj);

            lv_obj_t *list = lv_dropdown_get_list(page_clock_items[ITEM_UTC].data.obj);
            lv_dropdown_open(page_clock_items[ITEM_UTC].data.obj);
            lv_obj_add_style(list, &style_dropdown, LV_PART_MAIN);
            lv_obj_set_style_text_color(list, lv_color_make(0, 0, 0), LV_PART_SELECTED | LV_STATE_CHECKED);
            page_clock_item_focused = 1;
        } else {
            lv_event_send(page_clock_items[ITEM_UTC].data.obj, LV_EVENT_RELEASED, NULL);
            page_clock_item_focused = 0;
            int option = lv_dropdown_get_selected(page_clock_items[ITEM_UTC].data.obj);

            if (page_clock_items[ITEM_UTC].last_option != option) {
                page_clock_items[ITEM_UTC].last_option = option;
                g_setting.clock.utc_offset = index_to_utc_offset(option);
                ini_putl("clock", "utc_offset", g_setting.clock.utc_offset, SETTING_INI);
                page_clock_is_dirty = 1;
                
                if (!page_clock_set_clock_pending_timer) {
                    page_clock_set_clock_pending_timer = lv_timer_create(page_clock_set_clock_pending_cb, 50, NULL);
                    lv_timer_set_repeat_count(page_clock_set_clock_pending_timer, -1);
                }
            }
        }
        break;
    case ITEM_SET_CLOCK:
        if (page_clock_set_clock_confirm) {
            snprintf(buf, sizeof(buf), "#FF0000 %s %s...#", _lang("Updating"), _lang("Clock"));
            lv_label_set_text(page_clock_items[ITEM_SET_CLOCK].data.obj, buf);
            page_clock_set_clock_timer = lv_timer_create(page_clock_set_clock_timer_cb, 1000, NULL);
            lv_timer_set_repeat_count(page_clock_set_clock_timer, 1);
            page_clock_set_clock_confirm = 2;
        } else {
            snprintf(buf, sizeof(buf), "#FFFF00 %s...#", _lang("Click to confirm or Scroll to cancel"));
            lv_label_set_text(page_clock_items[ITEM_SET_CLOCK].data.obj, buf);
            page_clock_set_clock_confirm = 1;
        }
        break;
    case ITEM_SYNC_NTP:
        // Verificar si el WiFi está habilitado y conectado
        if (!page_wifi_is_sta_connected()) {
            lv_label_set_text(page_clock_items[ITEM_SYNC_NTP].data.obj, "#FF0000 WiFi Client Mode Required#");
            lv_timer_t *reset_timer = lv_timer_create(page_clock_sync_reset_cb, 2000, NULL);
            lv_timer_set_repeat_count(reset_timer, 1);
            return;
        }

        if (page_clock_set_clock_confirm) {
            snprintf(buf, sizeof(buf), "#FF0000 %s %s...#", _lang("Syncing"), _lang("Clock"));
            lv_label_set_text(page_clock_items[ITEM_SYNC_NTP].data.obj, buf);
            
            if (clock_sync_from_ntp_async(ntp_sync_callback, NULL) == 0) {
                lv_timer_t *check_timer = lv_timer_create(ntp_sync_check_timer_cb, 500, NULL);
                lv_timer_set_repeat_count(check_timer, 20); // Timeout after 10 seconds
            } else {
                snprintf(buf, sizeof(buf), "#FF0000 %s#", _lang("Sync Failed"));
                lv_label_set_text(page_clock_items[ITEM_SYNC_NTP].data.obj, buf);
                lv_timer_t *reset_timer = lv_timer_create(page_clock_sync_reset_cb, 2000, NULL);
                lv_timer_set_repeat_count(reset_timer, 1);
            }
            page_clock_set_clock_confirm = 0;
        } else {
            snprintf(buf, sizeof(buf), "#FFFF00 %s...#", _lang("Click to confirm or Scroll to cancel"));
            lv_label_set_text(page_clock_items[ITEM_SYNC_NTP].data.obj, buf);
            page_clock_set_clock_confirm = 1;
        }
        break;
    case ITEM_BACK:
        submenu_exit();
        break;
    default:
        if (!page_clock_item_focused) {
            page_clock_items[page_clock_item_selected].last_option =
                lv_dropdown_get_selected(page_clock_items[page_clock_item_selected].data.obj);

            lv_obj_t *list = lv_dropdown_get_list(page_clock_items[page_clock_item_selected].data.obj);
            lv_dropdown_open(page_clock_items[page_clock_item_selected].data.obj);
            lv_obj_add_style(list, &style_dropdown, LV_PART_MAIN);
            lv_obj_set_style_text_color(list, lv_color_make(0, 0, 0), LV_PART_SELECTED | LV_STATE_CHECKED);
            page_clock_item_focused = 1;
        } else {
            lv_event_send(page_clock_items[page_clock_item_selected].data.obj, LV_EVENT_RELEASED, NULL);
            page_clock_item_focused = 0;
            int option = lv_dropdown_get_selected(page_clock_items[page_clock_item_selected].data.obj);

            if (page_clock_items[page_clock_item_selected].last_option != option) {
                page_clock_items[page_clock_item_selected].last_option = option;
                page_clock_is_dirty = 1;
                if (!page_clock_set_clock_pending_timer) {
                    page_clock_set_clock_pending_timer = lv_timer_create(page_clock_set_clock_pending_cb, 50, NULL);
                    lv_timer_set_repeat_count(page_clock_set_clock_pending_timer, -1);
                }
            }

            if (page_clock_item_selected == ITEM_YEAR ||
                page_clock_item_selected == ITEM_MONTH) {
                struct rtc_date date;
                page_clock_build_date_from_selected(&date);
                page_clock_build_options_from_date(&date);
            }
        }
        break;
    }
}

/**
* Main Menu page data structure, notice max is set to zero
* in order to allow us to override default user input logic.
*/
page_pack_t pp_clock = {
   .p_arr = {
       .cur = 0,
       .max = 0,
   },
   .name = "Clock",
   .create = page_clock_create,
   .enter = page_clock_enter,
   .exit = page_clock_exit,
   .on_created = NULL,
   .on_update = NULL,
   .on_roller = page_clock_on_roller,
   .on_click = page_clock_on_click,
   .on_right_button = NULL,
};