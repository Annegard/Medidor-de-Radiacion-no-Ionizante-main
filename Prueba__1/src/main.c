/**
 * @file main.c
 * @author Alejo Casas, Gaston Pautasso
 * @brief 
 * @version 4.1
 * @date 2022-09-03
 * 
 * @copyright Copyright (c) 2022
 * 
 * En esta version se añade la comunicacion UART y el GPS
 * 
 * Tando el GPS/UART como el ADC se ejecutan en el procesador A mientras que la lectura de los pulsadores se
 * ejecuta en el procesador B
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <driver/adc.h> //ADC
#include <stdint.h>
#include "freertos/FreeRTOS.h"
// #include "freertos/semphr.h" //semaforo
#include "freertos/task.h"   //libreria Tareas
#include "driver/gpio.h"     //pines generales
#include "esp_log.h"
#include "sdkconfig.h"
// #include "soc/soc.h"          //disable brownout detector
// #include "soc/rtc_cntl_reg.h" //disable brownout detector (deteccion de apagon)
// #include "soc/rtc_wdt.h"
#include "freertos/FreeRTOSConfig.h"

// librerias propias
#include "../include/I2C.h"
#include "../include/Uart.h"
#include "../include/LCDI2C.h"
#include "../include/pulsador.h"
//#include "../include/NMEA.h"

// //=========================== Variables y Definiciones ================================
// DEFINICIONES
#define TiempoEntreMediciones 10
#define T_ESPERA_MS 40
#define T_ESPERA pdMS_TO_TICKS(T_ESPERA_MS)
#define TiempoFuera 200

// Variables Auxiliares
bool Auxiliar1 = true, Auxiliar2 = false;
uint8_t ESTADO = 0;

// ADC
float LecturaADC = 0;

// Pines
gpio_int_type_t led = GPIO_NUM_2;

// GPS
char GGA[100];
char RMC[100];
//GPSSTRUCT gpsData;
//int flagGGA = 0, flagRMC = 0;
char lcdBuffer[50];
int GPS_Timeout = TiempoFuera; // GGA or RMC will not be received if the VCC is not sufficient

// //=========================== Función principal ================================

void TareaProcesadorA(void *taskParmPtr); // Prototipo de la función de la tarea
void TareaProcesadorB(void *taskParmPtr); // Prototipo de la función de la tarea
void GPS_LCD();
void PRESENTACION_LCD();

void app_main()
{
    I2C_init();            // Inicializo el i2c
    LCDI2C_init();         // Inicializo el LCD
    inicializarPulsador(); // Inicializo pulsadores
    // Configuracion de ADC
    adc1_config_width(ADC_WIDTH_12Bit); // configuran la resoluncion
    adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_11db);
    init_uart_0();
    Uart_init2();

    // PRESENTACION LCD

    // Crear tarea en freeRTOS
    // Devuelve pdPASS si la tarea fue creada y agregada a la lista ready
    // En caso contrario devuelve pdFAIL.
   
    BaseType_t res1 = xTaskCreatePinnedToCore(
        TareaProcesadorA,             // 
        "TareaProcesadorA",           // Nombre de la tarea como String amigable para el usuario
        configMINIMAL_STACK_SIZE * 10, // Cantidad de stack de la tarea
        NULL,                         // Parametros de tarea
        tskIDLE_PRIORITY,             // Prioridad de la tarea -> Queremos que este un nivel encima de IDLE
        NULL,                         // Puntero a la tarea creada en el sistema
        PROCESADORA);
    // Gestion de errores
    if (res1 == pdPASS)
    {
        printf("tarea ADC creada.\r\n");
    }
    if (res1 == pdFAIL)
    {
        printf("Error al crear la tarea ADC.\r\n");
        while (true)
            ; // si no pudo crear la tarea queda en un bucle infinito
    }
    
    vTaskDelay(500 / portTICK_PERIOD_MS);//Tiempo de espera para crear tarea.

    BaseType_t res2 = xTaskCreatePinnedToCore(
        TareaProcesadorB,             // Funcion de la tarea a ejecutar
        "TareaProcesadorB",           // Nombre de la tarea como String amigable para el usuario
        configMINIMAL_STACK_SIZE * 2, // Cantidad de stack de la tarea
        NULL,                         // Parametros de tarea
        tskIDLE_PRIORITY,             // Prioridad de la tarea -> Queremos que este un nivel encima de IDLE
        NULL,                         // Puntero a la tarea creada en el sistema
        PROCESADORB);

    // Gestion de errores
    if (res2 == pdPASS)
    {
        printf("tarea GPS creada.\r\n");
    }
    if (res2 == pdFAIL)
    {
        printf("Error al crear la tarea GPS.\r\n");
        while (true)
            ; // si no pudo crear la tarea queda en un bucle infinito
    }
}

/*  FUNCION void PRESENTACION_LCD()
    la funcion ejecuta una serie de mensajes en el LCD, no recibe ningun parametro*/
