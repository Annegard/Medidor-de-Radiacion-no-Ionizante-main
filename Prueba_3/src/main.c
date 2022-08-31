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
#include "pulsador.h"

// //=========================== Variables y Definiciones ================================
float LecturaADC = 0;

#define TiempoEntreMediciones 10
#define T_ESPERA_MS 40
#define T_ESPERA pdMS_TO_TICKS(T_ESPERA_MS)

// //=========================== Función principal ================================

gpio_int_type_t led[1] = {GPIO_NUM_2};
extern pulsadorInfo pulsador[N_PULSADOR];
SemaphoreHandle_t xSemaforo_Serial;

void TareaADC(void *taskParmPtr); // Prototipo de la función de la tarea
void TareaGPS(void *taskParmPtr); // Prototipo de la función de la tarea

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
    lcd_gotoxy(1, 1); // Imprimo fila 1 en display
    lcd_print("Hola!");
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    // lcd_gotoxy(1, 1);
    // lcd_print("Alejo y Gaston");
    // vTaskDelay(1000 / portTICK_PERIOD_MS);
    // lcd_gotoxy(1, 2);
    // lcd_print("Hacemos historia");
    // vTaskDelay(1000 / portTICK_PERIOD_MS);
    // lcdCommand(0x01);
    // vTaskDelay(100 / portTICK_PERIOD_MS);
    // lcd_gotoxy(1, 1);
    // lcd_print("Hacemos historia");

    // Crear tarea en freeRTOS
    // Devuelve pdPASS si la tarea fue creada y agregada a la lista ready
    // En caso contrario devuelve pdFAIL.

    BaseType_t res1 = xTaskCreatePinnedToCore(
        TareaADC,                     // Funcion de la tarea a ejecutar
        "TareaADC",                   // Nombre de la tarea como String amigable para el usuario
        configMINIMAL_STACK_SIZE * 4, // Cantidad de stack de la tarea
        NULL,                         // Parametros de tarea
        tskIDLE_PRIORITY,         // Prioridad de la tarea -> Queremos que este un nivel encima de IDLE
        NULL,                         // Puntero a la tarea creada en el sistema
        PROCESADORB);

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
        TareaGPS,                     // Funcion de la tarea a ejecutar
        "TareaGPS",                   // Nombre de la tarea como String amigable para el usuario
        configMINIMAL_STACK_SIZE * 4, // Cantidad de stack de la tarea
        NULL,                         // Parametros de tarea
        tskIDLE_PRIORITY,         // Prioridad de la tarea -> Queremos que este un nivel encima de IDLE
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

void TareaADC(void *taskParmPtr)
{
    while (true)
    {
        printf("Tarea ADC\r\n");
    }
}

void TareaGPS(void *taskParmPtr)
{
    while (true)
    {
        printf("Tarea GPS\r\n");
    }
}