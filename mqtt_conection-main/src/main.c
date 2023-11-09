#include <stdio.h>
#include <stdlib.h>
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
#include "../include/Wifi.h"
#include "freertos/semphr.h"      //incluyo la libreria del semaforo
#define T 10 / portTICK_PERIOD_MS // tiempo en ms de espera para volver a ingresar a la función de actualizar semaforo

extern int cambioModo;
esp_err_t crear_tarea(void);
SemaphoreHandle_t mux2;
void tarea_gps(void *taskParmPtr);
void tarea_adc(void *taskParmPtr);
void tarea_guardar(void *taskParmPtr);
// void tarea_memoria( void* taskParmPtr );
// void vTask_adc(void *pvParameters);
#define STACK_SIZE 3072
#define STACK_SIZE2 3072
static const int RX_BUF_SIZE = 2048;
void getstring(char *str);
void guardarcoord(char *str);
float LecturaADC = 0;
gpio_int_type_t led1 = GPIO_NUM_2;
void Convert_Float_char(float Floats,char cadenas[]);

void app_main()
{
    mux2 = xSemaphoreCreateMutex();
    inicializarBoton(GPIO_NUM_25);
    I2C_init();    // Inicializo el i2c
    LCDI2C_init(); // Inicializo el LCD
    // init_uart_0();
    Uart_init2();
    actualizardisplay(cambioModo);
    // init_isr();
    crear_tarea();
    // iniciarSPI();          //Inicializamos SPI
    // config_esclavo();      //CONFIGURAMOS ESCLAVO
}

esp_err_t crear_tarea(void)
{

    /////TAREA GPS////
    BaseType_t res = xTaskCreatePinnedToCore(
        tarea_gps,                    // Funcion de la tarea a ejecutar
        "tareagps",                   // Nombre de la tarea como String amigable para el usuario
        configMINIMAL_STACK_SIZE * 3, // Cantidad de stack de la tarea
        NULL,                         // Parametros de tarea
        tskIDLE_PRIORITY,             // Prioridad de la tarea -> Queremos que este un nivel encima de IDLE
        NULL,                         // Puntero a la tarea creada en el sistema
        1                             // Procesador donde se ejecuta
    );
    // Gestion de errores
    if (res == pdFAIL)
    {
        printf("Error al crear la tarea.\r\n");
        while (true)
            ; // si no pudo crear la tarea queda en un bucle infinito
    }

    /////TAREA ADC////
    BaseType_t res1 = xTaskCreatePinnedToCore(
        tarea_adc,                    // Funcion de la tarea a ejecutar
        "tareaadc",                   // Nombre de la tarea como String amigable para el usuario
        configMINIMAL_STACK_SIZE * 1, // Cantidad de stack de la tarea
        NULL,                         // Parametros de tarea
        tskIDLE_PRIORITY + 1,         // Prioridad de la tarea -> Queremos que este un nivel encima de IDLE
        NULL,                         // Puntero a la tarea creada en el sistema
        1                             // Procesador donde se ejecuta
    );
    // Gestion de errores
    if (res1 == pdFAIL)
    {
        printf("Error al crear la tarea.\r\n");
        while (true)
            ; // si no pudo crear la tarea queda en un bucle infinito
    }

    GlobalQueue = xQueueCreate(50, sizeof(50));
    GlobalQueue1 = xQueueCreate(10, sizeof(10));

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

void tarea_adc(void *taskParmPtr)
{
    float suma = 0;
    float promedio = 0;
    int contador = 0;
    float valor_dB = 0;
    float voltaje = 0;
    char Guardar_val_pot[10];
    char *pasar_dato;
    adc1_config_width(ADC_WIDTH_12Bit); // configuran la resoluncion
    adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_11db);

    while (true)
    {
        // bucle del ADC
        LecturaADC = adc1_get_raw(ADC1_CHANNEL_6); // Leo el ADC
        
        if (contador < 10)
        {
            suma = suma + LecturaADC;
            contador++;
        }

        if (contador == 10)
        {
            promedio = suma / 10;
            voltaje = promedio * ( 3.3 / 4096);
            valor_dB = ((voltaje *18.65) -50)*(-1);
            Convert_Float_char(valor_dB,Guardar_val_pot);
            pasar_dato = Guardar_val_pot;
            xQueueSend(GlobalQueue1, &pasar_dato, pdMS_TO_TICKS(100));

            if (mux2 != NULL)
            {
                if (xSemaphoreTake(mux2, portMAX_DELAY) == pdTRUE)
                {
                    if (cambioModo == 2)
                    {
                        //
                        lcd_gotoxy(1, 2);
                        lcd_print("-");
                        lcd_gotoxy(2, 2);
                        Print_Float_LCD(valor_dB); // Imprimo los datos del ADC
                        lcd_gotoxy(10, 2);
                        lcd_print("dBm");
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
    char *p;
    char str1[1000];
    int j = 0;
    // gpio_set_direction(led1, GPIO_MODE_OUTPUT);
    uint8_t *data = (uint8_t *)malloc(RX_BUF_SIZE);
    char *str2= (char *)malloc(sizeof(str1)); 
    char str3[100];
    char *pasar_dato_gps;
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
                        
                        p = strstr((const char *)data, str); // busco en toda la cadena de caracteres que viene del gps la palabra "GPGGA"
                        if (p)
                        {
                            for (int i = 0; p[i] != '\n'; i++)
                            {
                                str2[j] = p[i];
                                str3[j] = p[i];
                                j++;
                            }
                            str2[j + 1] = '\n';
                            data[rxbytes] = 0;
                            pasar_dato_gps = str3;
                            xQueueSend(GlobalQueue, &pasar_dato_gps, pdMS_TO_TICKS(100));

                            getstring((char *)str2);


                            //uart_flush(UART_NUM_2);


                        }
                    }
                }
            }
        }
        xSemaphoreGive(mux2);
   
        vTaskDelay(5000 / portTICK_PERIOD_MS);

    }
}

