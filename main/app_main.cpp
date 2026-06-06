#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_err.h"
#include "esp_log.h"

#include "app_state.h"
#include "board_io.h"
#include "calibration_storage.h"
#include "usb_protocol.h"
#include "dyp_sensor.h"
#include "scale_sensor.h"
#include "bio_serial.h"
#include "ui_app.h"
#include "neon_pwm.h"
#include "rgb_strip.h"

static const char *TAG = "app_main";

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "Inicializando firmware ESP-IDF");

    ESP_ERROR_CHECK(app_state_init());
    ESP_ERROR_CHECK(calibration_storage_init());

    app_calibration_t calibration = {};
    esp_err_t cal_err = calibration_storage_load(&calibration);
    if (cal_err == ESP_OK) {
        ESP_ERROR_CHECK(app_state_set_calibration(&calibration));
        ESP_LOGI(TAG, "Calibracao carregada");
    } else {
        ESP_LOGW(TAG, "Calibracao nao carregada: %s", esp_err_to_name(cal_err));
    }

    ESP_ERROR_CHECK(board_io_init());
    ESP_ERROR_CHECK(neon_pwm_init());
    ESP_ERROR_CHECK(rgb_strip_init());
    ESP_ERROR_CHECK(dyp_sensor_init());
    ESP_ERROR_CHECK(scale_sensor_init());
    ESP_ERROR_CHECK(bio_serial_init());
    ESP_ERROR_CHECK(ui_app_init());

    xTaskCreatePinnedToCore(ui_app_task, "ui_app", 4096, nullptr, 5, nullptr, 1);
    xTaskCreatePinnedToCore(usb_protocol_task, "usb_protocol", 4096, nullptr, 6, nullptr, 0);
    xTaskCreatePinnedToCore(scale_sensor_task, "scale_sensor", 4096, nullptr, 5, nullptr, 1);
    xTaskCreatePinnedToCore(bio_serial_task, "bio_serial", 4096, nullptr, 5, nullptr, 1);
    xTaskCreatePinnedToCore(board_io_task, "board_io", 3072, nullptr, 4, nullptr, 0);

    ESP_LOGI(TAG, "Tasks principais criadas");

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}