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
#include "freertos/semphr.h" //semaforo
#include "freertos/queue.h"
#include "freertos/task.h" //libreria Tareas
#include "driver/gpio.h"   //pines generales
#include "esp_log.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOSConfig.h"
#include "driver/uart.h"

// librerias propias
#include "../include/I2C.h"
#include "../include/LCDI2C.h"

// //=========================== Definiciones ================================//
#define ENTRADA_0 12
#define MASCARA_PINES_ENTRADA (1ULL << ENTRADA_0)
#define FLAG_INTERRUPCION 0

#define TXD_PIN2 (GPIO_NUM_0)
#define RXD_PIN2 (GPIO_NUM_15)

// //=========================== Variables ================================// //

bool Auxiliar1 = true, Auxiliar2 = false, EstadoLED = true, EstadoLCD = true; // Variables Auxiliares
float LecturaADC = 0;                                                         // ADC
gpio_int_type_t blink = GPIO_NUM_2;                                           // Blink
int64_t Tiempo1 = 0, Tiempo2 = 0;
uint32_t Retardo = 1000000;

bool lectura_gps = false;
static const int RX_BUF_SIZE = 2024;
bool Aux1 = false, Aux2 = true;

// //=========================== Prototipos de funciones ================================// //
void Loop();
void BLINK();
void PulsadorConfigToInterrupt();
static void IRAM_ATTR gpioIsrHandler(void *arg);

void Uart_init2(void);
void rx_uart();
void getstring(char *str);

// //=========================== Función principal ================================ // //
void app_main()
{
    I2C_init();                  // Inicializo el i2c
    LCDI2C_init();               // Inicializo el LCD
    PulsadorConfigToInterrupt(); // Configuro la entrada para interrupciones

    // Inicializo UARTs
    Uart_init2();

    // Configuracion de ADC
    adc1_config_width(ADC_WIDTH_12Bit); // configuran la resolución
    adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_11db);

    lcd_print("hola");
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    // Ingresa el bucle
    Loop();
}

void Loop()
{

    while (true)
    {

        if ((esp_timer_get_time() - Tiempo1) >= Retardo)
        {
            Tiempo1 = esp_timer_get_time();

            LecturaADC = adc1_get_raw(ADC1_CHANNEL_0); // Leo el ADC
        }

        if (EstadoLCD)
        {
            if (Auxiliar1) // Hago esto para que se ejecute por unica vez
            {
                Auxiliar1 = false;                   // Lo paso a false para que no se ejecute de nuevo lo que esta dentro del if del ADC
                Auxiliar2 = true;                    // Lo paso a True para que cuando vuelva al GPS ejecute lo que esta dentro del IF
                lcdCommand(0x01);                    // Limpio la pantalla del LCD
                vTaskDelay(20 / portTICK_PERIOD_MS); // tiempo que necesita el LCD para limpiar la pantalla
                                                     // Uso un ledra verificar que cambio de estado

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
                lcdCommand(0x01);                    // Limpio la pantalla del LCD
                vTaskDelay(20 / portTICK_PERIOD_MS); // tiempo que necesita el LCD para limpiar la pantalla
                                                     // Uso un ledra verificar que cambio de estado

                lcd_gotoxy(1, 1);
                lcd_print("LEYENDO GPS");
            }
            rx_uart();
        }
    }
}

void BLINK()
{
    if (EstadoLED)
    {
        gpio_set_level(blink, 1);
        EstadoLED = false;
    }
    else
    {
        gpio_set_level(blink, 0);
        EstadoLED = true;
    }
}

void PulsadorConfigToInterrupt()
{
    gpio_pad_select_gpio(blink);
    gpio_set_direction(blink, GPIO_MODE_OUTPUT);

    gpio_config_t configuracionPines; // Para la configuracion de pines

    configuracionPines.intr_type = GPIO_INTR_NEGEDGE;        // interrupt of rising edge
    configuracionPines.pin_bit_mask = MASCARA_PINES_ENTRADA; // bit mask of the pins, use GPIO4/5 here
    configuracionPines.mode = GPIO_MODE_INPUT;               // set as input mode
    configuracionPines.pull_down_en = 1;                     // enable pull_down mode
    gpio_config(&configuracionPines);

    gpio_set_intr_type(ENTRADA_0, GPIO_INTR_NEGEDGE); // change gpio intrrupt type for one pin

    gpio_install_isr_service(FLAG_INTERRUPCION);                        // install gpio isr service
    gpio_isr_handler_add(ENTRADA_0, gpioIsrHandler, (void *)ENTRADA_0); // hook isr handler for specific gpio pin
}

static void IRAM_ATTR gpioIsrHandler(void *arg)
{
    if (EstadoLCD)
    {
        EstadoLCD = false;
    }
    else
    {
        EstadoLCD = true;
    }
}

// Configuracion de la UART 2
void Uart_init2(void)
{
    const uart_config_t uart_config_2 = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    uart_driver_install(UART_NUM_2, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_2, &uart_config_2);
    uart_set_pin(UART_NUM_2, TXD_PIN2, RXD_PIN2, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

//========================= Funciones =========================
void rx_uart(void)
{
    uint8_t *data = (uint8_t *)malloc(RX_BUF_SIZE + 1);
    const char *str = "GPGGA";
    char *p = NULL;
    char str1[100];
    int j = 0;

    char *str2 = (char *)malloc(sizeof(str1));

    const int rxbytes = uart_read_bytes(UART_NUM_2, data, RX_BUF_SIZE, 100 / portTICK_RATE_MS);

    if (rxbytes > 0)
    {
        p = strstr((const char *)data, str);
        if (p)
        {
            lectura_gps = true;
            for (int i = 0; p[i] != '\n'; i++)
            {
                str2[j] = p[i];
                j++;
            }
            str2[j + 1] = '\n';

            data[rxbytes] = 0;

        const int len = j + 2;
        uart_write_bytes(UART_NUM_0, (const char *)str2, len);
        getstring((char *)str2);

        }
    }
    else
    {

        /*data[rxbytes]= 0;
       uart_write_bytes(UART_NUM_0, (const char*)data,rxbytes); */
    }
}
void getstring(char *str)
{
    if (lectura_gps)
    {

        int i = 0;
        int cnt = 0;
        int k = 0;
        int n = 0;
        char new[50];
        // char new1[50];
        while (cnt < 2)
        {
            if (str[i] == ',')
            {
                cnt++;
            }
            i++;
        }
        int j = 0;
        str[29] = '\n';
        for (j = i; j < i + 26; j++)
        {
            new[k] = str[j];
            k++;
        }
        new[k] = '\n';
        lcd_gotoxy(1, 1);
        while (new[n] != '\n')
        {
            lcdData(new[n]);
            n++;
        }

        lcd_gotoxy(1, 2);
        for (int p = n + 1; new[p] != '\n'; p++)
        {
            lcdData(new[p]);
        }
        /*new1[k] = '\n';
        lcd_gotoxy(7,2);
        for(int t=n+1;new1[t]!= '\n';t++)
        {
           lcdData(new[t]);
        }*/
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

