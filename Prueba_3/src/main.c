#include <stdio.h>
#include <string.h>
#include <driver/adc.h> //ADC
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h" //semaforo
#include "freertos/task.h"   //libreria Tareas
#include "driver/gpio.h"     //pines generales
#include "esp_log.h"
#include "sdkconfig.h"
#include "soc/soc.h"          //disable brownout detector
#include "soc/rtc_cntl_reg.h" //disable brownout detector (deteccion de apagon)
#include "soc/rtc_wdt.h"
#include "freertos/FreeRTOSConfig.h"

// librerias propias
#include "../include/I2C.h"
#include "../include/Uart.h"
#include "../include/LCDI2C.h"
#include "../include/pulsador.h"

// //=========================== Variables y Definiciones ================================
extern estadoLCD EstadoLCD;
float LecturaADC = 0;
gpio_int_type_t led = GPIO_NUM_2;

#define TiempoEntreMediciones 10
#define T_ESPERA_MS 40
#define T_ESPERA pdMS_TO_TICKS(T_ESPERA_MS)

// //=========================== Función principal ================================

void TareaProcesadorA(void *taskParmPtr); // Prototipo de la función de la tarea
void TareaProcesadorB(void *taskParmPtr); // Prototipo de la función de la tarea

void app_main()
{
    // WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
    // rtc_wdt_protect_off();
    // rtc_wdt_disable();

    I2C_init();            // Inicializo el i2c
    LCDI2C_init();         // Inicializo el LCD
    config_Uart();         // Configuro el uart para recibir datos
    inicializarPulsador(); // Inicializo pulsadores

    // Configuracion de ADC
    adc1_config_width(ADC_WIDTH_12Bit); // configuran la resoluncion
    adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_11db);

    // PRESENTACION LCD
    // lcd_gotoxy(1, 1); // Imprimo fila 1 en display
    // lcd_print("Hola!");
    // vTaskDelay(2000 / portTICK_PERIOD_MS);

    // lcd_gotoxy(1, 1);
    // lcd_print("Alejo y Gaston");
    // vTaskDelay(2000 / portTICK_PERIOD_MS);
    
    // lcdCommand(0x01);
    // vTaskDelay(100 / portTICK_PERIOD_MS);

    // lcd_gotoxy(1, 1);
    // lcd_print("Un Pais con");
    // lcd_gotoxy(1, 2);
    // lcd_print("buena Gente");
    // vTaskDelay(4000 / portTICK_PERIOD_MS);

    // lcdCommand(0x01);
    // vTaskDelay(100 / portTICK_PERIOD_MS);

    

    // lcd_gotoxy(1, 1);
    // lcd_print("Hacemos historia");
    // vTaskDelay(2000 / portTICK_PERIOD_MS);

    // lcdCommand(0x01);
    // vTaskDelay(100 / portTICK_PERIOD_MS);

    // lcd_gotoxy(1, 1);
    // lcd_print("Somos historia");
    // vTaskDelay(2000 / portTICK_PERIOD_MS);

    

    // Crear tarea en freeRTOS
    // Devuelve pdPASS si la tarea fue creada y agregada a la lista ready
    // En caso contrario devuelve pdFAIL.
    // Ambas tareas, GPS y ADC, se ejecutan en simultaneo pero en distintos procesadores

    BaseType_t res1 = xTaskCreatePinnedToCore(
        TareaProcesadorA,             // Funcion de la tarea a ejecutar
        "TareaProcesadorA",           // Nombre de la tarea como String amigable para el usuario
        configMINIMAL_STACK_SIZE * 4, // Cantidad de stack de la tarea
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

    BaseType_t res2 = xTaskCreatePinnedToCore(
        TareaProcesadorB,             // Funcion de la tarea a ejecutar
        "TareaProcesadorB",           // Nombre de la tarea como String amigable para el usuario
        configMINIMAL_STACK_SIZE * 4, // Cantidad de stack de la tarea
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

bool Auxiliar1 = true, Auxiliar2 = false;
uint8_t ESTADO = 0;

void TareaProcesadorA(void *taskParmPtr)
{
    while (true)
    {
        ESTADO = actualizarPulsador(0);
        printf("%i\n", ESTADO);

        if (ESTADO)
        {
            if (Auxiliar1) // Hago esto para que se ejecute por unica vez
            {
                Auxiliar1 = false; // Lo paso a false para que no se ejecute de nuevo lo que esta dentro del if del ADC
                Auxiliar2 = true;  // Lo paso a True para que cuando vuelva al GPS ejecute lo que esta dentro del IF

                lcdCommand(0x01);                     // Limpio la pantalla del LCD
                vTaskDelay(100 / portTICK_PERIOD_MS); // tiempo que necesita el LCD para limpiar la pantalla
                gpio_set_level(led, 1);               // Uso un ledra verificar que cambio de estado

                lcd_gotoxy(1, 1);
                lcd_print("LEYENDO ADC");
            }

            // // bucle del ADC

            LecturaADC = adc1_get_raw(ADC1_CHANNEL_0); // Leo el ADC

            // printf("El valor del ADC1 es %f V\n", LecturaADC);

            lcd_gotoxy(1, 2);

            Print_Float_LCD(LecturaADC); // Imprimo los datos del ADC
        }
    }
}

void TareaProcesadorB(void *taskParmPtr)
{
    while (true)
    {
        if (!ESTADO)
        {
            if (Auxiliar2) // Hago esto para que se ejecute por unica vez
            {
                Auxiliar1 = true;
                Auxiliar2 = false;
                lcdCommand(0x01);
                vTaskDelay(100 / portTICK_PERIOD_MS); // tiempo que necesita el LCD para limpiar la pantalla
                gpio_set_level(led, 1);               // Uso un led para verificar que cambio de estado

                lcd_gotoxy(1, 1);
                lcd_print("LEYENDO GPS");
            }
        }
        // bucle del GPS
    }
}