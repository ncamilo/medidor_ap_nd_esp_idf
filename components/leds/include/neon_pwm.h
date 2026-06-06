#pragma once

#include <stdbool.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t neon_pwm_init(void);
esp_err_t neon_pwm_set_enabled(bool enabled);
esp_err_t neon_pwm_set_duty_percent(uint8_t duty_percent);

#ifdef __cplusplus
}
#endif