void getstring(char *str)
{
    int i = 0;
    int cnt = 0;
    int k = 0;
    int n = 0;
    char new[50];
    int p = 0;
    while (cnt < 2)
    {
        if (str[i] == ',')
        {
            cnt++; // incrementa i para saber cuantos caracteres tiene #latitud y cuantos #longitud
        }          // Para cuando encuentra una coma(,) e incrementa i
        i++;
    }
    int j = 0;
    // gpio_set_level(led1, 1);
    str[29] = '\n';
    for (j = i; j < i + 26; j++)
    {
        new[k] = str[j];
        k++;
    }
    new[k] = '\n';
    BorrarPantalla();
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
    for (n = 0; new[n] != '\n'; n++)
    {
        lcdData(new[n]);
        vTaskDelay(2 / portTICK_PERIOD_MS);
    }
    lcd_gotoxy(1, 2);
    for (p = n + 1; new[p] != '\n'; p++)
    {
        lcdData(new[p]);
        vTaskDelay(2 / portTICK_PERIOD_MS);
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
uint16_t Mils, Centenas, Decenas, Unidads, Decimas, Centesimas;

/*	Funcion que recibe un numero flotante, desplaza la coma de la 
	variable de tipo flotante y guarda cada unidad En forma de 
	enteros y despues los imprime
	
	esta funcion solo guarda las decenas, unidades, decimales y
	centesimas*/
void Convert_Float_char(float Floats, char cadenas[])
{
    float Flotantes = Floats; 
    //char cadena[8];

    /*	si flotante no es mayor a 999 Mil es cero,
		caso contrario obtengo el valor y lo guardo
		en la variable Mil*/
	if (Flotantes>999)
	{
		/*
		113.14/10 --> 1.1314
		Decena = 1 
		*/
		Mils = Flotantes/1000;
		/*
		113.14 - 1*100 --> 13.14 
		*/
		Flotantes = Flotantes - (Mils*1000);
	}
	else
	{
		Mils=0;
	}

	/*	si flotante no es mayor a 99 Centena es cero,
		caso contrario obtengo el valor y lo guardo
		en la variable Centena*/
	if (Flotantes>99)
	{
		/*
		113.14/10 --> 1.1314
		Decena = 1 
		*/
		Centenas = Flotantes/100;
		/*
		113.14 - 1*100 --> 13.14 
		*/
		Flotantes = Flotantes - (Centenas*100);
	}
	else
	{
		Centenas=0;
	}
	
	/*	si flotante no es mayor a 9 Decena es cero,
		caso contrario obtengo el valor y lo guardo
		en la variable decena*/
	if (Flotantes>9)
	{
		/*
		13.14/10 --> 1.314
		Decena = 1 
		*/
		Decenas = Flotantes/10;
		/*
		13.14 - 1*10 --> 3.14 
		*/
		Flotantes = Flotantes - (Decenas*10);
	}
	else
	{
		Decenas=0;
	}
	/*
		Unidad --> 3
	*/
	Unidads = Flotantes;
	
	/*
		3.14 - 3 --> 0.14
	*/
	Flotantes = Flotantes - Unidads;
	
	/*
		0.14 * 10 --> 1.4
	*/
	Decimas = Flotantes*10;
	
	/*
		como no puedo convertir un entero en decimal
		tengo que desplazarle la coma al float y hacer la resta
		
		0.14*100 - 1*10 --> 14-10 --> 4 
	*/
	Centesimas = Flotantes*100 - Decimas*10;

	//Desplazo 48 valores por el codigo ascci
    
    cadenas[0] = '-' ;
    cadenas[1] = Centenas +'0';
    cadenas[2] = Decenas+ '0';
    cadenas[3] = Unidads+ '0';
    cadenas[4] =  ',';
    cadenas[5] = Decimas+ '0';
    cadenas[6] = Centesimas+ '0';
    cadenas[7] = 'd';
    cadenas[8] = 'B';
    cadenas[9] = 'm';
    cadenas[10] = '\0';

}