#include "./lv_i18n.h"


////////////////////////////////////////////////////////////////////////////////
// Define plural operands
// http://unicode.org/reports/tr35/tr35-numbers.html#Operands

// Integer version, simplified

#define UNUSED(x) (void)(x)

static inline uint32_t op_n(int32_t val) { return (uint32_t)(val < 0 ? -val : val); }
static inline uint32_t op_i(uint32_t val) { return val; }
// always zero, when decimal part not exists.
static inline uint32_t op_v(uint32_t val) { UNUSED(val); return 0;}
static inline uint32_t op_w(uint32_t val) { UNUSED(val); return 0; }
static inline uint32_t op_f(uint32_t val) { UNUSED(val); return 0; }
static inline uint32_t op_t(uint32_t val) { UNUSED(val); return 0; }

static lv_i18n_phrase_t en_gb_singulars[] = {
    {"source", "Source"},
    {"auto_scan", "Auto Scan"},
    {"on", "On"},
    {"last", "Last"},
    {"off", "Off"},
    {"default", "Default"},
    {"hdzero", "HDZero"},
    {"expansion", "Expansion"},
    {"av_in", "AV in"},
    {"hdmi_in", "HDMI in"},
    {"back", "< Back"},
    {"set_clock", "Set Clock"},
    {"clock", "Clock"},
    {"format", "Format"},
    {"24_hour", "24 hour"},
    {"send_vtx", "Send VTX"},
    {"click_to_start", "Click to start"},
    {"bind", "Bind"},
    {"starting", "Starting..."},
    {"binding", "Binding"},
    {"fans", "Fans"},
    {"auto_control", "Auto Control"},
    {"top_fan", "Top Fan"},
    {"side_fans", "Side Fans"},
    {"set_alarm_angle", "Set Alarm Angle"},
    {"head_tracker", "Head tracker"},
    {"page", "Page"},
    {"tracking", "Tracking"},
    {"tilt_alarm", "Tilt Alarm"},
    {"alarm", "Alarm"},
    {"video", "Video"},
    {"arm", "Arm"},
    {"calibrate", "Calibrate"},
    {"set_center", "Set Center"},
    {"max_angle", "Max Angle"},
    {"pan", "Pan"},
    {"tilt", "Tilt"},
    {"roll", "Roll"},
    {"calibrating", "Calibrating..."},
    {"re_calibrating", "Re-Calibrate"},
    {"updating_angle", "Updating Angle..."},
    {"oled", "OLED"},
    {"brightness", "Brightness"},
    {"saturation", "Saturation"},
    {"contrast", "Contrast"},
    {"oled_auto_off", "Oled Auto off"},
    {"adjust_osd_elements", "Adjust OSD Elements"},
    {"osd_orbit", "OSD Orbit"},
    {"min", "Min"},
    {"max", "Max"},
    {"osd_mode", "OSD Mode"},
    {"at_startup", "At Startup"},
    {"show", "Show"},
    {"hide", "Hide"},
    {"playback", "Playback"},
    {"power", "Power"},
    {"battery", "Battery"},
    {"cell_count_mode", "Cell Count Mode"},
    {"auto", "Auto"},
    {"manual", "Manual"},
    {"cell_count", "Cell count"},
    {"warning_cell_voltage", "Warning Cell Voltage"},
    {"voltage_calibration", "Voltage Calibration"},
    {"display_mode", "Display Mode"},
    {"total", "Total"},
    {"cell_average", "Cell avg."},
    {"warning_type", "Warning Type"},
    {"beep", "Beep"},
    {"visual", "Visual"},
    {"both", "Both"},
    {"analogrx_power", "AnalogRX power"},
    {"record_mode", "Record Mode"},
    {"record_format", "Record Format"},
    {"record_osd", "Record OSD"},
    {"yes", "Yes"},
    {"no", "No"},
    {"record_audio", "Record Audio"},
    {"audio_source", "Audio Source"},
    {"mic", "Mic"},
    {"line_in", "Line in"},
    {"a/v_in", "A/V in"},
    {"naming_scheme", "Naming Scheme"},
    {"digits", "Digits"},
    {"date", "Date"},
    {"scan_ready", "Scan ready"},
    {"scanning", "Scanning..."},
    {"scanning_done", "Scanning done"},
    {"scanning_ready", "Scanning ready"},
    {"go_sleep", "Go Sleep!"},
    {"expansion_module", "Expansion Module"},
    {"analog_video", "Analog Video"},
    {"hdzero_band", "HDZero Band"},
    {"raceband", "Raceband"},
    {"lowband", "Lowband"},
    {"hdzero_bw", "HDZero BW"},
    {"wide", "Wide"},
    {"narrow", "Narrow"},
    {"oled_pattern_normal", "OLED pattern normal"},
    {"format_sd_card", "Format SD Card"},
    {"repair_sd_card", "Repair SD Card"},
    {"storage", "Storage"},
    {"logging", "Logging"},
    {"firmware", "Firmware"},
    {"current_version", "Current Version"},
    {"reset_all_settings", "Reset all settings"},
    {"update_vtx", "Update VTX"},
    {"update_goggle", "Update Goggle"},
    {"update_esp32", "Update ESP32"},
    {"vtx_firmware", "VTX firmware"},
    {"goggle_firmware", "Goggle firmware"},
    {"flashing", "Flashing..."},
    {"apply_settings", "Apply Settings"},
    {"enable", "Enable"},
    {"mode", "Mode"},
    {"host", "Host"},
    {"client", "Client"},
    {"password", "Password"},
    {"wifi_module", "WiFi module"},
    {"basic", "Basic"},
    {"advanced", "Advanced"},
    {"system", "System"},
    {"reset_all_elements(both_modes)", "Reset all elements (both modes)"},
    {"save_changes", "Save changes"},
    {"cancel", "Cancel"},
    {"pos_x", "Pos-X"},
    {"pos_y", "Pos-Y"},
    {"bad_file", "Bad file"},
    {"elrs_off", "ELRS: Off"},
    {"wifi_off", "WiFi: Off"},
    {"integrity_check", "Integrity check"},
    {"elrs_on", "ELRS: On"},
    {"inputs", "Inputs"},
    {"main_menu", "Main Menu"},
    {"toggle_OSD", "Toggle OSD"},
    {"center_ht", "Center HT"},
    {"calibrate_ht", "Calibrate HT"},
    {"toggle_fan_sleep", "Toggle fan speed"},
    {"start_dvr", "start DVR"},
    {"change_fan_speed", "Change fan speed"},
    {"oled_brightness", "OLED brightness"},
    {"elrs", "ELRS"},
    {"input", "Input"},
    {"roller", "Roller"},
    {"left_short", "Left short"},
    {"left_long", "Left long"},
    {"right_short", "Right short"},
    {"right_long", "Right long"},
    {"right_double", "Right double"},
    {"osd", "OSD"},
    {"scannow", "Scan now"},
    {"wifi", "WiFi"},
    {"auto_scan:", "Auto Scan:"},
    {"autoscan_page_notice", "*if Auto Scan is 'Last', goggles will default to show last tuned channel"},
    {"clock:", "Clock:"},
    {"clock_page_note", "*Battery not installed or clock not configured"},
    {"elrs:", "ELRS"},
    {"fans:", "Fans:"},
    {"focus_chart", "Focus Chart"},
    {"focus_chart:", "Focus Chart:"},
    {"focus_chart_page_note", "Click the Enter Button to display the Back Focusing Chart.\nClick the Enter Button again to exit\n"},
    {"image_settigns", "Image settings"},
    {"image_setting_page_note", "To change image settings, click the Enter button to enter video mode.\nMake sure a HDZero VTX or analog VTX is powered on for live video.\n"},
    {"input_page_note", "*Settings apply to video mode only"},
    {"osd:", "OSD:"},
    {"osd_page_note", "Note: The positioning preview will display all OSD elements. Some elements might\nnot show during normal operation, depending on input source and conditions.\nOSD Element positioning is based on a 1280x720 canvas.\nPositions can be set for 4x3 and 16x9 modes separately,\nthe Show Element toggle is shared between both modes.\n"},
    {"playback:", "Playback:"},
    {"playback_page_note", "*Long press the Enter button to exit\n**Long press the Func button to delete\n"},
    {"power:", "Power:"},
    {"record_page_note", "MP4 format requires properly closing files or the files will be corrupt.\nTS format is highly recommended.\n"},
    {"scan_now_page_note", "When scanning is complete, use the\ndial to select a channel and press\nthe Enter button to choose\n"},
    {"go_sleep!", "Go Sleep:"},
    {"go_sleep_page_note", "Click the Enter Button to go sleep.\nClick any button to exit sleep mode.\n"},
    {"source:", "Source:"},
    {"firmware_page_note", "Language change required a reebot of the goggles"},
    {"update_complete", "Update complete"},
    {"mgsbox_update_complete", "Goggle update completed successfully.\nPlease repower goggle now.\n"},
    {"reset_settings", "Settings reset"},
    {"msgbox_settings_reset", "All settings have been reset.\nPlease repower goggle now.\n"},
    {"release_notes", "Release notes"},
    {"empty", "Empty"},
    {"image_settings:", "Image settings:"},
    {"record_options", "Record Options"},
    {"record_options:", "Record Options:"},
    {"format_sd_card_process", "Format SD Card #FF0000 Formatting...#"},
    {"format_sd_card_confirmation", "Format SD Card #FFFF00 Click to confirm or Scroll to cancel...#"},
    {"repair_sd_card_process", "Repair SD Card #FF0000 Repairing...#"},
    {"repair_sd_card_confirmation", "Repair SD Card #FFFF00 Click to confirm or Scroll to cancel...#"},
    {"target:", "Target:"},
    {"page_wifi_update_page_1_note", "Password Requirements:\n    Minimum 8 characters, maximum 64 characters.\n\nLive Stream:\n    1.  Connect to the WiFi network identified above.\n    2. Use VLC Player to open a Network Stream:\n\n            rtsp://%s:8554/hdzero\n    \n"},
    {"page_wifi_update_page_3_note", "Password Requirements:\n    Minimum 8 characters, maximum 64 characters.\n"},
    {"ssid", "SSID"},
    {"dhcp", "DHCP"},
    {"address", "Address"},
    {"netmask", "Netmask"},
    {"gateway", "Gateway"},
    {"dns", "DNS"},
    {"rf_channel", "RF Channel"},
    {"root_pw", "Root PW"},
    {"ssh", "SSH"},
    {"statusbar_sdcard_full_clips", "%d clip(s), SD Card full"},
    {"statusbar_sdcard_clips_and_available", "%d clip(s), %.2fGB available"},
    {"statusbar_sdcard_full_data", "#FF0000 SD Card full#"},
    {"statusbar_sdcard_available", "%.2fGB available"},
    {"statusbar_sdcard_unsupported", "Unsupported"},
    {"statusbar_no_sdcard", "No SD Card"},
    {"statusbar_sdcard", "SD Card                 "},
    {"element:", "Element: "},
    {"not_found", "not found"},
    {"update_device", "Update %s"},
    {"firmware:", "Firmware:"},
    {"confirmation", "#FFFF00 Click to confirm or Scroll to cancel...#"},
    {"sd_card_format_status", "SD Card Format Status"},
    {"goggle", "Очки"},
    {"VTX", "VTX"},
    {"updating_clock", "#FF0000 Updating Clock...#"},
    {"elements_reset", "#00FF00 Elements reset.#"},
    {"format_was_successfull", "Format was successful.\nPress click to exit.\n"},
    {"format_has_failed", "Format has failed.\nPress click to exit.\n"},
    {"insert_sd_card", "Please insert a SD Card.\nPress click to exit.\n"},
    {"failed_to_extract_results", "Failed to extract results.\nPress click to exit.\n"},
    {"failed_to_access_results", "Failed to access results.\nPress click to exit.\n"},
    {"failed_to_generate_results", "Failed to generate results.\nPress click to exit.\n"},
    {"failed_to_start_format", "Failed to start format.\nPress click to exit.\n"},
    {"unsupported_status_code", "Unsupported status code.\nPress click to exit.\n"},
    {"filesystem_is_ok", "Filesystem is OK.\nPress click to exit.\n"},
    {"filesystem_was_fixed", "Filesystem was modified and fixed.\nPress click to exit.\n"},
    {"failed_to_remount", "Failed to remount SD Card.\nPress click to exit.\n"},
    {"failed_to_start_repair", "Failed to start repair.\nPress click to exit.\n"},
    {"sd_card_repair_status", "SD Card Repair Status"},
    {"storage:", "Storage:"},
    {"wifi_ssid", "WiFi: %s"},
    {"wifi_searching", "WiFi: Searcing"},
    {"failed", "#FF0000 FAILED#"},
    {"success", "#00FF00 Success#"},
    {"timeout", "#FEBE00 Timeout#"},
    {"cancelled", "#FEBE00 Cancelled#"},
    {"detected", "#00FF00 Detected#"},
    {"disconnected", "#C0C0C0 Disconnected"},
    {"hdzero_r_channel", "HDZero: R%d"},
    {"hdzero_f_channel", "HDZero: F%d"},
    {"hdzerp_l_channel", "HDZero: L%d"},
    {"hdmi_in_string", "HDMI In: %s"},
    {"av_id_string", "AV In: %s"},
    {"expansion_module_string", "Expansion Module: %s"},
    {"set_clock_red", "#%02x0000 Set Clock#"},
    {NULL, NULL} // End mark
};



