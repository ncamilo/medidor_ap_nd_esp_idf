#include "lvgl_v8_port.h"

#include "esp_err.h"
#include "esp_log.h"

static const char *TAG = "lvgl_v8_port";

esp_err_t lvgl_v8_port_init_stub(void)
{
    ESP_LOGW(TAG, "LVGL port ainda nao migrado");
    return ESP_ERR_NOT_SUPPORTED;
}