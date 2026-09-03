#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t ui_app_init(void);
void ui_app_task(void *arg);

#ifdef __cplusplus
}
#endif
