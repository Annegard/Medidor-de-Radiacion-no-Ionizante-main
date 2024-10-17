#ifndef PULSADOR_H
#define PULSADOR_H

/*==========================================================================================
   Descripcion:
   
==========================================================================================*/

/*==================[ Inclusiones ]============================================*/
//#include <stdio.h>
//#include <stdint.h>
//#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
//#include "freertos/queue.h"
//#include "esp_system.h"
#include "freertos/semphr.h"
//#include "esp_log.h"

//#include "../include/LCDI2C.h"
//#include "../include/I2C.h"

#include <driver/gpio.h>
#include "esp_timer.h"

//=========================== Definiciones ================================
typedef enum{
   ALTO,
   BAJO,
   DESCENDENTE,
   ASCENDENTE
} estadoBoton_t;

typedef enum{
   LATITUD,
   LONGITUD,
   POTENCIA,
   VOLTAJE,
   SDCARD,
   WIFI,
   DIA,
   HORA,
   CONTADOR_1,
   CONTADOR_2
} displayMODO_t;

typedef enum{
   GPS,
   POT,
   DATA,
   FECHA
} estadoMODO_t;

/*==================[Prototipos de funciones]======================*/

void actualizarBoton( gpio_int_type_t pulsador );
void inicializarBoton(gpio_int_type_t pulsador);

//esp_err_t init_isr(void);
//void isr_handler(void *arg1);

extern bool MODO_WIFI;

#endif