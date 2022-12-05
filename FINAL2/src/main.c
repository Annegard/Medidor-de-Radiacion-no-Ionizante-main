#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/adc.h"
#include "../include/pulsador.h"
#include "driver/gpio.h"
#include "../include/I2C.h"
#include "../include/LCDI2C.h"
#include "../include/Uart.h"
#include "../include/memoria.h"
#include "freertos/semphr.h" //incluyo la libreria del semaforo

#define T 10 / portTICK_PERIOD_MS // tiempo en ms de espera para volver a ingresar a la función de actualizar semaforo


extern int cambioModo;

esp_err_t crear_tarea(void);
SemaphoreHandle_t mux2 ;
void tarea_gps( void* taskParmPtr );
void tarea_adc( void* taskParmPtr );
//void tarea_memoria( void* taskParmPtr );
//void vTask_adc(void *pvParameters);
#define STACK_SIZE 3072
#define STACK_SIZE2 3072
static const int RX_BUF_SIZE=3072;
void getstring(char *str);
float LecturaADC=0;
gpio_int_type_t led1 = GPIO_NUM_2;


void app_main()
{
    mux2 = xSemaphoreCreateMutex();
    inicializarBoton(GPIO_NUM_25);
    I2C_init();    // Inicializo el i2c
    LCDI2C_init(); // Inicializo el LCD
    //init_uart_0();
    Uart_init2(); 
    actualizardisplay(cambioModo);
    //init_isr();
    crear_tarea();
    //iniciarSPI();          //Inicializamos SPI
    //config_esclavo();      //CONFIGURAMOS ESCLAVO

}

esp_err_t crear_tarea(void)
{
    /////TAREA GPS////
    /////TAREA GPS////
    /////TAREA GPS////
        BaseType_t res = xTaskCreatePinnedToCore(
    	tarea_gps,                     	// Funcion de la tarea a ejecutar
        "tareagps",   	                // Nombre de la tarea como String amigable para el usuario
        configMINIMAL_STACK_SIZE*3, 		// Cantidad de stack de la tarea
        NULL,                          	// Parametros de tarea
        tskIDLE_PRIORITY,         	// Prioridad de la tarea -> Queremos que este un nivel encima de IDLE
        NULL,                          		// Puntero a la tarea creada en el sistema
        1                                   // Procesador donde se ejecuta
    );
    // Gestion de errores
	if(res == pdFAIL)
	{
		printf( "Error al crear la tarea.\r\n" );
		while(true);					// si no pudo crear la tarea queda en un bucle infinito
	}

    /////TAREA ADC////
    /////TAREA ADC////
    /////TAREA ADC////
        BaseType_t res1 = xTaskCreatePinnedToCore(
    	tarea_adc,                     	// Funcion de la tarea a ejecutar
        "tareaadc",   	                // Nombre de la tarea como String amigable para el usuario
        configMINIMAL_STACK_SIZE*1, 		// Cantidad de stack de la tarea
        NULL,                          	// Parametros de tarea
        tskIDLE_PRIORITY+1,         	// Prioridad de la tarea -> Queremos que este un nivel encima de IDLE
        NULL,                          		// Puntero a la tarea creada en el sistema
        1                                   // Procesador donde se ejecuta
    );
    // Gestion de errores
	if(res1 == pdFAIL)
	{
		printf( "Error al crear la tarea.\r\n" );
		while(true);					// si no pudo crear la tarea queda en un bucle infinito
	}
       
        /////TAREA MEMORIA////
        /////TAREA MEMORIA////
        /////TAREA MEMORIA////
        /*BaseType_t res2 = xTaskCreatePinnedToCore(
    	tarea_memoria,                     	// Funcion de la tarea a ejecutar
        "tareamemoria",   	                // Nombre de la tarea como String amigable para el usuario
        configMINIMAL_STACK_SIZE*2, 		// Cantidad de stack de la tarea
        NULL,                          	// Parametros de tarea
        tskIDLE_PRIORITY+1,         	// Prioridad de la tarea -> Queremos que este un nivel encima de IDLE
        NULL,                          		// Puntero a la tarea creada en el sistema
        0                                   // Procesador donde se ejecuta
    );
    // Gestion de errores
	if(res == pdFAIL)
	{
		printf( "Error al crear la tarea.\r\n" );
		while(true);					// si no pudo crear la tarea queda en un bucle infinito
	}
*/
    return ESP_OK;
}

