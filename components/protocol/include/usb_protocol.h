#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t usb_protocol_init(void);
esp_err_t usb_protocol_handle_frame(const char *frame);
void usb_protocol_task(void *arg);

#ifdef __cplusplus
}
#endif