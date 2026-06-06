#include "app_state.h"
#include "esp_check.h"

#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "esp_err.h"
#include "esp_log.h"

static const char *TAG = "app_state";

static SemaphoreHandle_t s_mutex = nullptr;
static app_state_t s_state = {};

static esp_err_t lock_state(void)
{
    if (s_mutex == nullptr) {
        return ESP_ERR_INVALID_STATE;
    }

    if (xSemaphoreTake(s_mutex, pdMS_TO_TICKS(1000)) != pdTRUE) {
        return ESP_ERR_TIMEOUT;
    }

    return ESP_OK;
}

static void unlock_state(void)
{
    xSemaphoreGive(s_mutex);
}

esp_err_t app_state_init(void)
{
    if (s_mutex != nullptr) {
        return ESP_OK;
    }

    s_mutex = xSemaphoreCreateMutex();
    if (s_mutex == nullptr) {
        return ESP_ERR_NO_MEM;
    }

    memset(&s_state, 0, sizeof(s_state));

    s_state.person_height_m = 0.0f;
    s_state.weight_kg = 0.0f;

    s_state.calibration.calibrated = false;
    s_state.calibration.sensor_height_m = 2.10f;
    s_state.calibration.dyp_ref_1m = 1.0f;

    s_state.rgb_enabled = false;
    s_state.neon_enabled = false;

    s_state.rgb_r = 0;
    s_state.rgb_g = 0;
    s_state.rgb_b = 0;

    s_state.measurement_mode = MEASUREMENT_MODE_BOTH;
    s_state.orientation = APP_ORIENTATION_PORTRAIT;

    ESP_LOGI(TAG, "Estado global inicializado");
    return ESP_OK;
}

esp_err_t app_state_get(app_state_t *out_state)
{
    if (out_state == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_RETURN_ON_ERROR(lock_state(), TAG, "Falha ao bloquear estado");
    *out_state = s_state;
    unlock_state();

    return ESP_OK;
}

esp_err_t app_state_set_height(float person_height_m)
{
    ESP_RETURN_ON_ERROR(lock_state(), TAG, "Falha ao bloquear estado");
    s_state.person_height_m = person_height_m;
    unlock_state();

    return ESP_OK;
}

esp_err_t app_state_set_weight(float weight_kg)
{
    ESP_RETURN_ON_ERROR(lock_state(), TAG, "Falha ao bloquear estado");
    s_state.weight_kg = weight_kg;
    unlock_state();

    return ESP_OK;
}

esp_err_t app_state_set_rgb(uint8_t r, uint8_t g, uint8_t b)
{
    ESP_RETURN_ON_ERROR(lock_state(), TAG, "Falha ao bloquear estado");
    s_state.rgb_r = r;
    s_state.rgb_g = g;
    s_state.rgb_b = b;
    unlock_state();

    return ESP_OK;
}

esp_err_t app_state_set_rgb_enabled(bool enabled)
{
    ESP_RETURN_ON_ERROR(lock_state(), TAG, "Falha ao bloquear estado");
    s_state.rgb_enabled = enabled;
    unlock_state();

    return ESP_OK;
}

esp_err_t app_state_set_neon_enabled(bool enabled)
{
    ESP_RETURN_ON_ERROR(lock_state(), TAG, "Falha ao bloquear estado");
    s_state.neon_enabled = enabled;
    unlock_state();

    return ESP_OK;
}

esp_err_t app_state_set_measurement_mode(measurement_mode_t mode)
{
    ESP_RETURN_ON_ERROR(lock_state(), TAG, "Falha ao bloquear estado");
    s_state.measurement_mode = mode;
    unlock_state();

    return ESP_OK;
}

esp_err_t app_state_set_orientation(app_orientation_t orientation)
{
    ESP_RETURN_ON_ERROR(lock_state(), TAG, "Falha ao bloquear estado");
    s_state.orientation = orientation;
    unlock_state();

    return ESP_OK;
}

esp_err_t app_state_set_calibration(const app_calibration_t *calibration)
{
    if (calibration == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_RETURN_ON_ERROR(lock_state(), TAG, "Falha ao bloquear estado");
    s_state.calibration = *calibration;
    unlock_state();

    return ESP_OK;
}

esp_err_t app_state_get_calibration(app_calibration_t *out_calibration)
{
    if (out_calibration == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_RETURN_ON_ERROR(lock_state(), TAG, "Falha ao bloquear estado");
    *out_calibration = s_state.calibration;
    unlock_state();

    return ESP_OK;
}