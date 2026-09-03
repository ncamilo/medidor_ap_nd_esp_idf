#include "ui_app.h"
#include "app_state.h"

#include <cstdint>

#include "esp_check.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_display_panel.hpp"
#include "lvgl_v8_port.h"
#include "ui_screens.h"

using namespace esp_panel::board;
using namespace esp_panel::drivers;

static const char *TAG = "ui_app";

#ifndef UI_ENABLE_LCD_DIRECT_TEST
#define UI_ENABLE_LCD_DIRECT_TEST 0
#endif

static Board *s_panel = nullptr;

#if UI_ENABLE_LCD_DIRECT_TEST
static void lcd_direct_color_test(LCD *lcd)
{
    if (lcd == nullptr) {
        ESP_LOGE(TAG, "LCD nulo no teste direto");
        return;
    }

    const int w = lcd->getFrameWidth();
    const int h = lcd->getFrameHeight();

    ESP_LOGI(TAG, "LCD direto: width=%d height=%d", w, h);

    static uint16_t line_buf[480 * 20];

    const uint16_t colors[] = {
        0xFFFF, // branco
        0xF800, // vermelho
        0x07E0, // verde
        0x001F, // azul
        0x0000  // preto
    };

    const int band_h = h / 5;

    for (int band = 0; band < 5; band++) {
        const uint16_t color = colors[band];

        for (int i = 0; i < w * 20; i++) {
            line_buf[i] = color;
        }

        const int y_start = band * band_h;
        const int y_end = (band == 4) ? h : y_start + band_h;

        for (int y = y_start; y < y_end; y += 20) {
            const int rows = ((y + 20) <= y_end) ? 20 : (y_end - y);

            lcd->drawBitmap(
                0,
                y,
                w,
                rows,
                reinterpret_cast<const uint8_t *>(line_buf)
            );
        }
    }

    ESP_LOGI(TAG, "Teste direto LCD enviado");
}
#endif

esp_err_t ui_app_init(void)
{
    ESP_LOGI(TAG, "Inicializando display Viewe");

    s_panel = new Board();
    if (s_panel == nullptr) {
        ESP_LOGE(TAG, "Falha ao alocar Board");
        return ESP_ERR_NO_MEM;
    }

    if (!s_panel->init()) {
        ESP_LOGE(TAG, "Falha em panel->init()");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "panel->init() OK");

    if (!s_panel->begin()) {
        ESP_LOGE(TAG, "Falha em panel->begin()");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "panel->begin() OK");

    auto backlight = s_panel->getBacklight();
    if (backlight != nullptr) {
        backlight->on();
        backlight->setBrightness(200);
        ESP_LOGI(TAG, "Backlight ligado");
    } else {
        ESP_LOGW(TAG, "Backlight nulo");
    }

    auto lcd = s_panel->getLCD();
    auto touch = s_panel->getTouch();

    if (lcd == nullptr) {
        ESP_LOGE(TAG, "LCD nulo apÃ³s begin");
        return ESP_FAIL;
    }
    
#if UI_ENABLE_LCD_DIRECT_TEST
    lcd_direct_color_test(lcd);
    vTaskDelay(pdMS_TO_TICKS(2000));
#endif

    ESP_LOGI(TAG, "Inicializando LVGL");

    if (!lvgl_port_init(lcd, touch)) {
        ESP_LOGE(TAG, "Falha ao iniciar LVGL port");
        return ESP_FAIL;
    }

    ESP_RETURN_ON_ERROR(ui_screens_create(), TAG, "Falha ao criar tela inicial");

    ESP_LOGI(TAG, "Display/LVGL inicializados");
    return ESP_OK;
}

// void ui_app_task(void *arg)
// {
//     (void)arg;
//     vTaskDelete(nullptr);
// }
void ui_app_task(void *arg)
{
    (void)arg;

    app_state_t last = {};
    bool first_update = true;

    while (true) {
        app_state_t state = {};

        if (app_state_get(&state) == ESP_OK) {
            if (first_update || state.person_height_m != last.person_height_m) {
                ui_screens_set_height(state.person_height_m);
            }

            if (first_update || state.weight_kg != last.weight_kg) {
                ui_screens_set_weight(state.weight_kg);
            }

            if (first_update || state.measurement_mode != last.measurement_mode) {
                ui_screens_set_measurement_mode((int)state.measurement_mode);
            }

            if (first_update || state.orientation != last.orientation) {
                ui_screens_set_orientation((int)state.orientation);
            }

            first_update = false;
            last = state;
        }

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
