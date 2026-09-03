$ErrorActionPreference = "Stop"

function Ensure-Dir($Path) {
    if (!(Test-Path $Path)) {
        New-Item -ItemType Directory -Path $Path | Out-Null
    }
}

function Write-ProjectFile($Path, $Content) {
    $dir = Split-Path $Path -Parent
    if ($dir) {
        Ensure-Dir $dir
    }
    Set-Content -Path $Path -Value $Content -Encoding UTF8
    Write-Host "Atualizado: $Path"
}

Ensure-Dir ".\components\ui"
Ensure-Dir ".\components\ui\include"

Write-ProjectFile ".\components\ui\idf_component.yml" @'
dependencies:
  espressif/esp32_display_panel: "1.0.4"
  lvgl/lvgl: "8.4.0"
'@

Write-ProjectFile ".\components\ui\CMakeLists.txt" @'
idf_component_register(
    SRCS
        "ui_app.cpp"
        "ui_screens.cpp"
        "lvgl_v8_port.cpp"
    INCLUDE_DIRS
        "include"
    REQUIRES
        app_state
        freertos
        log
        esp_timer
        driver
        esp_lcd
        lvgl
        esp32_display_panel
)

target_compile_options(${COMPONENT_LIB} PRIVATE
    -Wno-missing-field-initializers
)
'@

Write-ProjectFile ".\components\ui\include\ui_app.h" @'
#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t ui_app_init(void);
void ui_app_task(void *arg);

#ifdef __cplusplus
}
#endif
'@

Write-ProjectFile ".\components\ui\include\ui_screens.h" @'
#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t ui_screens_create(void);
void ui_screens_set_status(const char *text);

#ifdef __cplusplus
}
#endif
'@

Write-ProjectFile ".\components\ui\ui_screens.cpp" @'
#include "ui_screens.h"

#include "lvgl.h"
#include "lvgl_v8_port.h"

static lv_obj_t *s_status = nullptr;

