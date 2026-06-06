#include "usb_protocol.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "app_state.h"
#include "dyp_sensor.h"
#include "scale_sensor.h"
#include "bio_serial.h"
#include "neon_pwm.h"
#include "rgb_strip.h"

static const char *TAG = "usb_protocol";

static void protocol_write(const char *text)
{
    printf("%s", text);
    fflush(stdout);
}

static bool parse_rgb_command(const char *frame, uint8_t *r, uint8_t *g, uint8_t *b)
{
    if (frame == nullptr || r == nullptr || g == nullptr || b == nullptr) {
        return false;
    }

    if (strlen(frame) != 12 || strncmp(frame, "RGB", 3) != 0) {
        return false;
    }

    for (int i = 3; i < 12; i++) {
        if (!isdigit((unsigned char)frame[i])) {
            return false;
        }
    }

    char part[4] = {};
    memcpy(part, frame + 3, 3);
    int ri = atoi(part);

    memcpy(part, frame + 6, 3);
    int gi = atoi(part);

    memcpy(part, frame + 9, 3);
    int bi = atoi(part);

    if (ri > 255 || gi > 255 || bi > 255) {
        return false;
    }

    *r = (uint8_t)ri;
    *g = (uint8_t)gi;
    *b = (uint8_t)bi;

    return true;
}

esp_err_t usb_protocol_init(void)
{
    setvbuf(stdin, nullptr, _IONBF, 0);
    setvbuf(stdout, nullptr, _IONBF, 0);

    ESP_LOGI(TAG, "Protocolo USB inicializado");
    return ESP_OK;
}

esp_err_t usb_protocol_handle_frame(const char *frame)
{
    if (frame == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_LOGI(TAG, "Frame recebido: %s", frame);

    if (strcmp(frame, "ALT") == 0) {
        float height_m = 0.0f;

        if (dyp_sensor_measure_height_m(&height_m) == ESP_OK) {
            char response[32] = {};
            snprintf(response, sizeof(response), "FA%04d$", (int)(height_m * 1000.0f));
            protocol_write(response);
        } else {
            protocol_write("FEALT$");
        }

        return ESP_OK;
    }

    if (strcmp(frame, "PES") == 0) {
        float weight_kg = 0.0f;

        if (scale_sensor_get_weight_kg(&weight_kg) == ESP_OK) {
            char response[32] = {};
            snprintf(response, sizeof(response), "PES%06.3f$", weight_kg);
            protocol_write(response);
        }

        return ESP_OK;
    }

    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;

    if (parse_rgb_command(frame, &r, &g, &b)) {
        app_state_set_rgb(r, g, b);
        rgb_strip_set_color(r, g, b);
        return ESP_OK;
    }

    if (strcmp(frame, "RG1") == 0) {
        app_state_set_rgb_enabled(true);
        rgb_strip_set_enabled(true);
        return ESP_OK;
    }

    if (strcmp(frame, "RG0") == 0) {
        app_state_set_rgb_enabled(false);
        rgb_strip_set_enabled(false);
        return ESP_OK;
    }

    if (strcmp(frame, "NE1") == 0) {
        app_state_set_neon_enabled(true);
        neon_pwm_set_enabled(true);
        return ESP_OK;
    }

    if (strcmp(frame, "NE0") == 0) {
        app_state_set_neon_enabled(false);
        neon_pwm_set_enabled(false);
        return ESP_OK;
    }

    if (frame[0] == 'F') {
        return bio_serial_write((const uint8_t *)frame, strlen(frame));
    }

    ESP_LOGW(TAG, "Comando desconhecido: %s", frame);
    return ESP_ERR_NOT_SUPPORTED;
}

void usb_protocol_task(void *arg)
{
    (void)arg;

    usb_protocol_init();

    char frame[128] = {};
    size_t frame_len = 0;
    bool in_frame = false;

    while (true) {
        int c = getchar();

        if (c < 0) {
            vTaskDelay(pdMS_TO_TICKS(10));
            continue;
        }

        if (c == '#') {
            in_frame = true;
            frame_len = 0;
            memset(frame, 0, sizeof(frame));
            continue;
        }

        if (c == '$' && in_frame) {
            frame[frame_len] = '\0';
            usb_protocol_handle_frame(frame);

            in_frame = false;
            frame_len = 0;
            memset(frame, 0, sizeof(frame));
            continue;
        }

        if (in_frame) {
            if (frame_len < sizeof(frame) - 1) {
                frame[frame_len++] = (char)c;
            } else {
                ESP_LOGW(TAG, "Frame USB excedeu tamanho maximo");
                in_frame = false;
                frame_len = 0;
                memset(frame, 0, sizeof(frame));
            }
        }
    }
}