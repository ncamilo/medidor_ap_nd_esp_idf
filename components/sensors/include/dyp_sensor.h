#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t dyp_sensor_init(void);
esp_err_t dyp_sensor_measure_height_m(float *out_height_m);

#ifdef __cplusplus
}
#endif