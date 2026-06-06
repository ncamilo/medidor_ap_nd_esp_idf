#include "rgb_strip.h"

#include "esp_log.h"

#include "board_io.h"
#include "board_pins.h"

static const char *TAG = "rgb_strip";

static bool s_ready = false;
static bool s_enabled = false;
static uint8_t s_r = 0;
static uint8_t s_g = 0;
static uint8_t s_b = 0;

esp_err_t rgb_strip_init(void)
{
    if (!board_gpio_is_valid(BOARD_PIN_RGB_DATA)) {
        ESP_LOGW(TAG, "Pino da fita RGB nao configurado em board_pins.h");
        s_ready = false;
        return ESP_OK;
    }

    // TODO: substituir este stub pelo driver RMT/led_strip compativel com WS2815.
    s_ready = true;

    ESP_LOGI(TAG, "Stub da fita RGB inicializado. LEDs=%d", BOARD_LED_STRIP_COUNT);
    return ESP_OK;
}

esp_err_t rgb_strip_set_color(uint8_t r, uint8_t g, uint8_t b)
{
    s_r = r;
    s_g = g;
    s_b = b;

    if (!s_ready) {
        ESP_LOGI(TAG, "RGB armazenado sem driver ativo: r=%u g=%u b=%u", r, g, b);
        return ESP_OK;
    }

    ESP_LOGI(TAG, "TODO aplicar RGB na fita: enabled=%d r=%u g=%u b=%u", s_enabled, r, g, b);
    return ESP_OK;
}

esp_err_t rgb_strip_set_enabled(bool enabled)
{
    s_enabled = enabled;

    if (!s_ready) {
        ESP_LOGI(TAG, "Estado RGB armazenado sem driver ativo: enabled=%d", enabled);
        return ESP_OK;
    }

    ESP_LOGI(TAG, "TODO %s fita RGB com cor r=%u g=%u b=%u", enabled ? "ligar" : "desligar", s_r, s_g, s_b);
    return ESP_OK;
}