static uint8_t en_gb_plural_fn(int32_t num)
{
    uint32_t n = op_n(num); UNUSED(n);
    uint32_t i = op_i(n); UNUSED(i);
    uint32_t v = op_v(n); UNUSED(v);

    if ((i == 1 && v == 0)) return LV_I18N_PLURAL_TYPE_ONE;
    return LV_I18N_PLURAL_TYPE_OTHER;
}

static const lv_i18n_lang_t en_gb_lang = {
    .locale_name = "en-GB",
    .singulars = en_gb_singulars,

    .locale_plural_fn = en_gb_plural_fn
};

static lv_i18n_phrase_t ru_ru_singulars[] = {
    {"source", "Источник"},
    {"auto_scan", "Автоскан"},
    {"on", "Вкл"},
    {"last", "Посл."},
    {"off", "Выкл"},
    {"default", "Дефолт"},
    {"hdzero", "HDZero"},
    {"expansion", "Расшир."},
    {"av_in", "A/V"},
    {"hdmi_in", "HDMI"},
    {"back", "< Назад"},
    {"set_clock", "Установить время"},
    {"clock", "Время"},
    {"format", "Формат"},
    {"24_hour", "24 Часа"},
    {"send_vtx", "Отправить VTX"},
    {"click_to_start", "Нажмите для начала"},
    {"bind", "Соединенить"},
    {"starting", "Загрузка"},
    {"binding", "Соединение..."},
    {"fans", "Куллеры"},
    {"auto_control", "Автоматич."},
    {"top_fan", "Верхний куллер"},
    {"side_fans", "Боковые куллеры"},
    {"set_alarm_angle", "Уст. увед. наклона"},
    {"head_tracker", "Трекинг"},
    {"page", "Страница"},
    {"tracking", "Трекинг"},
    {"tilt_alarm", "Увед. накл."},
    {"alarm", "Сигнал"},
    {"video", "Видео"},
    {"arm", "Арм"},
    {"calibrate", "Откалибровать"},
    {"set_center", "Установить центр"},
    {"max_angle", "Макс. угол"},
    {"pan", "Панорама"},
    {"tilt", "Наклон"},
    {"roll", "Вращение"},
    {"calibrating", "Калибровка..."},
    {"re_calibrating", "Перекалибровка"},
    {"updating_angle", "Обновление угла..."},
    {"oled", "OLED"},
    {"brightness", "Яркость"},
    {"saturation", "Насыщенность"},
    {"contrast", "Контраст"},
    {"oled_auto_off", "OLED авто выкл"},
    {"adjust_osd_elements", "Настройка эл-тов OSD"},
    {"osd_orbit", "OSD орбита"},
    {"min", "Мин"},
    {"max", "Макс"},
    {"osd_mode", "Режим OSD"},
    {"at_startup", "При запуске"},
    {"show", "Показать"},
    {"hide", "Скрыть"},
    {"playback", "Воспроизв."},
    {"power", "Питание"},
    {"battery", "Аккумулятор"},
    {"cell_count_mode", "Подсчет ячеек"},
    {"auto", "Авто"},
    {"manual", "Ручной"},
    {"cell_count", "Кол-во ячеек"},
    {"warning_cell_voltage", "Предупрежд. низк. напряж."},
    {"voltage_calibration", "Калибровка напряж."},
    {"display_mode", "Отображение"},
    {"total", "Всего"},
    {"cell_average", "Среднее"},
    {"warning_type", "Предупрежд."},
    {"beep", "Звук"},
    {"visual", "Визуал"},
    {"both", "Оба"},
    {"analogrx_power", "AnalogRX питание"},
    {"record_mode", "Режим записи"},
    {"record_format", "Формат записи"},
    {"record_osd", "Запись OSD"},
    {"yes", "Да"},
    {"no", "Нет"},
    {"record_audio", "Запись аудио"},
    {"audio_source", "Источник аудио"},
    {"mic", "Микроф."},
    {"line_in", "Линейн."},
    {"a/v_in", "A/V вход"},
    {"naming_scheme", "Именование"},
    {"digits", "Цифры"},
    {"date", "Дата"},
    {"scan_ready", "Сканирование готово"},
    {"scanning", "Сканирование..."},
    {"scanning_done", "Сканирование завершено"},
    {"scanning_ready", "Сканирование готово"},
    {"go_sleep", "Сон"},
    {"expansion_module", "Модуль расширения"},
    {"analog_video", "Аналог"},
    {"hdzero_band", "HDZero band"},
    {"raceband", "Raceband"},
    {"lowband", "Lowband"},
    {"hdzero_bw", "HDZero BW"},
    {"wide", "Широкий"},
    {"narrow", "Узкий"},
    {"oled_pattern_normal", "OLED шаблон нормальный"},
    {"format_sd_card", "Форматировать SD карту"},
    {"repair_sd_card", "Восстановить SD карту"},
    {"storage", "Хранилище"},
    {"logging", "Логирование"},
    {"firmware", "Прошивка"},
    {"current_version", "Текущая версия"},
    {"reset_all_settings", "Сброс всех настроек"},
    {"update_vtx", "Обновить VTX"},
    {"update_goggle", "Обновить очки"},
    {"update_esp32", "Обновить ESP32"},
    {"vtx_firmware", "Прошивка VTX"},
    {"goggle_firmware", "Прошивка очков"},
    {"flashing", "Обновление..."},
    {"apply_settings", "Применить настройки"},
    {"enable", "Вкл"},
    {"mode", "Режим"},
    {"host", "Хост"},
    {"client", "Клиент"},
    {"password", "Пароль"},
    {"wifi_module", "WiFi модуль"},
    {"basic", "Базовый"},
    {"advanced", "Расшир."},
    {"system", "Система"},
    {"reset_all_elements(both_modes)", "Сброс всех элементов (оба режима)"},
    {"save_changes", "Сохранить изменения"},
    {"cancel", "Отмена"},
    {"pos_x", "Поз.  X"},
    {"pos_y", "Поз. Y"},
    {"bad_file", "Испорченный файл"},
    {"elrs_off", "ELRS: Выкл"},
    {"wifi_off", "WiFi: Выкл"},
    {"integrity_check", "Проверка целостности"},
    {"elrs_on", "ELRS: Вкл"},
    {"main_menu", "Главное меню"},
    {"toggle_OSD", "Переключить OSD"},
    {"center_ht", "Центр HT"},
    {"calibrate_ht", "Калибровка HT"},
    {"toggle_fan_sleep", "Переключить сон куллера"},
    {"start_dvr", "Запись DVR"},
    {"change_fan_speed", "Изменить скорость куллера"},
    {"oled_brightness", "Яркость OLED"},
    {"inputs", "Вводы"},
    {"elrs", "ELRS"},
    {"input", "Ввод"},
    {"record_options", "Запись / DVR"},
    {"image_settigns", "Изображение"},
    {"roller", "Колесо"},
    {"left_short", "Коротк. влево"},
    {"left_long", "Длинн. влево"},
    {"right_short", "Коротк. вправо"},
    {"right_long", "Длинн. вправо"},
    {"right_double", "Двойн. вправо"},
    {"osd", "OSD"},
    {"scannow", "Сканировать"},
    {"wifi", "WiFi"},
    {"auto_scan:", "Автоскан:"},
    {"autoscan_page_notice", "*Если выбрано \"Последний\", то очки по умолчанию \nпокажут последний настроенный канал\n"},
    {"clock:", "Время:"},
    {"clock_page_note", "*Батарея не установлена или время не настроено"},
    {"elrs:", "ELRS:"},
    {"fans:", "Куллеры:"},
    {"focus_chart", "Фокус чарт"},
    {"focus_chart:", "Фокус чарт:"},
    {"focus_chart_page_note", "Нажмите кнопку Enter для отображения фокус чарта.\nНажмите кнопку Enter еще раз, чтобы выйти\n"},
    {"image_setting_page_note", "Чтобы изменить настройки изображения, нажмите \nкнопку Enter для переключения на режим видео. \nУбедитесь, что HDZero или аналоговый\nпередатчики включены для лайв-видео\n"},
    {"input_page_note", "*Настройки применяются только для режима видео"},
    {"osd:", "OSD:"},
    {"osd_page_note", "Внимание: Превью позиционирования покажет все элементы OSD. \nНекоторые элементы могут быть скрыты во время обычного пользования, \nзависимо от входного источника и условий. \nПозицирование элементов OSD базировано на канвасе 1280x720. \nПозиционирование может быть отдельно установлено для\nрежимов 4x3 и 16x9, переключатель Show Element применяется для обоих режимов\n"},
    {"playback:", "Воспроизведение:"},
    {"playback_page_note", "*Удержите кнопку Enter для выхода \nУдержите кнопку Func для удаления\n"},
    {"power:", "Питание:"},
    {"record_page_note", "Формат MP4 требует правильного закрытия файла, иначе запись будет испорчена.\nКрайне рекомендуется использовать формат TS\n"},
    {"scan_now_page_note", "Когда сканирование завершится, \nиспользуйте колесо для выбора канала и\nнажмите кнопку Enter для подтвержения\n"},
    {"go_sleep!", "Иди спать!"},
    {"go_sleep_page_note", "Нажмите на Enter для входа в режим сна. \nНажмите на любую кнопку для выхода из режима сна\n"},
    {"source:", "Источник:"},
    {"firmware_page_note", "Смена языка требует перезагрузки очков"},
    {"update_complete", "Обновление завершено"},
    {"mgsbox_update_complete", "Очки успешно обновлены.\nПожалуйста, перезагрузите очки.\n"},
    {"reset_settings", "Сброс настроек"},
    {"msgbox_settings_reset", "Все настройки были сброшены.\nПожалуйста, перезагрузите очки.\n"},
    {"release_notes", "Изменения:"},
    {"empty", "нету"},
    {"image_settings:", "Настройки изображения:"},
    {"record_options:", "Настройки записи:"},
    {"format_sd_card_process", "Форматирование SD карты #FF0000 Форматирование...#"},
    {"format_sd_card_confirmation", "Форматирование SD карты #FFFF00 Нажмите для подтверждения или прокрутите для отмены...#"},
    {"repair_sd_card_process", "Восстановление SD карты #FF0000 Восстановление...#"},
    {"repair_sd_card_confirmation", "Восстановление SD карты #FFFF00 Нажмите для подтверждения или прокрутите для отмены...#"},
    {"target:", "Файл:"},
    {"page_wifi_update_page_1_note", "Требования к паролю:\n    Минимум 8 символов, максимум 64 символа.\n\nПрямая трансляция:\n    1. Подключитесь к WiFi-сети, указанной выше.\n    2. Используйте VLC Player для открытия сетевого потока:\n\n            rtsp://%s:8554/hdzero\n  \n"},
    {"page_wifi_update_page_3_note", "Требования к паролю:\n    Минимум 8 символов, максимум 64 символа.\n"},
    {"ssid", "SSID"},
    {"dhcp", "DHCP"},
    {"address", "Адрес"},
    {"netmask", "Netmask"},
    {"gateway", "Шлюз"},
    {"dns", "DNS"},
    {"rf_channel", "RF канал"},
    {"root_pw", "Root PW"},
    {"ssh", "SSH"},
    {"statusbar_sdcard_full_clips", "%d клип(ов), карта заполнена"},
    {"statusbar_sdcard_clips_and_available", "%d клип(ов), %.2fGB доступно"},
    {"statusbar_sdcard_full_data", "#FF0000 карта заполнена"},
    {"statusbar_sdcard_available", "%.2fGB доступно"},
    {"statusbar_sdcard_unsupported", "Не поддерживается"},
    {"statusbar_no_sdcard", "Нет SD карты"},
    {"statusbar_sdcard", "SD Карта                "},
    {"element:", "Элемент:"},
    {"not_found", "не найдена"},
    {"update_device", "Обновить %s"},
    {"firmware:", "Прошивка:"},
    {"confirmation", "#FFFF00 Нажмите для подтв./прокр. для отмены#"},
    {"sd_card_format_status", "Статус форматирования SD карты"},
    {"goggle", "Очки"},
    {"VTX", "VTX"},
    {"updating_clock", "#FF0000 Установка Времени...#"},
    {"elements_reset", "#00FF00 Элементы сброшены.#"},
    {"format_was_successfull", "Форматирование прошло успешно.\nНажмите для выхода.\n"},
    {"format_has_failed", "Форматирование не удалось.\nНажмите для выхода.\n"},
    {"insert_sd_card", "Пожалуйста, вставьте SD-карту.\nНажмите для выхода.\n"},
    {"failed_to_extract_results", "Не удалось извлечь результаты.\nНажмите для выхода.\n"},
    {"failed_to_access_results", "Не удалось получить доступ к результатам.\nНажмите для выхода.\n"},
    {"failed_to_generate_results", "Не удалось сгенерировать результаты.\nНажмите для выхода.\n"},
    {"failed_to_start_format", "Не удалось начать форматирование.\nНажмите для выхода.\n"},
    {"unsupported_status_code", "Неподдерживаемый ответный код.\nНажмите для выхода.\n"},
    {"filesystem_is_ok", "Файловая система в порядке.\nНажмите для выхода.\n"},
    {"filesystem_was_fixed", "Файловая система была изменена и исправлена.\nНажмите для выхода.\n"},
    {"failed_to_remount", "Не удалось повторно смонтировать SD-карту.\nНажмите для выхода.\n"},
    {"failed_to_start_repair", "Не удалось начать восстановление.\nНажмите для выхода.\n"},
    {"sd_card_repair_status", "Статус восстановления SD карты"},
    {"storage:", "Хранилище:"},
    {"wifi_ssid", "WiFi: %s"},
    {"wifi_searching", "WiFi: Поиск"},
    {"failed", "#FF0000 ПРОВАЛ#"},
    {"success", "#00FF00 Успешно#"},
    {"timeout", "#FEBE00 Таймаут#"},
    {"cancelled", "#FEBE00 Отмена#"},
    {"detected", "#00FF00 Обнаружен#"},
    {"disconnected", "#C0C0C0 Не подключен"},
    {"hdzero_r_channel", "HDZero: R%d"},
    {"hdzero_f_channel", "HDZero: F%d"},
    {"hdzerp_l_channel", "HDZero: L%d"},
    {"hdmi_in_string", "HDMI Вход: %s"},
    {"av_id_string", "A/V Вход: %s"},
    {"expansion_module_string", "Модуль расширения: %s"},
    {"set_clock_red", "#%02x0000 Установить время#"},
    {NULL, NULL} // End mark
};



