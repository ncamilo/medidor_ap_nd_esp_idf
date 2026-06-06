#include "ui_app.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "app_state.h"

static const char *TAG = "ui_app";

esp_err_t ui_app_init(void)
{
    // TODO: migrar inicializacao real de display, touch e LVGL.
    ESP_LOGI(TAG, "UI stub inicializada");
    return ESP_OK;
}

void ui_app_task(void *arg)
{
    (void)arg;

    while (true) {
        app_state_t state = {};
        if (app_state_get(&state) == ESP_OK) {
            ESP_LOGI(
                TAG,
                "UI state: altura=%.3f peso=%.3f rgb=%d neon=%d orient=%d",
                state.person_height_m,
                state.weight_kg,
                state.rgb_enabled,
                state.neon_enabled,
                state.orientation
            );
        }

        // vTaskDelay(pdMS_TO_TICKS(2000));
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}