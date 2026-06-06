#pragma once

#include <stdbool.h>

#include "esp_err.h"
#include "driver/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    bool orientation_landscape;
    bool mode_height;
    bool scale_div100;
} board_dip_state_t;

bool board_gpio_is_valid(gpio_num_t pin);

esp_err_t board_io_init(void);
board_dip_state_t board_io_read_dips(void);
void board_io_task(void *arg);

#ifdef __cplusplus
}
#endif