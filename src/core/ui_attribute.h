#ifndef _UI_ATTRIBUTE_H
#define _UI_ATTRIBUTE_H

//#define TEST_STATUSBAR
//#define TEST_SCANNOW

#define SCREEN_HMAX                     1900
#define SCREEN_VMAX                     1000

// statusbar attribute

#define STATUSBAR_WIDTH                 SCREEN_HMAX
#define STATUSBAR_HEIGHT                96
#define STATUSBAR_POS_OFFSET_X          0
#define STATUSBAR_POS_OFFSET_Y          0
#define STATUSBAR_BG_COLOR              0x131313
#define STATUSBAR_TEXT_COLOR_NORMAL     0xffffff //white
#define STATUSBAR_TEXT_COLOR_WARNING    0xff0000 //red
#define STATUSBAR_TEXT_FONT             (&lv_font_montserrat_24)

#define IMG_HDZ_LOGO_WIDTH          264
#define IMG_HDZ_LOGO_HEIGHT         96
#define IMG_SDCARD_WIDTH            64
#define IMG_SDCARD_HEIGHT           96
#define IMG_IC_WIDTH                64
#define IMG_IC_HEIGHT               96
#define IMG_BACKPACK_WIDTH          64
#define IMG_BACKPACK_HEIGHT         96
#define IMG_WIFI_WIDTH              64
#define IMG_WIFI_HEIGHT             96
#define IMG_BAT_WIDTH               64
#define IMG_BAT_HEIGHT              96
#define LABEL_SDCARD_WIDTH          256
#define LABEL_CHANNEL_WIDTH         256
#define LABEL_BACKPACK_WIDTH        256
#define LABEL_WIFI_WIDTH            256
#define LABEL_BAT_WIDTH             128

#define IMG_HDZ_LOGO_POS_X          0
#define IMG_HDZ_LOGO_POS_Y          0
#define IMG_SDCARD_POS_X            264
#define IMG_SDCARD_POS_Y            0
#define LABEL_SDCARD_POS_X          (IMG_SDCARD_POS_X + IMG_SDCARD_WIDTH)
#define LABEL_SDCARD_POS_Y          25
#define IMG_IC_POS_X                IMG_SDCARD_POS_X + 350
#define IMG_IC_POS_Y                0
#define LABEL_CHANNEL_POS_X         (IMG_IC_POS_X + IMG_IC_WIDTH)
#define LABEL_CHANNEL_POS_Y         40
#define IMG_BACKPACK_POS_X          (IMG_SDCARD_POS_X + 350*2)
#define IMG_BACKPACK_POS_Y          0
#define LABEL_BACKPACK_POS_X        (IMG_BACKPACK_POS_X + IMG_BACKPACK_WIDTH)
#define LABEL_BACKPACK_POS_Y        40
#define IMG_WIFI_POS_X              (IMG_SDCARD_POS_X + 350*3)
#define IMG_WIFI_POS_Y              0
#define LABEL_WIFI_POS_X            (IMG_WIFI_POS_X + IMG_WIFI_WIDTH)
#define LABEL_WIFI_POS_Y            40
#define IMG_BAT_POS_X               (IMG_SDCARD_POS_X + 350*4)
#define IMG_BAT_POS_Y               0
#define LABEL_BAT_POS_X             (IMG_BAT_POS_X + IMG_BAT_WIDTH)
#define LABEL_BAT_POS_Y             40

//menu
#define MENU_WIDTH                      SCREEN_HMAX
#define MENU_HEIGHT                     (SCREEN_VMAX - STATUSBAR_HEIGHT)
#define MENU_POS_OFFSET_X               (0)
#define MENU_POS_OFFSET_Y               (STATUSBAR_HEIGHT)
#define MENU_BG_COLOR                   0x404040 //gray
#define MENU_TEXT_COLOR                 0xffffff
//menu_lv2
#define MENU_LV2_POS_X                  0
#define MENU_LV2_POS_Y                  100
#define MENU_LV2_WIDTH                  1050
#define MENU_LV2_HEIGHT                 (SCREEN_VMAX - STATUSBAR_HEIGHT - MENU_LV2_POS_Y)
#define MENU_LV2_BG_COLOR               0x131313 //gray
#define MENU_LV2_TEXT_COLOR             0xffffff
#define MENU_LV2_LABEL_TITLE_POS_X      100
#define MENU_LV2_LABEL_TITLE_POS_Y      130
#define MENU_LV2_ITEM_START_X           150
#define MENU_LV2_ITEM_START_Y           190
#define MENU_LV2_ITEM_INTERVAL_Y        60
#define MENU_LV2_SLIDER_LABEL_WIDTH     256
#define MENU_LV2_SLIDER_LABEL_HEIGHT    60
#define MENU_LV2_SLIDER_WIDTH           320
#define MENU_LV2_SLIDER_HEIGHT          2
#define MENU_LV2_SLIDER_POS_X           (MENU_LV2_ITEM_START_X + MENU_LV2_SLIDER_LABEL_WIDTH + 50)
#define MENU_LV2_SLIDER_POS_Y_OFFSET    16
#define MENU_LV2_SLIDER_VALUE_POS_X     (MENU_LV2_ITEM_START_X + MENU_LV2_SLIDER_LABEL_WIDTH + MENU_LV2_SLIDER_WIDTH + 100)
#define MENU_LV2_SLIDER_VALUE_POS_Y_OFFSET 0

