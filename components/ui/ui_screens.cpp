#include "ui_screens.h"

#include <stdio.h>
#include <string.h>

#include "lvgl.h"
#include "lvgl_v8_port.h"

enum UiOrientation {
    UI_LANDSCAPE = 0,
    UI_PORTRAIT = 1,
};

enum UiMeasurementMode {
    UI_MODE_BOTH = 0,
    UI_MODE_WEIGHT_ONLY = 1,
    UI_MODE_HEIGHT_ONLY = 2,
};

static lv_obj_t *s_label_height_title = nullptr;
static lv_obj_t *s_label_height_value = nullptr;
static lv_obj_t *s_label_weight_title = nullptr;
static lv_obj_t *s_label_weight_value = nullptr;
static lv_obj_t *s_status_label = nullptr;

static int s_screen_width = 480;
static int s_screen_height = 320;
static int s_orientation = UI_LANDSCAPE;
static int s_measurement_mode = UI_MODE_BOTH;
static bool s_calibrated = false;

static lv_style_t s_style_title;
static lv_style_t s_style_value;
static bool s_styles_ready = false;

static void ui_clear_widget_refs(void)
{
    s_label_height_title = nullptr;
    s_label_height_value = nullptr;
    s_label_weight_title = nullptr;
    s_label_weight_value = nullptr;
    s_status_label = nullptr;
}

static void ui_init_styles(void)
{
    if (s_styles_ready) {
        return;
    }

    lv_style_init(&s_style_title);
    lv_style_set_text_font(&s_style_title, &lv_font_montserrat_32);
    lv_style_set_text_color(&s_style_title, lv_color_hex(0x003366));

    lv_style_init(&s_style_value);
    lv_style_set_text_font(&s_style_value, &lv_font_montserrat_40);
    lv_style_set_text_color(&s_style_value, lv_color_hex(0x003366));

    s_styles_ready = true;
}

static void inherit_bg(lv_obj_t *obj)
{
    lv_obj_set_style_bg_opa(obj, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(obj, 0, 0);
    lv_obj_set_style_outline_width(obj, 0, 0);
    lv_obj_set_style_shadow_width(obj, 0, 0);
    lv_obj_set_style_radius(obj, 0, 0);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
}

static bool mode_has_height(void)
{
    return s_measurement_mode == UI_MODE_BOTH || s_measurement_mode == UI_MODE_HEIGHT_ONLY;
}

static bool mode_has_weight(void)
{
    return s_measurement_mode == UI_MODE_BOTH || s_measurement_mode == UI_MODE_WEIGHT_ONLY;
}

static void ui_request_height_measurement(lv_event_t *)
{
    if (s_status_label) {
        lv_label_set_text(s_status_label, "Status: medir altura acionado");
        lv_obj_clear_flag(s_status_label, LV_OBJ_FLAG_HIDDEN);
    }

    if (s_label_height_value) {
        lv_label_set_text(s_label_height_value, "...");
    }
}

static void ui_show_password_screen(void);

static void create_hidden_calibration_button(lv_obj_t *parent)
{
    lv_obj_t *btn = lv_btn_create(parent);
    lv_obj_set_size(btn, 45, 45);
    lv_obj_align(btn, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_style_bg_opa(btn, LV_OPA_TRANSP, 0);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_CLICK_FOCUSABLE);

    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, LV_SYMBOL_SETTINGS);
    lv_obj_center(label);

    lv_obj_add_event_cb(
        btn,
        [](lv_event_t *) {
            ui_show_password_screen();
        },
        LV_EVENT_CLICKED,
        nullptr
    );
}

