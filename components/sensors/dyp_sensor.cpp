#include "dyp_sensor.h"
#include "esp_check.h"

#include <stdint.h>

#include "driver/uart.h"
#include "esp_log.h"

#include "app_state.h"
#include "board_io.h"
#include "board_pins.h"

static const char *TAG = "dyp_sensor";

static bool s_uart_ready = false;

esp_err_t dyp_sensor_init(void)
{
    if (!board_gpio_is_valid(BOARD_PIN_DYP_TX) || !board_gpio_is_valid(BOARD_PIN_DYP_RX)) {
        ESP_LOGW(TAG, "UART DYP nao configurada em board_pins.h");
        s_uart_ready = false;
        return ESP_OK;
    }

    uart_config_t config = {};
    config.baud_rate = BOARD_DYP_BAUDRATE;
    config.data_bits = UART_DATA_8_BITS;
    config.parity = UART_PARITY_DISABLE;
    config.stop_bits = UART_STOP_BITS_1;
    config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
    config.source_clk = UART_SCLK_DEFAULT;

    ESP_ERROR_CHECK(uart_driver_install(BOARD_UART_DYP, 1024, 1024, 0, nullptr, 0));
    ESP_ERROR_CHECK(uart_param_config(BOARD_UART_DYP, &config));
    ESP_ERROR_CHECK(uart_set_pin(BOARD_UART_DYP, BOARD_PIN_DYP_TX, BOARD_PIN_DYP_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    s_uart_ready = true;
    ESP_LOGI(TAG, "UART DYP inicializada");

    return ESP_OK;
}

esp_err_t dyp_sensor_measure_height_m(float *out_height_m)
{
    if (out_height_m == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    if (!s_uart_ready) {
        return ESP_ERR_INVALID_STATE;
    }

    const uint8_t command[] = {0x55, 0xAA, 0x01, 0x05, 0x05};
    uint8_t rx[7] = {};

    uart_flush_input(BOARD_UART_DYP);
    uart_write_bytes(BOARD_UART_DYP, command, sizeof(command));

    int len = uart_read_bytes(BOARD_UART_DYP, rx, sizeof(rx), pdMS_TO_TICKS(500));
    if (len != (int)sizeof(rx)) {
        ESP_LOGW(TAG, "Frame DYP incompleto: %d bytes", len);
        return ESP_ERR_TIMEOUT;
    }

    if (rx[0] != 0x55 || rx[1] != 0xAA) {
        ESP_LOGW(TAG, "Header DYP invalido: %02X %02X", rx[0], rx[1]);
        return ESP_FAIL;
    }

    uint8_t checksum = 0;
    for (int i = 0; i < 6; i++) {
        checksum += rx[i];
    }

    if (checksum != rx[6]) {
        ESP_LOGW(TAG, "Checksum DYP invalido. calc=%02X rx=%02X", checksum, rx[6]);
        return ESP_FAIL;
    }

    // TODO: confirmar posicao exata dos bytes de distancia conforme firmware original.
    const uint16_t distance_mm = ((uint16_t)rx[4] << 8) | rx[5];
    const float distance_m = (float)distance_mm / 1000.0f;

    app_state_t state = {};
    ESP_RETURN_ON_ERROR(app_state_get(&state), TAG, "Falha ao ler estado");

    float height_m = state.calibration.sensor_height_m - distance_m;
    if (height_m < 0.0f) {
        height_m = 0.0f;
    }

    *out_height_m = height_m;
    ESP_RETURN_ON_ERROR(app_state_set_height(height_m), TAG, "Falha ao atualizar altura");

    ESP_LOGI(TAG, "DYP: distance=%.3f m height=%.3f m", distance_m, height_m);
    return ESP_OK;
}