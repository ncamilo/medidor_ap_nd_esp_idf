#include "neon_pwm.h"

#include "driver/ledc.h"
#include "esp_log.h"

#include "board_io.h"
#include "board_pins.h"

static const char *TAG = "neon_pwm";

static bool s_ready = false;

static constexpr ledc_mode_t LEDC_MODE = LEDC_LOW_SPEED_MODE;
static constexpr ledc_timer_t LEDC_TIMER = LEDC_TIMER_0;
static constexpr ledc_channel_t LEDC_CHANNEL = LEDC_CHANNEL_0;
static constexpr ledc_timer_bit_t LEDC_RESOLUTION = LEDC_TIMER_10_BIT;
static constexpr uint32_t LEDC_MAX_DUTY = 1023;

esp_err_t neon_pwm_init(void)
{
    if (!board_gpio_is_valid(BOARD_PIN_NEON_PWM)) {
        ESP_LOGW(TAG, "Pino do neon nao configurado em board_pins.h");
        s_ready = false;
        return ESP_OK;
    }

    ledc_timer_config_t timer_config = {};
    timer_config.speed_mode = LEDC_MODE;
    timer_config.timer_num = LEDC_TIMER;
    timer_config.duty_resolution = LEDC_RESOLUTION;
    timer_config.freq_hz = 5000;
    timer_config.clk_cfg = LEDC_AUTO_CLK;

    ESP_ERROR_CHECK(ledc_timer_config(&timer_config));

    ledc_channel_config_t channel_config = {};
    channel_config.gpio_num = BOARD_PIN_NEON_PWM;
    channel_config.speed_mode = LEDC_MODE;
    channel_config.channel = LEDC_CHANNEL;
    channel_config.intr_type = LEDC_INTR_DISABLE;
    channel_config.timer_sel = LEDC_TIMER;
    channel_config.duty = 0;
    channel_config.hpoint = 0;

    ESP_ERROR_CHECK(ledc_channel_config(&channel_config));

    s_ready = true;
    ESP_LOGI(TAG, "PWM neon inicializado");

    return ESP_OK;
}

esp_err_t neon_pwm_set_duty_percent(uint8_t duty_percent)
{
    if (!s_ready) {
        return ESP_OK;
    }

    if (duty_percent > 100) {
        duty_percent = 100;
    }

    uint32_t duty = (LEDC_MAX_DUTY * duty_percent) / 100;

    ESP_RETURN_ON_ERROR(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty), TAG, "Falha ao definir duty");
    ESP_RETURN_ON_ERROR(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL), TAG, "Falha ao atualizar duty");

    return ESP_OK;
}

esp_err_t neon_pwm_set_enabled(bool enabled)
{
    return neon_pwm_set_duty_percent(enabled ? 100 : 0);
}