void tarea_adc( void* taskParmPtr ){
    float suma=0;
    float promedio=0;
    int contador=0;
    float valor_dB=0;
    float voltaje = 0;
    adc1_config_width(ADC_WIDTH_9Bit); // configuran la resoluncion
    adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_11db);
    while (true)
    {
        // // bucle del ADC

        LecturaADC = adc1_get_raw(ADC1_CHANNEL_6); // Leo el ADC
        
        if(contador<10){
        suma = suma + LecturaADC;
        contador++;
        }
        if (contador == 10)
        {
            promedio = suma / 10;
            voltaje = promedio*(3.3/512);
            valor_dB = voltaje/0.053;
                if (mux2 != NULL)
                {
                    if (xSemaphoreTake(mux2, portMAX_DELAY) == pdTRUE)
                    {
                        if (cambioModo == 2){
                        lcd_gotoxy(1, 2);
                        Print_Float_LCD(valor_dB); // Imprimo los datos del ADC
                        lcd_gotoxy(9, 2);
                        lcd_print("dB");
                        // vTaskDelay(1000 / portTICK_PERIOD_MS);
                    }
                }
                xSemaphoreGive(mux2);
            }
            contador = 0;
            suma = 0;
            promedio = 0;
        }
        
        vTaskDelay(100 / portTICK_PERIOD_MS);
}
}
void tarea_gps(void *taskParmPtr)
{
    const char *str = "GPGGA";
    char *p = NULL;
    char str1[500];
    int j = 0;
    // gpio_set_direction(led1, GPIO_MODE_OUTPUT);
    uint8_t *data = (uint8_t *)malloc(RX_BUF_SIZE + 1);
    char *str2 = (char *)malloc(sizeof(str1));
    while (1)
    {

        if (mux2 != NULL)
        {
            if (xSemaphoreTake(mux2, portMAX_DELAY) == pdTRUE)
            {
                if (cambioModo == 1)
                {
                    const int rxbytes = uart_read_bytes(UART_NUM_2, data, RX_BUF_SIZE, 5000 / portTICK_RATE_MS);
                    if (rxbytes > 0)
                    {
                        p = strstr((const char *)data, str);
                        if (p)
                        {
                            for (int i = 0; p[i] != '\n'; i++)
                            {
                                str2[j] = p[i];
                                j++;
                            }
                            str2[j + 1] = '\n';
                            const int len = j + 2;
                            data[rxbytes] = 0;
                            //uart_write_bytes(UART_NUM_0, (const char *)str2, len);

                            // lcd_gotoxy(1, 1);
                            // lcd_print("ENTER ");
                            getstring((char *)str2);
                        }
                    }
                }
            }
        }
        xSemaphoreGive(mux2);
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
}
/*
void tarea_memoria( void* taskParmPtr ){
    
}*/
void getstring(char *str)
{
    int i = 0;
    int cnt = 0;
    int k = 0;
    int n=0;
    char new[50];
    int p=0;
    //char new1[50];    
    while (cnt < 2)
    {
        if (str[i] == ',')
        {
            cnt++;
        }
        i++;
    }
    int j = 0;
    //gpio_set_level(led1, 1);
    str[29] = '\n';
    for (j = i; j < i + 26; j++)
    {
        new[k] = str[j];
        k++;
    }
    new[k] = '\n';
    lcd_gotoxy(1, 1);
/*
    while (new[n] != '\n')
    {
        lcdData(new[n]);
        vTaskDelay(1 / portTICK_PERIOD_MS);
        n++;
    }
    //gpio_set_level(led1, 0);
    lcd_gotoxy(1, 2);
    p= n+1;
        while (new[p] != '\n')
    {
        lcdData(new[p]);
        vTaskDelay(1 / portTICK_PERIOD_MS);
        p++;
    }
*/
    for ( n = 0; new[n] != '\n'; n++)
    {
        lcdData(new[n]);
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }

    lcd_gotoxy(1, 2);

    for (p = n + 1; new[p] != '\n'; p++)
    {
        lcdData(new[p]);
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

/*void vTask_gps(void *pvParameters)
{
    while (1)
    {
        rx_uart();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}*/

/*void vTask_adc(void *pvParameters)
{
    while (1)
    {
        
        //lcd_gotoxy(1, 1);
        //lcd_print("TAREA ADC");
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}*/