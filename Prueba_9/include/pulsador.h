#ifndef PULSADOR_H
#define PULSADOR_H

/*==========================================================================================
   Descripcion:
   
==========================================================================================*/

/*==================[ Inclusiones ]============================================*/
#include "../include/Global.h"

/*==================[Prototipos de funciones]======================*/
void actualizarBoton(gpio_int_type_t pulsador );
void inicializarBoton(gpio_int_type_t pulsador);

#endif