//menu->root_page   
#define ROOT_PAGE_WIDTH                 264
#define ROOT_PAGE_HEIGHT                MENU_HEIGHT
#define ROOT_PAGE_POS_X                 0
#define ROOT_PAGE_POS_Y                 0

//menu->root_page->section
#define SECTION_WIDTH                   ROOT_PAGE_WIDTH
#define SECTION_HEIGHT                  MENU_HEIGHT
#define SECTION_POS_X                   0
#define SECTION_POS_Y                   0

#define PAGE_WIDTH_OFFSET               (-2) // gray line of menu list cost 2??? ligen
/*menu->page_scannow*/
#define PAGE_SCANNOW_WIDTH              (SCREEN_HMAX - SECTION_WIDTH + PAGE_WIDTH_OFFSET)
#define PAGE_SCANNOW_HEIGHT             MENU_HEIGHT
/*menu->page_scannow->cont*/
#define CONT_SCANNOW_WIDTH              PAGE_SCANNOW_WIDTH
#define CONT_SCANNOW_HEIGHT             250
#define CONT_SCANNOW_POS_X              0
#define CONT_SCANNOW_POS_Y              0
/*menu->page_scannow->cont->obj_progressbar*/
#define OBJ_PROGRESSBAR_WIDTH           600
#define OBJ_PROGRESSBAR_HEIGHT          50
#define OBJ_PROGRESSBAR_POS_X           100
#define OBJ_PROGRESSBAR_POS_Y           100
#define OBJ_PROGRESSBAR_COLOR_INVALID   0xffffff //white
#define OBJ_PROGRESSBAR_COLOR_VALID     0x00ff00 //green
/*menu->page_scannow->cont->label_progressbar_status*/
#define LABEL_PROGRESSBAR_POS_X         100
#define LABEL_PROGRESSBAR_POS_Y         60
/*menu->page_scannow->cont->label_help_info*/
#define LABEL_HELP_INFO_POS_X           1050
#define LABEL_HELP_INFO_POS_Y           80
/*menu->page_scannow->cont2*/
#define CONT2_SCANNOW_WIDTH             PAGE_SCANNOW_WIDTH
#define CONT2_SCANNOW_HEIGHT            (PAGE_SCANNOW_HEIGHT - CONT_SCANNOW_HEIGHT)
#define CONT2_SCANNOW_POS_X             0
#define CONT2_SCANNOW_POS_Y             0
/*menu->page_scannow->cont2->channel*/
#define SIGNAL_START_X                  300
#define SIGNAL_START_Y                  50
#define SIGNAL_INTERVAL_X               600
#define SIGNAL_INTERVAL_Y               80
#define CHANNEL_IMG0_WIDTH              77
#define CHANNEL_IMG0_HEIGHT             77
#define CHANNEL_IMG0_START_X            SIGNAL_START_X
#define CHANNEL_IMG0_START_Y            SIGNAL_START_Y
#define CHANNEL_LABEL_START_X           (CHANNEL_IMG0_START_X + CHANNEL_IMG0_WIDTH)
#define CHANNEL_LABEL_START_Y           (SIGNAL_START_Y + 25)
#define CHANNEL_IMG1_WIDTH              164
#define CHANNEL_IMG1_HEIGHT             78
#define CHANNEL_IMG1_START_X            (CHANNEL_LABEL_START_X + 60)
#define CHANNEL_IMG1_START_Y            (SIGNAL_START_Y - 10)

