#pragma once

#include "driver/gpio.h"
#include "driver/uart.h"

#define BOARD_GPIO_NC ((gpio_num_t)-1)

// =========================
// UARTs
// =========================

#define BOARD_UART_DYP   UART_NUM_1
#define BOARD_UART_SCALE UART_NUM_2
#define BOARD_UART_BIO   UART_NUM_0

#define BOARD_DYP_BAUDRATE   9600
#define BOARD_SCALE_BAUDRATE 9600
#define BOARD_BIO_BAUDRATE   4800

// =========================
// Sensor DYP - altura
// ESP32 TX -> DYP RX
// ESP32 RX <- DYP TX
// =========================

#define BOARD_PIN_DYP_TX GPIO_NUM_8
#define BOARD_PIN_DYP_RX GPIO_NUM_9

// =========================
// Balança serial
// Se a balança só transmite, TX pode ficar BOARD_GPIO_NC,
// mas o código atual exige RX válido.
// =========================

#define BOARD_PIN_SCALE_TX BOARD_GPIO_NC
#define BOARD_PIN_SCALE_RX GPIO_NUM_38

// =========================
// Bioimpedância
// ESP32 TX -> Bio RX
// ESP32 RX <- Bio TX
// =========================

#define BOARD_PIN_BIO_TX GPIO_NUM_43
#define BOARD_PIN_BIO_RX GPIO_NUM_44

// =========================
// DIP switches
// Considerando chave para GND com pull-up interno.
// Ativo em nível baixo.
// =========================

#define BOARD_PIN_DIP_ORIENTATION   GPIO_NUM_11
#define BOARD_PIN_DIP_MODE_WEIGHT   GPIO_NUM_12   
#define BOARD_PIN_DIP_MODE_HEIGHT   GPIO_NUM_13
#define BOARD_PIN_DIP_SCALE_DIV100  GPIO_NUM_21

// =========================
// Saídas
// =========================

#define BOARD_PIN_NEON_PWM GPIO_NUM_15
#define BOARD_PIN_RGB_DATA GPIO_NUM_18

#define BOARD_LED_STRIP_COUNT 300