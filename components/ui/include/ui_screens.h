#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t ui_screens_create(void);

void ui_screens_set_height(float height_m);
void ui_screens_set_weight(float weight_kg);
void ui_screens_set_status(const char *text);
void ui_screens_set_measurement_mode(int mode);
void ui_screens_set_orientation(int orientation);

#ifdef __cplusplus
}
#endif