#pragma once

#include "esp_err.h"
#include "app_state.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t calibration_storage_init(void);
esp_err_t calibration_storage_load(app_calibration_t *out_calibration);
esp_err_t calibration_storage_save(const app_calibration_t *calibration);
esp_err_t calibration_storage_reset(void);

#ifdef __cplusplus
}
#endif