#ifndef UART_H_
#define UART_H_
//========================= Inclusiones =========================
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "../include/LCDI2C.h"
#include "../include/I2C.h"

//========================= Definiciones =========================

//static void getstring(char *str);

/*char arr1[] = "LAT";
char arr2[] = "LON";
char arr3[] = "           ";
char arr4[] = "            ";

unsigned char *msg_str = &arr1;
unsigned char *msg_str2 = &arr2;
unsigned char *msg_str3 = &arr3;
unsigned char *msg_str4 = &arr4;*/

//#define TXD_PIN (GPIO_NUM_1)
//#define RXD_PIN (GPIO_NUM_3)

#define TXD_PIN2 (GPIO_NUM_17)
#define RXD_PIN2 (GPIO_NUM_16)

//void init(void);
void Uart_init2(void);
void rx_uart(void *arg);
void getstring(char *str);
#endif