static lv_obj_t *create_main_screen(void)
{
    ui_clear_widget_refs();
    ui_init_styles();

    lv_obj_t *scr = lv_obj_create(nullptr);
    lv_obj_clear_flag(scr, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(scr, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_color(scr, lv_color_hex(0xF0F8FF), 0);

    lv_obj_t *header = lv_obj_create(scr);
    lv_obj_set_size(header, s_screen_width, 60);
    lv_obj_clear_flag(header, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(header, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_color(header, lv_color_hex(0x001144), 0);
    lv_obj_set_style_border_width(header, 0, 0);
    lv_obj_align(header, LV_ALIGN_TOP_MID, 0, 0);

    lv_obj_t *header_label = lv_label_create(header);
    lv_label_set_text(header_label, "BioTriagem");
    lv_obj_set_style_text_color(header_label, lv_color_white(), 0);
    lv_obj_set_style_text_font(header_label, &lv_font_montserrat_24, 0);
    lv_obj_center(header_label);

    const int margin_x = 10;
    const int margin_y = 60;

    if (s_orientation == UI_PORTRAIT) {
        const lv_coord_t screen_h = lv_disp_get_ver_res(lv_disp_get_default());
        const lv_coord_t button_h = 60;
        const lv_coord_t button_bottom_margin = 5;
        const lv_coord_t top_margin = margin_y;
        const lv_coord_t group_gap = 24;

        lv_coord_t line_h = (lv_coord_t)lv_font_get_line_height(&lv_font_montserrat_40);
        if (line_h <= 0) {
            line_h = 44;
        }

        int blocks = 0;
        if (mode_has_height()) {
            blocks++;
        }
        if (mode_has_weight()) {
            blocks++;
        }

        const lv_coord_t content_h =
            blocks * (2 * line_h) + ((blocks > 1 ? blocks - 1 : 0) * group_gap);

        const lv_coord_t usable_h = screen_h - button_h - button_bottom_margin - top_margin;
        lv_coord_t y = top_margin + (usable_h - content_h) / 2;
        if (y < top_margin) {
            y = top_margin;
        }

        if (mode_has_height()) {
            s_label_height_title = lv_label_create(scr);
            lv_label_set_text(s_label_height_title, "Altura:");
            lv_obj_add_style(s_label_height_title, &s_style_title, 0);
            lv_obj_set_style_text_font(s_label_height_title, &lv_font_montserrat_40, 0);
            lv_obj_align(s_label_height_title, LV_ALIGN_TOP_MID, 0, y);

            y += line_h;

            s_label_height_value = lv_label_create(scr);
            lv_label_set_text(s_label_height_value, "--");
            lv_obj_add_style(s_label_height_value, &s_style_value, 0);
            lv_obj_set_style_text_font(s_label_height_value, &lv_font_montserrat_40, 0);
            lv_obj_align(s_label_height_value, LV_ALIGN_TOP_MID, 0, y);

            y += line_h + group_gap;
        }

        if (mode_has_weight()) {
            s_label_weight_title = lv_label_create(scr);
            lv_label_set_text(s_label_weight_title, "Peso:");
            lv_obj_add_style(s_label_weight_title, &s_style_title, 0);
            lv_obj_set_style_text_font(s_label_weight_title, &lv_font_montserrat_40, 0);
            lv_obj_align(s_label_weight_title, LV_ALIGN_TOP_MID, 0, y);

            y += line_h;

            s_label_weight_value = lv_label_create(scr);
            lv_label_set_text(s_label_weight_value, "--");
            lv_obj_add_style(s_label_weight_value, &s_style_value, 0);
            lv_obj_set_style_text_font(s_label_weight_value, &lv_font_montserrat_40, 0);
            lv_obj_align(s_label_weight_value, LV_ALIGN_TOP_MID, 0, y);
        }
    } else {
        lv_obj_t *main = lv_obj_create(scr);
        inherit_bg(main);
        lv_obj_set_size(main, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_flex_flow(main, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_style_pad_gap(main, 20, 0);
        lv_obj_align(main, LV_ALIGN_TOP_MID, 0, margin_y);

        if (mode_has_height()) {
            lv_obj_t *row = lv_obj_create(main);
            inherit_bg(row);
            lv_obj_set_size(row, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
            lv_obj_set_style_pad_gap(row, 8, 0);

            s_label_height_title = lv_label_create(row);
            lv_label_set_text(s_label_height_title, "Altura:");
            lv_obj_add_style(s_label_height_title, &s_style_title, 0);
            lv_obj_set_style_text_font(s_label_height_title, &lv_font_montserrat_40, 0);

            s_label_height_value = lv_label_create(row);
            lv_label_set_text(s_label_height_value, "--");
            lv_obj_add_style(s_label_height_value, &s_style_value, 0);
            lv_obj_set_style_text_font(s_label_height_value, &lv_font_montserrat_40, 0);
        }

        if (mode_has_weight()) {
            lv_obj_t *row = lv_obj_create(main);
            inherit_bg(row);
            lv_obj_set_size(row, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
            lv_obj_set_style_pad_gap(row, 8, 0);

            s_label_weight_title = lv_label_create(row);
            lv_label_set_text(s_label_weight_title, "Peso:");
            lv_obj_add_style(s_label_weight_title, &s_style_title, 0);
            lv_obj_set_style_text_font(s_label_weight_title, &lv_font_montserrat_40, 0);

            s_label_weight_value = lv_label_create(row);
            lv_label_set_text(s_label_weight_value, "--");
            lv_obj_add_style(s_label_weight_value, &s_style_value, 0);
            lv_obj_set_style_text_font(s_label_weight_value, &lv_font_montserrat_40, 0);
        }
    }

    s_status_label = lv_label_create(scr);
    lv_label_set_text(s_status_label, "Status: aguardando");
    lv_obj_set_style_text_color(s_status_label, lv_color_hex(0x003366), 0);
    lv_obj_set_style_text_font(s_status_label, &lv_font_montserrat_20, 0);
    lv_obj_align(s_status_label, LV_ALIGN_BOTTOM_MID, 0, -72);

    if (mode_has_height()) {
        lv_obj_t *btn = lv_btn_create(scr);
        lv_obj_set_size(btn, s_screen_width - 2 * margin_x, 60);
        lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -5);
        lv_obj_add_event_cb(btn, ui_request_height_measurement, LV_EVENT_CLICKED, nullptr);

        lv_obj_t *label = lv_label_create(btn);
        lv_label_set_text(label, LV_SYMBOL_EYE_OPEN " Medir Altura");
        lv_obj_set_style_text_font(label, &lv_font_montserrat_24, 0);
        lv_obj_center(label);
    }

    create_hidden_calibration_button(scr);

    return scr;
}

struct PasswordScreenData {
    lv_obj_t *password_box;
    lv_obj_t *status_label;
};

static void clear_status_cb(lv_timer_t *timer)
{
    lv_obj_t *label = static_cast<lv_obj_t *>(timer->user_data);
    if (label) {
        lv_obj_add_flag(label, LV_OBJ_FLAG_HIDDEN);
    }
    lv_timer_del(timer);
}

static void ui_load_main_screen(void)
{
    lv_scr_load(create_main_screen());
}

static void ui_show_calibration_screen(void);

static void ui_show_password_screen(void)
{
    ui_clear_widget_refs();

    static PasswordScreenData data = {};

    lv_obj_t *scr = lv_obj_create(nullptr);
    lv_obj_clear_flag(scr, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(scr, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_color(scr, lv_color_hex(0xF0F8FF), 0);

    lv_obj_t *header = lv_obj_create(scr);
    lv_obj_set_size(header, s_screen_width, 60);
    lv_obj_clear_flag(header, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(header, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_color(header, lv_color_hex(0x001144), 0);
    lv_obj_set_style_border_width(header, 0, 0);
    lv_obj_align(header, LV_ALIGN_TOP_MID, 0, 0);

    lv_obj_t *title = lv_label_create(header);
    lv_label_set_text(title, LV_SYMBOL_WARNING " Senha:");
    lv_obj_set_style_text_color(title, lv_color_white(), 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_24, 0);
    lv_obj_align(title, LV_ALIGN_LEFT_MID, 10, 0);

    lv_obj_t *textarea = lv_textarea_create(header);
    lv_textarea_set_password_mode(textarea, true);
    lv_textarea_set_one_line(textarea, true);
    lv_textarea_set_max_length(textarea, 6);
    lv_obj_set_width(textarea, 100);
    lv_obj_set_style_text_font(textarea, &lv_font_montserrat_24, 0);
    lv_obj_align_to(textarea, title, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

    lv_obj_t *status = lv_label_create(scr);
    lv_label_set_text(status, "");
    lv_obj_set_style_text_color(status, lv_color_hex(0xFF0000), 0);
    lv_obj_set_style_text_font(status, &lv_font_montserrat_24, 0);
    lv_obj_align(status, LV_ALIGN_TOP_MID, 0, 80);
    lv_obj_add_flag(status, LV_OBJ_FLAG_HIDDEN);

    data.password_box = textarea;
    data.status_label = status;

    lv_obj_t *btn_ok = lv_btn_create(scr);
    lv_obj_set_size(btn_ok, 90, 40);
    lv_obj_align(btn_ok, LV_ALIGN_BOTTOM_RIGHT, -10, -10);

    lv_obj_t *lbl_ok = lv_label_create(btn_ok);
    lv_label_set_text(lbl_ok, LV_SYMBOL_DOWNLOAD " OK");
    lv_obj_center(lbl_ok);

    lv_obj_add_event_cb(
        btn_ok,
        [](lv_event_t *event) {
            auto *d = static_cast<PasswordScreenData *>(lv_event_get_user_data(event));
            if (!d || !d->password_box) {
                return;
            }

            const char *password = lv_textarea_get_text(d->password_box);
            if (password && strcmp(password, "3412") == 0) {
                ui_show_calibration_screen();
                return;
            }

            if (d->status_label) {
                lv_label_set_text(d->status_label, LV_SYMBOL_WARNING " Senha incorreta!");
                lv_obj_clear_flag(d->status_label, LV_OBJ_FLAG_HIDDEN);
                lv_obj_move_foreground(d->status_label);
                lv_timer_create(clear_status_cb, 2000, d->status_label);
            }
        },
        LV_EVENT_CLICKED,
        &data
    );

    lv_obj_t *btn_back = lv_btn_create(scr);
    lv_obj_set_size(btn_back, 90, 40);
    lv_obj_align(btn_back, LV_ALIGN_BOTTOM_LEFT, 10, -10);

    lv_obj_t *lbl_back = lv_label_create(btn_back);
    lv_label_set_text(lbl_back, LV_SYMBOL_HOME " Voltar");
    lv_obj_center(lbl_back);

    lv_obj_add_event_cb(
        btn_back,
        [](lv_event_t *) {
            ui_load_main_screen();
        },
        LV_EVENT_CLICKED,
        nullptr
    );

    lv_obj_t *keyboard = lv_keyboard_create(scr);
    lv_keyboard_set_mode(keyboard, LV_KEYBOARD_MODE_NUMBER);
    lv_keyboard_set_textarea(keyboard, textarea);
    lv_obj_align(keyboard, LV_ALIGN_BOTTOM_MID, 0, -60);

    lv_obj_add_event_cb(
        keyboard,
        [](lv_event_t *) {
            ui_load_main_screen();
        },
        LV_EVENT_CANCEL,
        nullptr
    );

    lv_scr_load(scr);
}

static void show_calibration_status(lv_obj_t *label, const char *text, lv_color_t color)
{
    if (!label) {
        return;
    }

    lv_label_set_text(label, text);
    lv_obj_set_style_text_color(label, color, 0);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_24, 0);
    lv_obj_set_width(label, lv_pct(90));
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_obj_clear_flag(label, LV_OBJ_FLAG_HIDDEN);
    lv_obj_move_foreground(label);

    lv_timer_create(clear_status_cb, 2000, label);
}

static void ui_show_calibration_screen(void)
{
    ui_clear_widget_refs();

    lv_obj_t *scr = lv_obj_create(nullptr);
    lv_obj_clear_flag(scr, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(scr, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_color(scr, lv_color_hex(0xF0F8FF), 0);

    lv_obj_t *title = lv_label_create(scr);
    lv_label_set_text(title, LV_SYMBOL_SETTINGS " Calibrar Sensor");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_24, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

    lv_obj_t *instr = lv_label_create(scr);
    lv_label_set_text(instr, "Deixe o sensor livre\ne toque em Calibrar");
    lv_obj_set_style_text_font(instr, &lv_font_montserrat_24, 0);
    lv_obj_align(instr, LV_ALIGN_TOP_MID, 0, 50);

    lv_obj_t *status = lv_label_create(scr);
    lv_label_set_text(status, "");
    lv_obj_add_flag(status, LV_OBJ_FLAG_HIDDEN);
    lv_obj_align(status, LV_ALIGN_TOP_MID, 0, 100);
    lv_obj_set_style_text_font(status, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(status, lv_color_hex(0xCC0000), 0);
    lv_obj_set_width(status, lv_pct(90));
    lv_label_set_long_mode(status, LV_LABEL_LONG_WRAP);

    lv_obj_t *btn_calibrate = lv_btn_create(scr);
    lv_obj_set_size(btn_calibrate, 200, 50);
    lv_obj_align(btn_calibrate, LV_ALIGN_CENTER, 0, 20);

    lv_obj_t *lbl_calibrate = lv_label_create(btn_calibrate);
    lv_obj_set_style_text_font(lbl_calibrate, &lv_font_montserrat_24, 0);
    lv_label_set_text(lbl_calibrate, "Calibrar Sensor");
    lv_obj_center(lbl_calibrate);

    lv_obj_add_event_cb(
        btn_calibrate,
        [](lv_event_t *event) {
            lv_obj_t *status_label = static_cast<lv_obj_t *>(lv_event_get_user_data(event));
            show_calibration_status(
                status_label,
                LV_SYMBOL_WARNING " Sensor ainda nao integrado",
                lv_color_hex(0xCC0000)
            );
        },
        LV_EVENT_CLICKED,
        status
    );

    lv_obj_t *btn_back = lv_btn_create(scr);
    lv_obj_set_size(btn_back, 120, 40);
    lv_obj_align(btn_back, LV_ALIGN_BOTTOM_LEFT, 10, -10);

    lv_obj_t *lbl_back = lv_label_create(btn_back);
    lv_label_set_text(lbl_back, LV_SYMBOL_LEFT " Voltar");
    lv_obj_set_style_text_font(lbl_back, &lv_font_montserrat_24, 0);
    lv_obj_center(lbl_back);

    lv_obj_add_event_cb(
        btn_back,
        [](lv_event_t *) {
            ui_load_main_screen();
        },
        LV_EVENT_CLICKED,
        nullptr
    );

    lv_scr_load(scr);
}

esp_err_t ui_screens_create(void)
{
    if (!lvgl_port_lock(-1)) {
        return ESP_FAIL;
    }

    lv_scr_load(create_main_screen());
    lv_refr_now(nullptr);

    lvgl_port_unlock();
    return ESP_OK;
}

void ui_screens_set_height(float height_m)
{
    char buf[32];

    if (height_m > 0.0f) {
        snprintf(buf, sizeof(buf), "%.2f m", height_m);
    } else {
        snprintf(buf, sizeof(buf), "--");
    }

    if (!lvgl_port_lock(100)) {
        return;
    }

    if (s_label_height_value) {
        lv_label_set_text(s_label_height_value, buf);
        lv_obj_set_style_text_color(
            s_label_height_value,
            s_calibrated ? lv_color_hex(0x003366) : lv_color_hex(0xCC0000),
            0
        );
    }

    lvgl_port_unlock();
}

void ui_screens_set_weight(float weight_kg)
{
    char buf[32];

    if (weight_kg > 0.0f) {
        snprintf(buf, sizeof(buf), "%.2f kg", weight_kg);
    } else {
        snprintf(buf, sizeof(buf), "--");
    }

    if (!lvgl_port_lock(100)) {
        return;
    }

    if (s_label_weight_value) {
        lv_label_set_text(s_label_weight_value, buf);
        lv_obj_set_style_text_color(
            s_label_weight_value,
            s_calibrated ? lv_color_hex(0x003366) : lv_color_hex(0xCC0000),
            0
        );
    }

    lvgl_port_unlock();
}

void ui_screens_set_calibrated(bool calibrated)
{
    s_calibrated = calibrated;
}

void ui_screens_set_measurement_mode(int mode)
{
    if (mode < UI_MODE_BOTH || mode > UI_MODE_HEIGHT_ONLY) {
        mode = UI_MODE_BOTH;
    }

    if (!lvgl_port_lock(100)) {
        return;
    }

    if (s_measurement_mode != mode) {
        s_measurement_mode = mode;
        lv_scr_load(create_main_screen());
    }

    lvgl_port_unlock();
}

void ui_screens_set_orientation(int orientation)
{
    if (orientation != UI_LANDSCAPE && orientation != UI_PORTRAIT) {
        orientation = UI_LANDSCAPE;
    }

    if (!lvgl_port_lock(100)) {
        return;
    }

    if (s_orientation != orientation) {
        s_orientation = orientation;

        if (orientation == UI_LANDSCAPE) {
            s_screen_width = 480;
            s_screen_height = 320;
        } else {
            s_screen_width = 320;
            s_screen_height = 480;
        }

        lv_disp_t *display = lv_disp_get_default();
        if (display) {
            lv_disp_set_rotation(
                display,
                orientation == UI_LANDSCAPE ? LV_DISP_ROT_90 : LV_DISP_ROT_NONE
            );
        }

        lv_scr_load(create_main_screen());
    }

    lvgl_port_unlock();
}

void ui_screens_set_status(const char *text)
{
    if (!text || !*text) {
        text = "aguardando";
    }

    char buf[96];
    snprintf(buf, sizeof(buf), "Status: %s", text);

    if (!lvgl_port_lock(100)) {
        return;
    }

    if (s_status_label) {
        lv_label_set_text(s_status_label, buf);
    }

    lvgl_port_unlock();
}