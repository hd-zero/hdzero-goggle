#include "ui/ui_style.h"

const lv_style_prop_t props[] = {LV_STYLE_BG_COLOR, 0};
lv_style_transition_dsc_t transition_dsc;

lv_style_t style_rootmenu;
lv_style_t style_submenu;
lv_style_t style_statusbar;
lv_style_t style_subpage;
lv_style_t style_context;
lv_style_t style_silder_main;
lv_style_t style_silder_select;
lv_style_t style_silder_indicator;
lv_style_t style_silder_knob;
lv_style_t style_silder_pressed_color;
lv_style_t style_silder_pressed_color;
lv_style_t style_scan;
lv_style_t style_select;
lv_style_t style_osd;
lv_style_t style_pb,style_pb_dark;

int style_init(void)
{
    lv_style_reset(&style_rootmenu);
    lv_style_init(&style_rootmenu);
    lv_style_set_bg_color(&style_rootmenu, lv_color_make(19, 19, 19));
    lv_style_set_text_color(&style_rootmenu, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_pad_top(&style_rootmenu, 0);
	lv_style_set_pad_bottom(&style_rootmenu, 0);
	lv_style_set_pad_left(&style_rootmenu, 0);
	lv_style_set_pad_right(&style_rootmenu, 0);
	lv_style_set_radius(&style_rootmenu, 0);
	lv_style_set_border_width(&style_rootmenu, 0);

    lv_style_set_border_color(&style_rootmenu, lv_palette_main(LV_PALETTE_RED));
    lv_style_reset(&style_rootmenu);
    lv_style_init(&style_rootmenu);
    lv_style_set_bg_color(&style_rootmenu, lv_color_make(19, 19, 19));
    lv_style_set_text_color(&style_rootmenu, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_pad_top(&style_rootmenu, 0);
	lv_style_set_pad_bottom(&style_rootmenu, 0);
	lv_style_set_pad_left(&style_rootmenu, 0);
	lv_style_set_pad_right(&style_rootmenu, 0);
	lv_style_set_radius(&style_rootmenu, 0);
	lv_style_set_border_width(&style_rootmenu, 0);
    lv_style_set_border_color(&style_rootmenu, lv_palette_main(LV_PALETTE_RED));

	lv_style_reset(&style_submenu);
    lv_style_init(&style_submenu);
    lv_style_set_bg_color(&style_submenu, lv_color_make(32, 32, 32));
    lv_style_set_border_width(&style_submenu, 0);
    lv_style_set_text_color(&style_submenu, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_pad_top(&style_submenu, 96);
	lv_style_set_pad_bottom(&style_submenu, 0);
	lv_style_set_pad_left(&style_submenu, 48);
	lv_style_set_pad_right(&style_submenu, 0);
	lv_style_set_radius(&style_submenu, 0);
	lv_style_set_align(&style_submenu,LV_ALIGN_TOP_LEFT );

	lv_style_reset(&style_pb);
    lv_style_init(&style_pb);
    lv_style_set_border_width(&style_pb, 3);
    lv_style_set_border_color(&style_pb, lv_palette_main(LV_PALETTE_RED));
	lv_style_set_radius(&style_pb, 0);

	lv_style_reset(&style_pb_dark);
    lv_style_init(&style_pb_dark);
    lv_style_set_border_width(&style_pb_dark, 3);
    lv_style_set_border_color(&style_pb_dark, lv_color_make(32, 32, 32));
	lv_style_set_radius(&style_pb_dark, 0);

    lv_style_reset(&style_context);
    lv_style_init(&style_context);
//    lv_style_set_border_color(&style_context, lv_palette_main(LV_PALETTE_GREEN));
    lv_style_set_border_width(&style_context, 0);
    lv_style_set_bg_color(&style_context, lv_color_make(32, 32, 32));
    lv_style_set_text_color(&style_context, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_pad_top(&style_context, 0);
	lv_style_set_pad_bottom(&style_context, 0);
	lv_style_set_pad_left(&style_context, 0);
	lv_style_set_pad_right(&style_context, 0);
	lv_style_set_radius(&style_context, 0);

    lv_style_reset(&style_subpage);
    lv_style_init(&style_subpage);
    lv_style_set_border_color(&style_subpage, lv_palette_main(LV_PALETTE_GREEN));
	lv_style_set_pad_top(&style_subpage, 0);
	lv_style_set_pad_bottom(&style_subpage, 0);
	lv_style_set_pad_left(&style_subpage, 0);
	lv_style_set_pad_right(&style_subpage, 0);
	lv_style_set_radius(&style_subpage, 0);

    lv_style_reset(&style_statusbar);
    lv_style_init(&style_statusbar);
    lv_style_set_bg_color(&style_statusbar, lv_color_make(19, 19, 19));
   // lv_style_set_border_width(&style_statusbar, 2);
   // lv_style_set_border_color(&style_statusbar, lv_palette_main(LV_PALETTE_RED));
   // lv_style_set_text_color(&style_statusbar, lv_color_make(0xff, 0xff, 0xff));



    lv_style_reset(&style_silder_main);
    lv_style_init(&style_silder_main);
    lv_style_set_bg_opa(&style_silder_main, LV_OPA_COVER);
    lv_style_set_bg_color(&style_silder_main, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_radius(&style_silder_main, 0);

    lv_style_reset(&style_silder_select);
    lv_style_init(&style_silder_select);
    lv_style_set_bg_opa(&style_silder_select, LV_OPA_COVER);
    lv_style_set_bg_color(&style_silder_select, lv_color_make(0x0 , 0xff, 0x0));
    lv_style_set_radius(&style_silder_select, 0);

    lv_style_reset(&style_silder_indicator);
    lv_style_init(&style_silder_indicator);
    lv_style_set_bg_opa(&style_silder_indicator, LV_OPA_COVER);
    lv_style_set_bg_color(&style_silder_indicator, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_radius(&style_silder_indicator, 0);

    lv_style_reset(&style_silder_knob);
    lv_style_init(&style_silder_knob);
    lv_style_set_bg_opa(&style_silder_knob, LV_OPA_COVER);
    lv_style_set_bg_color(&style_silder_knob, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_radius(&style_silder_knob, 0);
    lv_style_set_pad_top(&style_silder_knob, 20); /*Makes the knob larger*/
    lv_style_set_pad_bottom(&style_silder_knob, 20); /*Makes the knob larger*/
    lv_style_set_pad_left(&style_silder_knob, 2); /*Makes the knob larger*/
    lv_style_set_pad_right(&style_silder_knob, 2); /*Makes the knob larger*/

    lv_style_reset(&style_silder_pressed_color);
    lv_style_init(&style_silder_pressed_color);
    lv_style_set_bg_color(&style_silder_pressed_color, lv_palette_darken(LV_PALETTE_CYAN, 2));

    lv_style_reset(&style_scan);
    lv_style_init(&style_scan);
    lv_style_set_bg_color(&style_scan, lv_color_make(255, 0, 0));
    lv_style_set_border_width(&style_scan, 0);

    lv_style_reset(&style_select);
    lv_style_init(&style_select);
    lv_style_set_bg_color(&style_select, lv_color_hex(0x646464));
    lv_style_set_bg_opa(&style_select, LV_OPA_COVER);
    lv_style_set_border_width(&style_select, 0);
    lv_style_set_radius(&style_select, 0);

    lv_style_reset(&style_osd);
    lv_style_init(&style_osd);
    lv_style_set_bg_color(&style_osd, lv_color_hex(0x7f7f7f));
    lv_style_set_border_width(&style_osd, 0);
    lv_style_set_radius(&style_osd, 0);

	return 0;
}
