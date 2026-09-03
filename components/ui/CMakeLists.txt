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
