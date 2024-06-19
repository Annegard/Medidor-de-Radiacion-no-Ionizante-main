/**
 * @file main.c
 * @author Alejo Casas, Gaston Pautasso
 * @brief 
 * @version 4.1
 * @date 2023-11-20
 * 
 * @copyright Copyright (c) 2023
 * 
 * Proyecto Para la depuracion de distintas etapas de reversiones anteriores de codigo
 */

#include "driver/gpio.h"    //pines generales
#include "esp_adc/adc_oneshot.h"     //ADC

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"  //Libreria Tareas
#include "freertos/semphr.h"//Libreria Semaforo
#include "esp_log.h"

// librerias propias
#include "../include/I2C.h"
#include "../include/LCDI2C.h"
#include "../include/pulsador.h"
#include "../include/GPS_UART.h"
#include "../include/Wifi.h"


// //=========================== Variables y Definiciones ================================
// DEFINICIONES
#define T 10 / portTICK_PERIOD_MS // tiempo en ms de espera para volver a ingresar a la función de actualizar semaforo
#define PROCESADORA         0
#define PROCESADORB         1
#define Promedio 10

#define DATA_QUEUE_SIZE 10

SemaphoreHandle_t mux2;

estadoMODO_t estadoModo;

uint8_t Auxiliar=0;

float voltaje_promedio = 0;
float valor_dB = 0;

float Latitud = 0;
float Longitud = 0;
uint8_t Hora =0;
uint8_t Minuto=0;
uint8_t Segundos=0;
uint8_t Dia=0;
uint8_t Mes=0;

esp_err_t CrearTareaADC(void);
esp_err_t CrearTareaPulsador(gpio_int_type_t pulsador);

void tarea_adc(void *taskParmPtr);
void tarea_Pulsador(void *taskParmPtr);

void actualizardisplay();

QueueHandle_t ColaADC;
QueueHandle_t ColaLAT;
QueueHandle_t ColaLON;

bool MQTT_CONNEECTED = false;

