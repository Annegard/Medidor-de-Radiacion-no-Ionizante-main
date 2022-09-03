#ifndef UART_H_
#define UART_H_
//========================= Inclusiones =========================
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "../include/LCDI2C.h"
#include "../include/I2C.h"

// Fuentes:
// https://www.sigmaelectronica.net/trama-gps/
// https://github.com/Tinyu-Zhao/TinyGPSPlus-ESP32/tree/master/src
// https://github.com/Tinyu-Zhao/TinyGPSPlus-ESP32/tree/master/src

//========================= Definiciones =========================

#define TXD_PIN2 (GPIO_NUM_17)
#define RXD_PIN2 (GPIO_NUM_16)

// void init(void);
void Uart_init2(void);
bool rx_uart_Available(char *str, char *buffertocopyinto);
void getstring(char *str);
#endif