#ifndef PULSADOR_H
#define PULSADOR_H

#include "driver/gpio.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "freertos/semphr.h"
#include "../include/LCDI2C.h"
#include "../include/I2C.h"
#include "../include/Uart.h"
#include "../include/I2C.h"
#include "../include/LCDI2C.h"
#include "esp_log.h"
#include "../include/Wifi.h"

typedef enum{
   ALTO,
   BAJO,
   DESCENDENTE,
   ASCENDENTE
} estadoBoton_t;

void errorBoton( void );
void inicializarBoton();
void tareaPulsador( void* taskParmPtr );
void actualizarBoton( gpio_int_type_t pulsador );
void botonPresionado( void );
void botonLiberado( void );
void actualizardisplay(int selector);
esp_err_t init_isr(void);
void isr_handler(void *arg1);

#endif