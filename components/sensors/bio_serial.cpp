#include "bio_serial.h"

#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/uart.h"
#include "esp_log.h"

#include "board_io.h"
#include "board_pins.h"

static const char *TAG = "bio_serial";

static bool s_uart_ready = false;

esp_err_t bio_serial_init(void)
{
    if (!board_gpio_is_valid(BOARD_PIN_BIO_TX) || !board_gpio_is_valid(BOARD_PIN_BIO_RX)) {
        ESP_LOGW(TAG, "UART bioimpedancia nao configurada em board_pins.h");
        s_uart_ready = false;
        return ESP_OK;
    }

    uart_config_t config = {};
    config.baud_rate = BOARD_BIO_BAUDRATE;
    config.data_bits = UART_DATA_8_BITS;
    config.parity = UART_PARITY_DISABLE;
    config.stop_bits = UART_STOP_BITS_1;
    config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
    config.source_clk = UART_SCLK_DEFAULT;

    ESP_ERROR_CHECK(uart_driver_install(BOARD_UART_BIO, 2048, 2048, 0, nullptr, 0));
    ESP_ERROR_CHECK(uart_param_config(BOARD_UART_BIO, &config));
    ESP_ERROR_CHECK(uart_set_pin(BOARD_UART_BIO, BOARD_PIN_BIO_TX, BOARD_PIN_BIO_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    s_uart_ready = true;
    ESP_LOGI(TAG, "UART bioimpedancia inicializada");

    return ESP_OK;
}

esp_err_t bio_serial_write(const uint8_t *data, size_t len)
{
    if (data == nullptr || len == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    if (!s_uart_ready) {
        return ESP_ERR_INVALID_STATE;
    }

    int written = uart_write_bytes(BOARD_UART_BIO, data, len);
    return written == (int)len ? ESP_OK : ESP_FAIL;
}

void bio_serial_task(void *arg)
{
    (void)arg;

    while (true) {
        if (!s_uart_ready) {
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }

        uint8_t buffer[128] = {};
        int len = uart_read_bytes(BOARD_UART_BIO, buffer, sizeof(buffer), pdMS_TO_TICKS(100));

        if (len > 0) {
            // Provisorio: espelha para stdout.
            // Em produto final, separar logs do protocolo USB.
            for (int i = 0; i < len; i++) {
                putchar(buffer[i]);
            }
            fflush(stdout);
        }
    }
}