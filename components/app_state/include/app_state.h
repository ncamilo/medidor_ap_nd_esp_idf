#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    MEASUREMENT_MODE_HEIGHT = 0,
    MEASUREMENT_MODE_WEIGHT = 1,
    MEASUREMENT_MODE_BOTH = 2,
} measurement_mode_t;

typedef enum {
    APP_ORIENTATION_PORTRAIT = 0,
    APP_ORIENTATION_LANDSCAPE = 1,
} app_orientation_t;

typedef struct {
    bool calibrated;
    float sensor_height_m;
    float dyp_ref_1m;
} app_calibration_t;

typedef struct {
    float person_height_m;
    float weight_kg;

    app_calibration_t calibration;

    bool rgb_enabled;
    bool neon_enabled;

    uint8_t rgb_r;
    uint8_t rgb_g;
    uint8_t rgb_b;

    measurement_mode_t measurement_mode;
    app_orientation_t orientation;
} app_state_t;

esp_err_t app_state_init(void);
esp_err_t app_state_get(app_state_t *out_state);

esp_err_t app_state_set_height(float person_height_m);
esp_err_t app_state_set_weight(float weight_kg);

esp_err_t app_state_set_rgb(uint8_t r, uint8_t g, uint8_t b);
esp_err_t app_state_set_rgb_enabled(bool enabled);
esp_err_t app_state_set_neon_enabled(bool enabled);

esp_err_t app_state_set_measurement_mode(measurement_mode_t mode);
esp_err_t app_state_set_orientation(app_orientation_t orientation);

esp_err_t app_state_set_calibration(const app_calibration_t *calibration);
esp_err_t app_state_get_calibration(app_calibration_t *out_calibration);

#ifdef __cplusplus
}
#endif