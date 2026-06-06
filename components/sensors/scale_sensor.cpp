#include "scale_sensor.h"
#include "esp_check.h"

#include <ctype.h>
#include <stdint.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/uart.h"
#include "esp_log.h"

#include "app_state.h"
#include "board_io.h"
#include "board_pins.h"

static const char *TAG = "scale_sensor";

static bool s_uart_ready = false;

static bool parse_weight_frame(const uint8_t *frame, size_t len, float *out_weight)
{
    if (frame == nullptr || out_weight == nullptr || len == 0) {
        return false;
    }

    char digits[6] = {};
    size_t count = 0;

    for (size_t i = 0; i < len && count < 5; i++) {
        if (isdigit(frame[i])) {
            digits[count++] = (char)frame[i];
        }
    }

    if (count != 5) {
        return false;
    }

    int value = atoi(digits);

    // TODO: confirmar regra div100/div10 conforme DIP e firmware original.
    *out_weight = (float)value / 100.0f;
    return true;
}

esp_err_t scale_sensor_init(void)
{
    if (!board_gpio_is_valid(BOARD_PIN_SCALE_TX) || !board_gpio_is_valid(BOARD_PIN_SCALE_RX)) {
        ESP_LOGW(TAG, "UART balanca nao configurada em board_pins.h");
        s_uart_ready = false;
        return ESP_OK;
    }

    uart_config_t config = {};
    config.baud_rate = BOARD_SCALE_BAUDRATE;
    config.data_bits = UART_DATA_8_BITS;
    config.parity = UART_PARITY_DISABLE;
    config.stop_bits = UART_STOP_BITS_1;
    config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
    config.source_clk = UART_SCLK_DEFAULT;

    ESP_ERROR_CHECK(uart_driver_install(BOARD_UART_SCALE, 2048, 0, 0, nullptr, 0));
    ESP_ERROR_CHECK(uart_param_config(BOARD_UART_SCALE, &config));
    ESP_ERROR_CHECK(uart_set_pin(BOARD_UART_SCALE, BOARD_PIN_SCALE_TX, BOARD_PIN_SCALE_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    s_uart_ready = true;
    ESP_LOGI(TAG, "UART balanca inicializada");

    return ESP_OK;
}

esp_err_t scale_sensor_get_weight_kg(float *out_weight_kg)
{
    if (out_weight_kg == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    app_state_t state = {};
    ESP_RETURN_ON_ERROR(app_state_get(&state), TAG, "Falha ao ler estado");
    *out_weight_kg = state.weight_kg;

    return ESP_OK;
}

void scale_sensor_task(void *arg)
{
    (void)arg;

    uint8_t frame[64] = {};
    size_t frame_len = 0;
    bool in_frame = false;

    while (true) {
        if (!s_uart_ready) {
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }

        uint8_t byte = 0;
        int len = uart_read_bytes(BOARD_UART_SCALE, &byte, 1, pdMS_TO_TICKS(100));

        if (len <= 0) {
            continue;
        }

        if (byte == 0x02) {
            in_frame = true;
            frame_len = 0;
            continue;
        }

        if (byte == 0x03 && in_frame) {
            float weight = 0.0f;

            if (parse_weight_frame(frame, frame_len, &weight)) {
                app_state_set_weight(weight);
                ESP_LOGI(TAG, "Peso atualizado: %.3f kg", weight);
            }

            in_frame = false;
            frame_len = 0;
            continue;
        }

        if (in_frame && frame_len < sizeof(frame)) {
            frame[frame_len++] = byte;
        }
    }
}