#include "board_io.h"
#include "board_pins.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "esp_log.h"

#include "app_state.h"

static const char *TAG = "board_io";

bool board_gpio_is_valid(gpio_num_t pin)
{
    return ((int)pin >= 0);
}

static void append_pin(uint64_t *mask, gpio_num_t pin)
{
    if (board_gpio_is_valid(pin)) {
        *mask |= (1ULL << pin);
    }
}

esp_err_t board_io_init(void)
{
    uint64_t input_mask = 0;

    append_pin(&input_mask, BOARD_PIN_DIP_ORIENTATION);
    append_pin(&input_mask, BOARD_PIN_DIP_MODE_HEIGHT);
    append_pin(&input_mask, BOARD_PIN_DIP_SCALE_DIV100);

    if (input_mask == 0) {
        ESP_LOGW(TAG, "Nenhum DIP switch configurado em board_pins.h");
        return ESP_OK;
    }

    gpio_config_t config = {};
    config.pin_bit_mask = input_mask;
    config.mode = GPIO_MODE_INPUT;
    config.pull_up_en = GPIO_PULLUP_ENABLE;
    config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    config.intr_type = GPIO_INTR_DISABLE;

    ESP_ERROR_CHECK(gpio_config(&config));

    ESP_LOGI(TAG, "GPIOs de DIP inicializados");
    return ESP_OK;
}

board_dip_state_t board_io_read_dips(void)
{
    board_dip_state_t dips = {};

    if (board_gpio_is_valid(BOARD_PIN_DIP_ORIENTATION)) {
        dips.orientation_landscape = gpio_get_level(BOARD_PIN_DIP_ORIENTATION) == 0;
    }

    if (board_gpio_is_valid(BOARD_PIN_DIP_MODE_HEIGHT)) {
        dips.mode_height = gpio_get_level(BOARD_PIN_DIP_MODE_HEIGHT) == 0;
    }

    if (board_gpio_is_valid(BOARD_PIN_DIP_SCALE_DIV100)) {
        dips.scale_div100 = gpio_get_level(BOARD_PIN_DIP_SCALE_DIV100) == 0;
    }

    return dips;
}

void board_io_task(void *arg)
{
    (void)arg;

    board_dip_state_t previous = {};
    bool initialized = false;

    while (true) {
        board_dip_state_t current = board_io_read_dips();

        if (!initialized ||
            current.orientation_landscape != previous.orientation_landscape ||
            current.mode_height != previous.mode_height ||
            current.scale_div100 != previous.scale_div100) {

            initialized = true;
            previous = current;

            app_state_set_orientation(
                current.orientation_landscape
                    ? APP_ORIENTATION_LANDSCAPE
                    : APP_ORIENTATION_PORTRAIT
            );

            app_state_set_measurement_mode(
                current.mode_height
                    ? MEASUREMENT_MODE_HEIGHT
                    : MEASUREMENT_MODE_BOTH
            );

            ESP_LOGI(
                TAG,
                "DIPs: orientation_landscape=%d mode_height=%d scale_div100=%d",
                current.orientation_landscape,
                current.mode_height,
                current.scale_div100
            );
        }

        vTaskDelay(pdMS_TO_TICKS(200));
    }
}