static uint8_t ru_ru_plural_fn(int32_t num)
{
    uint32_t n = op_n(num); UNUSED(n);
    uint32_t v = op_v(n); UNUSED(v);
    uint32_t i = op_i(n); UNUSED(i);
    uint32_t i10 = i % 10;
    uint32_t i100 = i % 100;
    if ((v == 0 && i10 == 1 && i100 != 11)) return LV_I18N_PLURAL_TYPE_ONE;
    if ((v == 0 && (2 <= i10 && i10 <= 4) && (!(12 <= i100 && i100 <= 14)))) return LV_I18N_PLURAL_TYPE_FEW;
    if ((v == 0 && i10 == 0) || (v == 0 && (5 <= i10 && i10 <= 9)) || (v == 0 && (11 <= i100 && i100 <= 14))) return LV_I18N_PLURAL_TYPE_MANY;
    return LV_I18N_PLURAL_TYPE_OTHER;
}

static const lv_i18n_lang_t ru_ru_lang = {
    .locale_name = "ru-RU",
    .singulars = ru_ru_singulars,

    .locale_plural_fn = ru_ru_plural_fn
};

const lv_i18n_language_pack_t lv_i18n_language_pack[] = {
    &en_gb_lang,
    &ru_ru_lang,
    NULL // End mark
};