esp_err_t ui_screens_create(void)
{
    if (!lvgl_port_lock(-1)) {
        return ESP_FAIL;
    }

    lv_obj_t *scr = lv_scr_act();
    lv_obj_clean(scr);

    lv_obj_set_style_bg_color(scr, lv_color_hex(0x001428), 0);

    lv_obj_t *title = lv_label_create(scr);
    lv_label_set_text(title, "BioTriagem");
    lv_obj_set_style_text_color(title, lv_color_white(), 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_24, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 24);

    lv_obj_t *subtitle = lv_label_create(scr);
    lv_label_set_text(subtitle, "ESP-IDF + VIEWE + LVGL");
    lv_obj_set_style_text_color(subtitle, lv_color_hex(0x00FFFF), 0);
    lv_obj_align(subtitle, LV_ALIGN_TOP_MID, 0, 64);

    s_status = lv_label_create(scr);
    lv_label_set_text(s_status, "Display inicializado");
    lv_obj_set_style_text_color(s_status, lv_color_white(), 0);
    lv_obj_set_style_text_font(s_status, &lv_font_montserrat_18, 0);
    lv_obj_align(s_status, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *box = lv_obj_create(scr);
    lv_obj_set_size(box, 260, 90);
    lv_obj_align(box, LV_ALIGN_BOTTOM_MID, 0, -40);
    lv_obj_set_style_bg_color(box, lv_color_hex(0x003A5C), 0);
    lv_obj_set_style_radius(box, 12, 0);

    lv_obj_t *box_label = lv_label_create(box);
    lv_label_set_text(box_label, "UI minima OK");
    lv_obj_set_style_text_color(box_label, lv_color_white(), 0);
    lv_obj_center(box_label);

    lv_obj_invalidate(scr);
    lv_refr_now(nullptr);

    lvgl_port_unlock();
    return ESP_OK;
}

void ui_screens_set_status(const char *text)
{
    if (text == nullptr || s_status == nullptr) {
        return;
    }

    if (!lvgl_port_lock(100)) {
        return;
    }

    lv_label_set_text(s_status, text);
    lv_obj_invalidate(s_status);

    lvgl_port_unlock();
}
'@

Write-ProjectFile ".\components\ui\ui_app.cpp" @'
#include "ui_app.h"

#include <cstdint>

#include "esp_check.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_display_panel.hpp"
#include "lvgl_v8_port.h"
#include "ui_screens.h"

using namespace esp_panel::board;
using namespace esp_panel::drivers;

static const char *TAG = "ui_app";

static Board *s_panel = nullptr;

static void lcd_direct_color_test(LCD *lcd)
{
    if (lcd == nullptr) {
        ESP_LOGE(TAG, "LCD nulo no teste direto");
        return;
    }

    const int w = lcd->getFrameWidth();
    const int h = lcd->getFrameHeight();

    ESP_LOGI(TAG, "LCD direto: width=%d height=%d", w, h);

    static uint16_t line_buf[480 * 20];

    const uint16_t colors[] = {
        0xFFFF, // branco
        0xF800, // vermelho
        0x07E0, // verde
        0x001F, // azul
        0x0000  // preto
    };

    const int band_h = h / 5;

    for (int band = 0; band < 5; band++) {
        const uint16_t color = colors[band];

        for (int i = 0; i < w * 20; i++) {
            line_buf[i] = color;
        }

        const int y_start = band * band_h;
        const int y_end = (band == 4) ? h : y_start + band_h;

        for (int y = y_start; y < y_end; y += 20) {
            const int rows = ((y + 20) <= y_end) ? 20 : (y_end - y);

            lcd->drawBitmap(
                0,
                y,
                w,
                rows,
                reinterpret_cast<const uint8_t *>(line_buf)
            );
        }
    }

    ESP_LOGI(TAG, "Teste direto LCD enviado");
}

esp_err_t ui_app_init(void)
{
    ESP_LOGI(TAG, "Inicializando display Viewe");

    s_panel = new Board();
    if (s_panel == nullptr) {
        ESP_LOGE(TAG, "Falha ao alocar Board");
        return ESP_ERR_NO_MEM;
    }

    if (!s_panel->init()) {
        ESP_LOGE(TAG, "Falha em panel->init()");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "panel->init() OK");

    if (!s_panel->begin()) {
        ESP_LOGE(TAG, "Falha em panel->begin()");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "panel->begin() OK");

    auto backlight = s_panel->getBacklight();
    if (backlight != nullptr) {
        backlight->on();
        backlight->setBrightness(200);
        ESP_LOGI(TAG, "Backlight ligado");
    } else {
        ESP_LOGW(TAG, "Backlight nulo");
    }

    auto lcd = s_panel->getLCD();
    auto touch = s_panel->getTouch();

    if (lcd == nullptr) {
        ESP_LOGE(TAG, "LCD nulo após begin");
        return ESP_FAIL;
    }

    lcd_direct_color_test(lcd);

    vTaskDelay(pdMS_TO_TICKS(2000));

    ESP_LOGI(TAG, "Inicializando LVGL");

    if (!lvgl_port_init(lcd, touch)) {
        ESP_LOGE(TAG, "Falha ao iniciar LVGL port");
        return ESP_FAIL;
    }

    ESP_RETURN_ON_ERROR(ui_screens_create(), TAG, "Falha ao criar tela inicial");

    ESP_LOGI(TAG, "Display/LVGL inicializados");
    return ESP_OK;
}

void ui_app_task(void *arg)
{
    (void)arg;
    vTaskDelete(nullptr);
}
'@

$baseUrl = "https://raw.githubusercontent.com/esp-arduino-libs/ESP32_Display_Panel/master/examples/esp_idf/lvgl_v8_port/main"

Invoke-WebRequest "$baseUrl/lvgl_v8_port.cpp" -OutFile ".\components\ui\lvgl_v8_port.cpp"
Invoke-WebRequest "$baseUrl/lvgl_v8_port.h" -OutFile ".\components\ui\include\lvgl_v8_port.h"

Write-Host "Baixado port oficial LVGL v8."

$sdkPath = ".\sdkconfig.defaults"
if (!(Test-Path $sdkPath)) {
    New-Item -ItemType File -Path $sdkPath | Out-Null
}

$content = Get-Content $sdkPath -Raw

$removePatterns = @(
    "(?m)^CONFIG_ESP_PANEL_BOARD_DEFAULT_USE_.*\r?\n?",
    "(?m)^# CONFIG_ESP_PANEL_BOARD_DEFAULT_USE_.*\r?\n?",
    "(?m)^CONFIG_ESP_PANEL_BOARD_MANUFACTURER_.*\r?\n?",
    "(?m)^# CONFIG_ESP_PANEL_BOARD_MANUFACTURER_.*\r?\n?",
    "(?m)^CONFIG_BOARD_VIEWE_.*\r?\n?",
    "(?m)^# CONFIG_BOARD_VIEWE_.*\r?\n?",
    "(?m)^CONFIG_LV_COLOR_16_SWAP.*\r?\n?",
    "(?m)^CONFIG_LV_FONT_MONTSERRAT_18.*\r?\n?",
    "(?m)^CONFIG_LV_FONT_MONTSERRAT_24.*\r?\n?",
    "(?m)^CONFIG_LV_MEM_CUSTOM.*\r?\n?",
    "(?m)^CONFIG_LV_MEMCPY_MEMSET_STD.*\r?\n?",
    "(?m)^CONFIG_LV_USE_LOG.*\r?\n?",
    "(?m)^CONFIG_LV_LOG_PRINTF.*\r?\n?",
    "(?m)^CONFIG_ESPTOOLPY_FLASHSIZE_.*\r?\n?",
    "(?m)^CONFIG_SPIRAM.*\r?\n?"
)

foreach ($pattern in $removePatterns) {
    $content = [regex]::Replace($content, $pattern, "")
}

$content = $content.TrimEnd() + @'

# ==== Viewe UEDX32480035E-WB-A + LVGL v8 ====
CONFIG_FREERTOS_HZ=1000

CONFIG_ESPTOOLPY_FLASHSIZE_16MB=y

CONFIG_SPIRAM=y
CONFIG_SPIRAM_MODE_OCT=y
CONFIG_SPIRAM_SPEED_80M=y
CONFIG_SPIRAM_USE_MALLOC=y

CONFIG_ESP_PANEL_BOARD_DEFAULT_USE_SUPPORTED=y
CONFIG_ESP_PANEL_BOARD_MANUFACTURER_VIEWE=y
CONFIG_BOARD_VIEWE_UEDX32480035E_WB_A=y

CONFIG_LV_COLOR_16_SWAP=y
CONFIG_LV_MEM_CUSTOM=y
CONFIG_LV_MEMCPY_MEMSET_STD=y
CONFIG_LV_USE_LOG=y
CONFIG_LV_LOG_PRINTF=y
CONFIG_LV_FONT_MONTSERRAT_18=y
CONFIG_LV_FONT_MONTSERRAT_24=y
'@

Set-Content -Path $sdkPath -Value $content -Encoding UTF8
Write-Host "Atualizado: sdkconfig.defaults"

Write-Host ""
Write-Host "Patch GUI Viewe aplicado."
Write-Host "Agora rode:"
Write-Host "  Remove-Item .\sdkconfig -Force -ErrorAction SilentlyContinue"
Write-Host "  idf.py set-target esp32s3"
Write-Host "  idf.py fullclean"
Write-Host "  idf.py reconfigure"
Write-Host "  idf.py build flash monitor"