void PRESENTACION_LCD()
{
    lcd_gotoxy(1, 1);
    lcd_print("Alejo y Gaston");
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    lcdCommand(0x01);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    lcd_gotoxy(1, 1);
    lcd_print("Un Pais con");
    lcd_gotoxy(1, 2);
    lcd_print("buena Gente");
    vTaskDelay(4000 / portTICK_PERIOD_MS);

    lcdCommand(0x01);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    lcd_gotoxy(1, 1);
    lcd_print("Hacemos historia");
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    lcdCommand(0x01);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    lcd_gotoxy(1, 1);
    lcd_print("Somos historia");
    vTaskDelay(2000 / portTICK_PERIOD_MS);
}

/*  Funcion GPS_LCD()
    La funcion se ejecuta como un bloque de codigo mas del procesador que la ejecute.

    Esta llama al puerto uart designado en la libreria UART donde, de existir datos en el bufer del puerto,
    busca las palabras "GGA" y "RMC" en la trama recibida guardando las posiciones de memoria en los arreglos
    GGA y RMC.

    Luego "decodifica" la informacion que se halla en los arreglos guardandola en la estructura gpsData para
    acceder a dicha informacion mas tarde. La estructura y las funciones que decodifican se hallan en la
    libreria NMEA.

    La informacion almacenada en la estructura gpsData es en realidad dos estructuras almacenadas GGA y RMC,
        Esto se debe a que de la trama de datos GGA se obtienen:
            @locación (Lat, Norte/Sur, Long, Este/Oeste);
            @Tiempo (Horas, Minutos, Segundos)
            @Altitud

        Mientras que la trama de datos RMC contiene:
            @Velidad
            @Fecha (Día, Mes, Año)
            @Curso (Esta te juro que no tengo idea para que sirve)

    En caso de no recibir datos en el puerto UART el microcontrolador ira reduciendo un contador, si este llega a cero
    mostrara un mensaje solicitando revisar el cableado del GPS, reiniciando de nuevo el contador para seguir intentando
    conectarse

    En caso de que el GPS este bien conectado pero no reciba datos de los satelites se establecen "banderas" que dependiendo
    del valores mostrara en el LCD la informacion recibida del GPS o que el GPS se esta conectando.
*/
void GPS_LCD()
{
    /*if (rx_uart_Available("GGA", GGA) == 1)//Si se logra conectar con el GPS
    {
    lcd_gotoxy(1, 1);
    lcd_print(" no se encontro caracter");
       
    }*/
}

void TareaProcesadorA(void *taskParmPtr)
{
    while (true)
    {
        // // bucle del ADC

        LecturaADC = adc1_get_raw(ADC1_CHANNEL_6); // Leo el ADC
        
        if (ESTADO)
        {
            if (Auxiliar1) // Hago esto para que se ejecute por unica vez
            {
                Auxiliar1 = false; // Lo paso a false para que no se ejecute de nuevo lo que esta dentro del if del ADC
                Auxiliar2 = true;  // Lo paso a True para que cuando vuelva al GPS ejecute lo que esta dentro del IF

                lcdCommand(0x01);                     // Limpio la pantalla del LCD
                vTaskDelay(10 / portTICK_PERIOD_MS); // tiempo que necesita el LCD para limpiar la pantalla
                gpio_set_level(led, 1);               // Uso un ledra verificar que cambio de estado

                lcd_gotoxy(1, 1);
                lcd_print("LEYENDO ADC");
            }

            lcd_gotoxy(1, 2);
            Print_Float_LCD(LecturaADC); // Imprimo los datos del ADC
        }
        else
        {
            if (Auxiliar2) // Hago esto para que se ejecute por unica vez
            {
                Auxiliar1 = true;
                Auxiliar2 = false;
                lcdCommand(0x01);
                vTaskDelay(10 / portTICK_PERIOD_MS); // tiempo que necesita el LCD para limpiar la pantalla
                gpio_set_level(led, 1);               // Uso un led para verificar que cambio de estado

                lcd_gotoxy(1, 1);
                lcd_print("LEYENDO GPS");
            }
            rx_uart();
            //GPS_LCD(); // Me comunico con el gps y muestro los datos en el LCD
        }
    }
}

void TareaProcesadorB(void *taskParmPtr)
{
    while (true)
    {
        ESTADO = actualizarPulsador(0);
        
    }
}