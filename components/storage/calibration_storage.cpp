#include "calibration_storage.h"
#include "esp_check.h"

#include <string.h>

#include "nvs.h"
#include "nvs_flash.h"

#include "esp_err.h"
#include "esp_log.h"

static const char *TAG = "calibration_storage";

static const char *NVS_NAMESPACE = "calib";
static const char *NVS_KEY_DATA = "data";

static app_calibration_t default_calibration(void)
{
    app_calibration_t calibration = {};
    calibration.calibrated = false;
    calibration.sensor_height_m = 2.10f;
    calibration.dyp_ref_1m = 1.0f;
    return calibration;
}

esp_err_t calibration_storage_init(void)
{
    esp_err_t err = nvs_flash_init();

    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "NVS precisa ser apagada: %s", esp_err_to_name(err));
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }

    ESP_RETURN_ON_ERROR(err, TAG, "Falha ao inicializar NVS");

    ESP_LOGI(TAG, "NVS inicializada");
    return ESP_OK;
}

esp_err_t calibration_storage_load(app_calibration_t *out_calibration)
{
    if (out_calibration == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    nvs_handle_t handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &handle);

    if (err == ESP_ERR_NVS_NOT_FOUND) {
        *out_calibration = default_calibration();
        ESP_LOGW(TAG, "Namespace de calibracao nao encontrado. Usando defaults");
        return ESP_OK;
    }

    ESP_RETURN_ON_ERROR(err, TAG, "Falha ao abrir NVS");

    size_t size = sizeof(app_calibration_t);
    err = nvs_get_blob(handle, NVS_KEY_DATA, out_calibration, &size);
    nvs_close(handle);

    if (err == ESP_ERR_NVS_NOT_FOUND || size != sizeof(app_calibration_t)) {
        *out_calibration = default_calibration();
        ESP_LOGW(TAG, "Calibracao ausente ou invalida. Usando defaults");
        return ESP_OK;
    }

    ESP_RETURN_ON_ERROR(err, TAG, "Falha ao ler calibracao");

    ESP_LOGI(
        TAG,
        "Calibracao: calibrated=%d sensor_height=%.3f dyp_ref_1m=%.3f",
        out_calibration->calibrated,
        out_calibration->sensor_height_m,
        out_calibration->dyp_ref_1m
    );

    return ESP_OK;
}

esp_err_t calibration_storage_save(const app_calibration_t *calibration)
{
    if (calibration == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    nvs_handle_t handle;
    ESP_RETURN_ON_ERROR(nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle), TAG, "Falha ao abrir NVS");

    esp_err_t err = nvs_set_blob(handle, NVS_KEY_DATA, calibration, sizeof(app_calibration_t));
    if (err == ESP_OK) {
        err = nvs_commit(handle);
    }

    nvs_close(handle);

    ESP_RETURN_ON_ERROR(err, TAG, "Falha ao salvar calibracao");

    ESP_LOGI(TAG, "Calibracao salva");
    return ESP_OK;
}

esp_err_t calibration_storage_reset(void)
{
    nvs_handle_t handle;
    ESP_RETURN_ON_ERROR(nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle), TAG, "Falha ao abrir NVS");

    esp_err_t err = nvs_erase_key(handle, NVS_KEY_DATA);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        err = ESP_OK;
    }

    if (err == ESP_OK) {
        err = nvs_commit(handle);
    }

    nvs_close(handle);

    return err;
}