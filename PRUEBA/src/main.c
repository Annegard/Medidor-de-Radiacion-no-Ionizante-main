#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "soc/soc.h"          //disable brownout detector
#include "soc/rtc_cntl_reg.h" //disable brownout detector (deteccion de apagon)
#include "soc/rtc_wdt.h"
#include "../include/I2C.h"
#include "../include/Uart.h"
#include "../include/LCDI2C.h"
#include <driver/adc.h>

//=========================== Variables y Definiciones ================================
float lectura = 0;
#define TiempoEntreMediciones 10

//=========================== Función principal ================================
void app_main()
{

    I2C_init();    // Inicializo el i2c
    LCDI2C_init(); // Inicializo el LCD
    vTaskDelay(100 / portTICK_PERIOD_MS);

    adc1_config_width(ADC_WIDTH_12Bit); // configuran la resoluncion
    adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_11db);

    // init();
    Uart_init2(); // Inicializo el UART

    xTaskCreate(rx_uart, "Rx_tarea_Uart", 1024 * 2, NULL, configMAX_PRIORITIES, NULL);
    /*lcd_gotoxy(1, 1);
    lcd_print("LAT"); // Imprimo fila 1 en display
    lcd_gotoxy(1, 2);
    lcd_print("LONG"); // Imprimo fila 1 en display*/
    while (1)
    {

        vTaskDelay(100 / portTICK_PERIOD_MS);

        /*lectura = adc1_get_raw(ADC1_CHANNEL_6);

        lectura = lectura ;

        printf("El valor del ADC1 es %f V\n", lectura);

        lcd_gotoxy(1, 2);

        Print_Float_LCD(lectura);*/
    }
}