/*menu->page_source*/
#define PAGE_SOURCE_WIDTH               (SCREEN_HMAX - SECTION_WIDTH + PAGE_WIDTH_OFFSET)
#define PAGE_SOURCE_HEIGHT              MENU_HEIGHT
/*menu->page_source->cont*/
#define CONT_SOURCE_WIDTH               PAGE_SOURCE_WIDTH
#define CONT_SOURCE_HEIGHT              PAGE_SOURCE_HEIGHT
/*menu->page_imagesetting*/
#define PAGE_IMAGESETTINGS_WIDTH        (SCREEN_HMAX - SECTION_WIDTH + PAGE_WIDTH_OFFSET)
#define PAGE_IMAGESETTINGS_HEIGHT       MENU_HEIGHT
/*menu->page_imagesetting->cont*/
#define CONT_IMAGESETTINGS_WIDTH        PAGE_IMAGESETTINGS_WIDTH
#define CONT_IMAGESETTINGS_HEIGHT       PAGE_IMAGESETTINGS_HEIGHT
/*menu->page_power*/
#define PAGE_POWER_WIDTH                (SCREEN_HMAX - SECTION_WIDTH + PAGE_WIDTH_OFFSET)
#define PAGE_POWER_HEIGHT               MENU_HEIGHT
/*menu->page_fans->cont*/
#define CONT_POWER_WIDTH                PAGE_POWER_WIDTH
#define CONT_POWER_HEIGHT               PAGE_POWER_HEIGHT
/*menu->page_fans*/
#define PAGE_FANS_WIDTH                 (SCREEN_HMAX - SECTION_WIDTH + PAGE_WIDTH_OFFSET)
#define PAGE_FANS_HEIGHT                MENU_HEIGHT
/*menu->page_fans->cont*/
#define CONT_FANS_WIDTH                 PAGE_FANS_WIDTH
#define CONT_FANS_HEIGHT                PAGE_FANS_HEIGHT
/*menu->page_record*/
#define PAGE_RECORD_WIDTH               (SCREEN_HMAX - SECTION_WIDTH + PAGE_WIDTH_OFFSET)
#define PAGE_RECORD_HEIGHT              MENU_HEIGHT
/*menu->page_record->cont*/
#define CONT_RECORD_WIDTH               PAGE_RECORD_WIDTH
#define CONT_RECORD_HEIGHT              PAGE_RECORD_HEIGHT
/*menu->page_autoscan*/
#define PAGE_AUTOSCAN_WIDTH             (SCREEN_HMAX - SECTION_WIDTH + PAGE_WIDTH_OFFSET)
#define PAGE_AUTOSCAN_HEIGHT            MENU_HEIGHT
/*menu->page_autoscan->cont*/
#define CONT_AUTOSCAN_WIDTH             PAGE_AUTOSCAN_WIDTH
#define CONT_AUTOSCAN_HEIGHT            PAGE_AUTOSCAN_HEIGHT
/*menu->page_connection*/
#define PAGE_CONNECTIONS_WIDTH          (SCREEN_HMAX - SECTION_WIDTH + PAGE_WIDTH_OFFSET)
#define PAGE_CONNECTIONS_HEIGHT         MENU_HEIGHT
/*menu->page_connection->cont*/
#define CONT_CONNECTIONS_WIDTH          PAGE_CONNECTIONS_WIDTH
#define CONT_CONNECTIONS_HEIGHT         PAGE_CONNECTIONS_HEIGHT
/*menu->page_headtracker*/
#define PAGE_HEADTRACKER_WIDTH          (SCREEN_HMAX - SECTION_WIDTH + PAGE_WIDTH_OFFSET)
#define PAGE_HEADTRACKER_HEIGHT         MENU_HEIGHT
/*menu->cont_headtracker->cont*/
#define CONT_HEADTRACKER_WIDTH          PAGE_HEADTRACKER_WIDTH
#define CONT_HEADTRACKER_HEIGHT         PAGE_HEADTRACKER_HEIGHT
/*menu->page_playback*/
#define PAGE_PLAYBACK_WIDTH             (SCREEN_HMAX - SECTION_WIDTH + PAGE_WIDTH_OFFSET)
#define PAGE_PLAYBACK_HEIGHT            MENU_HEIGHT
/*menu->cont_playback->cont*/
#define CONT_PLAYBACK_WIDTH             PAGE_PLAYBACK_WIDTH
#define CONT_PLAYBACK_HEIGHT            PAGE_PLAYBACK_HEIGHT
/*menu->cont_playback->cont->img*/
#define CONT_PLAYBACK_IMG_START_X       MENU_LV2_ITEM_START_X
#define CONT_PLAYBACK_IMG_START_Y       MENU_LV2_ITEM_START_Y
/*menu->page_version*/
#define PAGE_VERSION_WIDTH              (SCREEN_HMAX - SECTION_WIDTH + PAGE_WIDTH_OFFSET)
#define PAGE_VERSION_HEIGHT             MENU_HEIGHT
/*menu->cont_version->cont*/
#define CONT_VERSION_WIDTH              (PAGE_VERSION_WIDTH)
#define CONT_VERSION_HEIGHT             PAGE_VERSION_HEIGHT
#endif