// //=========================== Función principal ================================
void app_main()
{
    mux2 = xSemaphoreCreateMutex();
    estadoModo=CONECTAR_WIFI;//Inicializo la variable estadoModo
    Auxiliar = estadoModo;//cargo en la variable auxiliar el estadoModo para el pulsador

    ColaADC = xQueueCreate(DATA_QUEUE_SIZE, sizeof(int));
    ColaLAT = xQueueCreate(DATA_QUEUE_SIZE, sizeof(int));
    ColaLON = xQueueCreate(DATA_QUEUE_SIZE, sizeof(int));


    //init_Uart0();//115200 baudios a CPU
    init_Uart2();//9600 baudios a GPS

    I2C_init();    // Inicializo el i2c
    LCDI2C_init(); // Inicializo el LCD 

    BorrarPantalla();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    lcd_print("HOLA MUNDO");
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    //Creo Tareas
    CrearTareaGPS_UART();
    CrearTareaADC(); 
    CrearTareaPulsador(GPIO_NUM_25);
    subir_datos_MQTT();

    //El while de cualquier bucle en la funcion principal se
    //ejecuta en el procesador 0
    while (true)
    {
        actualizardisplay();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

esp_err_t CrearTareaPulsador(gpio_int_type_t pulsador){

        inicializarBoton(pulsador);
        
        BaseType_t res1 = xTaskCreatePinnedToCore(
        tarea_Pulsador,                    // Funcion de la tarea a ejecutar
        "tareaPulsador",                   // Nombre de la tarea como String amigable para el usuario
        configMINIMAL_STACK_SIZE*3, // Cantidad de stack de la tarea
        (void*) pulsador,                         // Parametros de tarea
        tskIDLE_PRIORITY+1,         // Prioridad de la tarea -> Queremos que este un nivel encima de IDLE
        NULL,                         // Puntero a la tarea creada en el sistema
        PROCESADORA                   // Procesador donde se ejecuta
    );
    // Gestion de errores
    if (res1 == pdFAIL)
    {
        printf("Error al crear la tarea ADC.\r\n");
        while (true)
            ; // si no pudo crear la tarea queda en un bucle infinito
    }
    return ESP_OK;
}

void tarea_Pulsador(void* parametros )
{
    gpio_int_type_t boton = (gpio_int_type_t) parametros; // Convertir el puntero a int
    
    //Bucle principal
    while (true)
    { 
        actualizarBoton(boton);

        if(estadoModo!=Auxiliar){
            if (mux2 != NULL){
		        if (xSemaphoreTake(mux2, portMAX_DELAY) == pdTRUE){
                    printf("Estado Modo: %i \n", estadoModo);
                    
                    Auxiliar = estadoModo;
                }
			}
        }
        xSemaphoreGive(mux2);
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
   
}

esp_err_t CrearTareaADC(void)
{
    /////TAREA ADC////
    BaseType_t res1 = xTaskCreatePinnedToCore(
        tarea_adc,                    // Funcion de la tarea a ejecutar
        "tareaadc",                   // Nombre de la tarea como String amigable para el usuario
        configMINIMAL_STACK_SIZE * 3, // Cantidad de stack de la tarea
        NULL,                         // Parametros de tarea
        tskIDLE_PRIORITY + 1,         // Prioridad de la tarea -> Queremos que este un nivel encima de IDLE
        NULL,                         // Puntero a la tarea creada en el sistema
        PROCESADORB                   // Procesador donde se ejecuta
    );
    // Gestion de errores
    if (res1 == pdFAIL)
    {
        printf("Error al crear la tarea ADC.\r\n");
        while (true)
            ; // si no pudo crear la tarea queda en un bucle infinito
    }

   return ESP_OK;
}

/** Tarea "tarea_adc":
 *  Tarea que congifura el ADC1 del microcontrolador y ejecuta un bucle infinito donde lee constantemente
 *  el pin configurado
*/
int AdcRawValueChannel_0 = 0;
adc_oneshot_unit_handle_t Adc1Handle;

void tarea_adc(void *taskParmPtr)
{
    uint8_t contador = 0;
    float suma = 0;
    float voltaje = 0;
    
    //----- SETUP ADC1 -----

     /** 
    *   ADC1_CHANNEL_0      ADC1 channel 0 is GPIO36
    *   ADC1_CHANNEL_1,     ADC1 channel 1 is GPIO37
    *   ADC1_CHANNEL_2,     ADC1 channel 2 is GPIO38
    *   ADC1_CHANNEL_3,     ADC1 channel 3 is GPIO39
    *   ADC1_CHANNEL_4,     ADC1 channel 4 is GPIO32
    *   ADC1_CHANNEL_5,     ADC1 channel 5 is GPIO33
    *   ADC1_CHANNEL_6,     ADC1 channel 6 is GPIO34
    *   ADC1_CHANNEL_7,     ADC1 channel 7 is GPIO35 
    *   ADC1_CHANNEL_MAX,
    *
    *   adc_atten_t: (PARAMETRO ATENUACION)
    * 
    *   Entre más alta sea la atenuación es posible leer valores de voltaje mayores.
    *       +----------+-------------+-----------------+
    *       |          | attenuation | suggested range |
    *       |    SoC   |     (=dB)    |      (mV)       |
    *       +==========+============+=================+
    *       |          |       0     |    100 ~  950   |
    *       |          +-------------+-----------------+
    *       |          |       2.5   |    100 ~ 1250   |
    *       |   ESP32  +-------------+-----------------+
    *       |          |       6     |    150 ~ 1750   |
    *       |          +-------------+-----------------+
    *       |          |      11     |    150 ~ 2450   |
    *       +----------+-------------+-----------------+
    *  
    *   typedef enum {
    *      ADC_ATTEN_DB_0   = 0,  ///<No input attenuation, ADC can measure up to approx.
    *      ADC_ATTEN_DB_2_5 = 1,  ///<The input voltage of ADC will be attenuated extending the range of measurement by about 2.5 dB (1.33 x)
    *      ADC_ATTEN_DB_6   = 2,  ///<The input voltage of ADC will be attenuated extending the range of measurement by about 6 dB (2 x)
    *      ADC_ATTEN_DB_11  = 3,  ///<The input voltage of ADC will be attenuated extending the range of measurement by about 11 dB (3.55 x)
    *   } adc_atten_t;
    */
	adc_oneshot_unit_init_cfg_t Adc1InitConfig = {
		.unit_id = ADC_UNIT_1,
	};
	adc_oneshot_new_unit(&Adc1InitConfig, &Adc1Handle);

	adc_oneshot_chan_cfg_t Adc1Config = {
		.bitwidth = ADC_BITWIDTH_12,
		.atten = ADC_ATTEN_DB_11,
	};
    //el ADC con el pin GPIO34 y una atenuacion de 11DB (150 ~ 2450 mV)
	ESP_ERROR_CHECK(adc_oneshot_config_channel(Adc1Handle, ADC_CHANNEL_6, &Adc1Config));

    // bucle del ADC
    while (true)
    {
        if(estadoModo == MEDIR_POTENCIA || estadoModo == CONECTAR_WIFI){
            //----- Lectura Canal 6 -----
            if (adc_oneshot_read(Adc1Handle, ADC_CHANNEL_6, &AdcRawValueChannel_0) == ESP_OK){
                
                voltaje = AdcRawValueChannel_0 * ( 2.6 / 4096);// 2,6 es el voltaje maximo

                if (contador < 10){
                    suma = suma + voltaje;
                    contador++;
                }
            
                else if (contador == 10){
                    if (mux2 != NULL){
		                if (xSemaphoreTake(mux2, portMAX_DELAY) == pdTRUE){
                            voltaje_promedio = suma / 10;
                            valor_dB = ((voltaje_promedio *18.65) -50)*(-1);
                            
                            // printf("Lectura Voltaje[V]: %f \n", voltaje_promedio);
                            // printf("Lectura Potencia[dB]: %f \n", valor_dB);
                        }
                    }
                    xSemaphoreGive(mux2);
                    
                    contador = 0;
                    suma = 0;
                }
            }
        }
        
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void actualizardisplay(){
    BorrarPantalla();
    switch (estadoModo)
    {
      case MENU_PRINCIPAL:
        printf("menu principal\n");
        lcd_print("menu principal");
        //
        //  lcd_gotoxy(1, 1);
        //lcd_print("1-GPS");
        //  lcd_gotoxy(9, 1);
        //  lcd_print("2-ADC");
        //  lcd_gotoxy(1, 2);
        //  lcd_print("3-SAVE");
        //  lcd_gotoxy(9, 2);
        //  lcd_print("4-SEND");

        break;

    case CONECTAR_GPS:
        printf("gps\n");
        printf("Longitud: %f \n", Longitud);
        printf("Latitud: %f \n", Latitud);
        //xQueueSend(ColaLAT, &Latitud, pdMS_TO_TICKS(100));
        //xQueueSend(ColaLON, &Longitud, pdMS_TO_TICKS(100));
        
        //lcd_print("GPS");

        lcd_gotoxy(1, 1);
        lcd_print("LON:");
        lcd_gotoxy(4, 1);
        Print_Float_LCD(Longitud, 6/*Cantidad de decimas*/);

        lcd_gotoxy(1, 2);
        lcd_print("LAT:");
        lcd_gotoxy(4, 2);
        Print_Float_LCD(Latitud, 6/*Cantidad de decimas*/);
         break;

    case MEDIR_POTENCIA:
        printf("adc\n");
        printf("Lectura Voltaje[V]: %f \n", voltaje_promedio);
        printf("Lectura Potencia[dB]: %f \n", valor_dB);

        //xQueueSend(ColaADC, &valor_dB, pdMS_TO_TICKS(100));

        //lcd_print("ADC");

        lcd_gotoxy(1, 1);
        lcd_print("Vol:");
        lcd_gotoxy(5, 1);
        Print_Float_LCD(voltaje_promedio, 4/*Cantidad de decimas*/);
        lcd_gotoxy(14, 1);
        lcd_print("V");

        lcd_gotoxy(1, 2);
        lcd_print("Pot:");
        lcd_gotoxy(5, 2);
        Print_Float_LCD(valor_dB, 4/*Cantidad de decimas*/);
        lcd_gotoxy(14, 2);
        lcd_print("dBm");
        break;
    case CONECTAR_WIFI:
        //printf("wifi\n");
        lcd_print("WIFI-MQTT:");
        xQueueSend(ColaADC, &valor_dB, pdMS_TO_TICKS(100));
        xQueueSend(ColaLAT, &Latitud, pdMS_TO_TICKS(100));
        xQueueSend(ColaLON, &Longitud, pdMS_TO_TICKS(100));

        if (MQTT_CONNEECTED){
            lcd_print("conect");
        }
        else{
            lcd_print("uncon.");
        }
        
        lcd_gotoxy(1, 2);
        lcd_print("SSID:");
        lcd_gotoxy(6, 2);
        lcd_print(WIFI_SSID);

        //  BorrarPantalla();
        //  lcd_gotoxy(1, 1);
        //  lcd_print("Conectando");
        //  lcd_gotoxy(1, 2);
        //  lcd_print("Wi-Fi...");
         //subir_datos();
        break;
    case GUARDAR_DATOS:
        Print_Float_LCD(Hora,0);
        lcd_print(":");
        lcd_gotoxy(4, 1);
        Print_Float_LCD(Minuto,0);
        lcd_print(":");
        lcd_gotoxy(7, 1);
        Print_Float_LCD(Segundos,0);

        lcd_gotoxy(1, 2);
        Print_Float_LCD(Dia,0);
        lcd_print("/");
        Print_Float_LCD(Mes,0);
        break;
      default:
        //  lcd_gotoxy(1, 1);
        //  lcd_print("default");
         //errorBoton();
         break;
   }
}