////////////////////////////////////////////////////////////////////////////////


// Internal state
static const lv_i18n_language_pack_t * current_lang_pack;
static const lv_i18n_lang_t * current_lang;


/**
 * Reset internal state. For testing.
 */
void __lv_i18n_reset(void)
{
    current_lang_pack = NULL;
    current_lang = NULL;
}

/**
 * Set the languages for internationalization
 * @param langs pointer to the array of languages. (Last element has to be `NULL`)
 */
int lv_i18n_init(const lv_i18n_language_pack_t * langs)
{
    if(langs == NULL) return -1;
    if(langs[0] == NULL) return -1;

    current_lang_pack = langs;
    current_lang = langs[0];     /*Automatically select the first language*/
    return 0;
}

/**
 * Change the localization (language)
 * @param l_name name of the translation locale to use. E.g. "en-GB"
 */
int lv_i18n_set_locale(const char * l_name)
{
    if(current_lang_pack == NULL) return -1;

    uint16_t i;

    for(i = 0; current_lang_pack[i] != NULL; i++) {
        // Found -> finish
        if(strcmp(current_lang_pack[i]->locale_name, l_name) == 0) {
            current_lang = current_lang_pack[i];
            return 0;
        }
    }

    return -1;
}


static const char * __lv_i18n_get_text_core(lv_i18n_phrase_t * trans, const char * msg_id)
{
    uint16_t i;
    for(i = 0; trans[i].msg_id != NULL; i++) {
        if(strcmp(trans[i].msg_id, msg_id) == 0) {
            /*The msg_id has found. Check the translation*/
            if(trans[i].translation) return trans[i].translation;
        }
    }

    return NULL;
}


