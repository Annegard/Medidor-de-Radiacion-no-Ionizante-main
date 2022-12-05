#ifndef PULSADOR_H_
#define PULSADOR_H_
/*==================[ Inclusiones ]============================================*/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "freertos/semphr.h" //incluyo la libreria del semaforo
/*==================[ Definiciones ]===================================*/
#define TIEMPO_NO_VALIDO    0
#define N_PULSADOR          2   //Cantidad de pulsadores
#define PROCESADORA         0
#define PROCESADORB         1

typedef enum
{
    ADC,
    GPS
} estadoLCD;

typedef enum
{
    ALTO,
    BAJO,
    DESCENDENTE,
    ASCENDENTE
} estadoPulsador;

typedef struct
{
    gpio_int_type_t tecla;
    estadoPulsador estado;   //variables
} pulsadorInfo;

/*==================[Prototipos de funciones]======================*/
void inicializarPulsador(  void );
bool actualizarPulsador( uint8_t  indice);//Cada vez que se la llama consulta el estado del pulsador que recibe como parametro

#endif