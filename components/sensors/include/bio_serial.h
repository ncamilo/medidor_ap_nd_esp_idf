#pragma once

#include <stddef.h>
#include <stdint.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t bio_serial_init(void);
esp_err_t bio_serial_write(const uint8_t *data, size_t len);
void bio_serial_task(void *arg);

#ifdef __cplusplus
}
#endif