/**
 * Get the translation from a message ID
 * @param msg_id message ID
 * @return the translation of `msg_id` on the set local
 */
const char * lv_i18n_get_text(const char * msg_id)
{
    if(current_lang == NULL) return msg_id;

    const lv_i18n_lang_t * lang = current_lang;
    const void * txt;

    // Search in current locale
    if(lang->singulars != NULL) {
        txt = __lv_i18n_get_text_core(lang->singulars, msg_id);
        if (txt != NULL) return txt;
    }

    // Try to fallback
    if(lang == current_lang_pack[0]) return msg_id;
    lang = current_lang_pack[0];

    // Repeat search for default locale
    if(lang->singulars != NULL) {
        txt = __lv_i18n_get_text_core(lang->singulars, msg_id);
        if (txt != NULL) return txt;
    }

    return msg_id;
}

/**
 * Get the translation from a message ID and apply the language's plural rule to get correct form
 * @param msg_id message ID
 * @param num an integer to select the correct plural form
 * @return the translation of `msg_id` on the set local
 */
const char * lv_i18n_get_text_plural(const char * msg_id, int32_t num)
{
    if(current_lang == NULL) return msg_id;

    const lv_i18n_lang_t * lang = current_lang;
    const void * txt;
    lv_i18n_plural_type_t ptype;

    // Search in current locale
    if(lang->locale_plural_fn != NULL) {
        ptype = lang->locale_plural_fn(num);

        if(lang->plurals[ptype] != NULL) {
            txt = __lv_i18n_get_text_core(lang->plurals[ptype], msg_id);
            if (txt != NULL) return txt;
        }
    }

    // Try to fallback
    if(lang == current_lang_pack[0]) return msg_id;
    lang = current_lang_pack[0];

    // Repeat search for default locale
    if(lang->locale_plural_fn != NULL) {
        ptype = lang->locale_plural_fn(num);

        if(lang->plurals[ptype] != NULL) {
            txt = __lv_i18n_get_text_core(lang->plurals[ptype], msg_id);
            if (txt != NULL) return txt;
        }
    }

    return msg_id;
}

/**
 * Get the name of the currently used locale.
 * @return name of the currently used locale. E.g. "en-GB"
 */
const char * lv_i18n_get_current_locale(void)
{
    if(!current_lang) return NULL;
    return current_lang->locale_name;
}
