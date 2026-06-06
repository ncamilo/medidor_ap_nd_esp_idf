#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t scale_sensor_init(void);
esp_err_t scale_sensor_get_weight_kg(float *out_weight_kg);
void scale_sensor_task(void *arg);

#ifdef __cplusplus
}
#endif