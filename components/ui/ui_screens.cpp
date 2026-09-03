#include "ui_screens.h"

#include <stdio.h>

#include "lvgl.h"
#include "lvgl_v8_port.h"

static lv_obj_t *s_height_label = nullptr;
static lv_obj_t *s_weight_label = nullptr;
static lv_obj_t *s_status_label = nullptr;
static lv_obj_t *s_mode_label = nullptr;

static void set_label_text(lv_obj_t *label, const char *text)
{
    if (label == nullptr || text == nullptr) {
        return;
    }

    lv_label_set_text(label, text);
    lv_obj_invalidate(label);
}

esp_err_t ui_screens_create(void)
{
    if (!lvgl_port_lock(-1)) {
        return ESP_FAIL;
    }

    lv_obj_t *scr = lv_scr_act();
    lv_obj_clean(scr);

    lv_obj_set_style_bg_color(scr, lv_color_hex(0x061826), 0);

    lv_obj_t *title = lv_label_create(scr);
    lv_label_set_text(title, "BioTriagem");
    lv_obj_set_style_text_color(title, lv_color_white(), 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_24, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 18);

    s_status_label = lv_label_create(scr);
    lv_label_set_text(s_status_label, "Status: aguardando");
    lv_obj_set_style_text_color(s_status_label, lv_color_hex(0x00D9FF), 0);
    lv_obj_align(s_status_label, LV_ALIGN_TOP_MID, 0, 56);

    lv_obj_t *card = lv_obj_create(scr);
    lv_obj_set_size(card, 290, 210);
    lv_obj_align(card, LV_ALIGN_CENTER, 0, -10);
    lv_obj_set_style_bg_color(card, lv_color_hex(0x0B2A3D), 0);
    lv_obj_set_style_border_color(card, lv_color_hex(0x1D8AA8), 0);
    lv_obj_set_style_border_width(card, 2, 0);
    lv_obj_set_style_radius(card, 16, 0);
    lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE);

    s_height_label = lv_label_create(card);
    lv_label_set_text(s_height_label, "Altura: ---.--- m");
    lv_obj_set_style_text_color(s_height_label, lv_color_white(), 0);
    lv_obj_set_style_text_font(s_height_label, &lv_font_montserrat_24, 0);
    lv_obj_align(s_height_label, LV_ALIGN_TOP_LEFT, 12, 26);

    s_weight_label = lv_label_create(card);
    lv_label_set_text(s_weight_label, "Peso: ---.--- kg");
    lv_obj_set_style_text_color(s_weight_label, lv_color_white(), 0);
    lv_obj_set_style_text_font(s_weight_label, &lv_font_montserrat_24, 0);
    lv_obj_align(s_weight_label, LV_ALIGN_TOP_LEFT, 12, 86);

    s_mode_label = lv_label_create(card);
    lv_label_set_text(s_mode_label, "Modo: altura/peso");
    lv_obj_set_style_text_color(s_mode_label, lv_color_hex(0xC8F7FF), 0);
    lv_obj_align(s_mode_label, LV_ALIGN_TOP_LEFT, 14, 152);

    lv_obj_t *btn = lv_btn_create(scr);
    lv_obj_set_size(btn, 220, 58);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -28);
    lv_obj_set_style_radius(btn, 14, 0);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x0077AA), 0);

    lv_obj_t *btn_label = lv_label_create(btn);
    lv_label_set_text(btn_label, "MEDIR ALTURA");
    lv_obj_set_style_text_color(btn_label, lv_color_white(), 0);
    lv_obj_center(btn_label);

    lv_obj_invalidate(scr);
    lv_refr_now(nullptr);

    lvgl_port_unlock();
    return ESP_OK;
}

void ui_screens_set_height(float height_m)
{
    char buf[48];

    if (height_m > 0.0f) {
        snprintf(buf, sizeof(buf), "Altura: %.3f m", height_m);
    } else {
        snprintf(buf, sizeof(buf), "Altura: ---.--- m");
    }

    if (!lvgl_port_lock(100)) {
        return;
    }

    set_label_text(s_height_label, buf);

    lvgl_port_unlock();
}

void ui_screens_set_weight(float weight_kg)
{
    char buf[48];

    if (weight_kg > 0.0f) {
        snprintf(buf, sizeof(buf), "Peso: %.3f kg", weight_kg);
    } else {
        snprintf(buf, sizeof(buf), "Peso: ---.--- kg");
    }

    if (!lvgl_port_lock(100)) {
        return;
    }

    set_label_text(s_weight_label, buf);

    lvgl_port_unlock();
}

void ui_screens_set_status(const char *text)
{
    char buf[96];

    if (text == nullptr || text[0] == '\0') {
        text = "aguardando";
    }

    snprintf(buf, sizeof(buf), "Status: %s", text);

    if (!lvgl_port_lock(100)) {
        return;
    }

    set_label_text(s_status_label, buf);

    lvgl_port_unlock();
}

void ui_screens_set_measurement_mode(int mode)
{
    char buf[48];

    switch (mode) {
        case 0:
            snprintf(buf, sizeof(buf), "Modo: altura/peso");
            break;

        case 1:
            snprintf(buf, sizeof(buf), "Modo: altura");
            break;

        default:
            snprintf(buf, sizeof(buf), "Modo: %d", mode);
            break;
    }

    if (!lvgl_port_lock(100)) {
        return;
    }

    set_label_text(s_mode_label, buf);

    lvgl_port_unlock();
}

void ui_screens_set_orientation(int orientation)
{
    // Por enquanto não muda layout.
    // Vamos usar depois para portrait/landscape real.
    (void)orientation;
}