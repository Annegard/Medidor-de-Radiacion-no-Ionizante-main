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

//=========================== Definiciones ================================
typedef enum{
   ALTO,
   BAJO,
   DESCENDENTE,
   ASCENDENTE
} estadoBoton_t;

typedef enum{
   MENU_PRINCIPAL,
   CONECTAR_GPS,
   MEDIR_POTENCIA,
   CONECTAR_WIFI,
   GUARDAR_DATOS
   
} estadoMODO_t;

/*==================[Prototipos de funciones]======================*/
void errorBoton( void );
void inicializarBoton(gpio_int_type_t pulsador);
void actualizarBoton( gpio_int_type_t pulsador );
void botonPresionado( void );
void botonLiberado( void );

//esp_err_t init_isr(void);
//void isr_handler(void *arg1);

#endif