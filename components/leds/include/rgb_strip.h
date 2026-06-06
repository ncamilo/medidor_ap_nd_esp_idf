#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t rgb_strip_init(void);
esp_err_t rgb_strip_set_color(uint8_t r, uint8_t g, uint8_t b);
esp_err_t rgb_strip_set_enabled(bool enabled);

#ifdef __cplusplus
}
#endif