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
#include "../include/LCDI2C.h"
#include "../include/pulsador.h"
#include "../include/GPS_UART.h"
#include "../include/Wifi.h"
#include "../include/SDCARD_SPI.h"

// //=========================== Variables y Definiciones ================================
// DEFINICIONES
#define T 10 / portTICK_PERIOD_MS // tiempo en ms de espera para volver a ingresar a la función de actualizar semaforo
#define PROCESADORA         0

#define PROCESADORB         1

#define PRIORIDAD_LCD       4
#define PRIORIDAD_ADC       3
#define PRIORIDAD_GPS       2
#define PRIORIDAD_PULSADOR  1
#define PRIORIDAD_SD        0

#define Promedio 10

#define BOTON_A GPIO_NUM_36
// #define BOTON_B GPIO_NUM_26

#define DATA_QUEUE_SIZE 10

SemaphoreHandle_t Semaforo_Boton = NULL;
SemaphoreHandle_t Semaforo_ADC = NULL;
SemaphoreHandle_t Semaforo_GPS = NULL;

displayMODO_t displayModo;
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

static int64_t tiempo_actual = 0;
static int64_t tiempo_inicial = 0;   

esp_err_t CrearTareaADC(void);
esp_err_t CrearTareaPulsador(gpio_int_type_t pulsador);
esp_err_t CrearTareaSD_SPI();
esp_err_t CrearTareaLCD(void);
esp_err_t CrearTareaGPS_UART(void);
esp_err_t CrearTareaWIFI_MQTT(int refrezco);

static void BorrarTareaWIFI_MQTT();
static void tarea_adc(void *taskParmPtr);
static void tarea_Pulsador(void *taskParmPtr);
static void tarea_LCD();
static void tarea_WIFI(void* parametros);
static void actualizardisplay(int displayModo, int fila);

QueueHandle_t ColaADC;

TaskHandle_t punteroTareaWIFI;

bool MQTT_CONNEECTED = false;
bool SD_CONECTADA = false;
bool ESPACIO_EN_SD = false;
bool MODO_WIFI = false;

// //=========================== Función principal ================================
void app_main()
{
    
    Semaforo_Boton = xSemaphoreCreateMutex();
    Semaforo_ADC = xSemaphoreCreateMutex();
    Semaforo_GPS = xSemaphoreCreateMutex();

    estadoModo=POT-1;//Inicializo la variable estadoModo
    Auxiliar = estadoModo;//cargo en la variable auxiliar el estadoModo para el pulsador

    vTaskDelay(2000 / portTICK_PERIOD_MS);

    //Creo Tareas en el procesador B
    CrearTareaLCD();
    CrearTareaGPS_UART();
    CrearTareaADC();
    // CrearTareaPulsador(BOTON_A);//Recibe de parametro el boton a leer
    // CrearTareaSD_SPI();//Recibe de parametro cada cuantos segundos guarda los datos

    //Creo Tareas en el procesador A
    // CrearTareaWIFI_MQTT(1);//Recibe de parametro cada cuantos segundos guarda los datos
}

esp_err_t CrearTareaLCD(){

    BaseType_t res = xTaskCreatePinnedToCore(
        tarea_LCD,                    // Funcion de la tarea a ejecutar
        "tareaLCD",                   // Nombre de la tarea como String amigable para el usuario
        configMINIMAL_STACK_SIZE*3, // Cantidad de stack de la tarea
        NULL,                         // Parametros de tarea
        tskIDLE_PRIORITY+PRIORIDAD_LCD,         // Prioridad de la tarea -> Queremos que este un nivel encima de IDLE
        NULL,                         // Puntero a la tarea creada en el sistema
        PROCESADORB                   // Procesador donde se ejecuta
    );
    // Gestion de errores
    if (res == pdFAIL)
    {
        printf("Error al crear la tarea ADC.\r\n");
        while (true)
            ; // si no pudo crear la tarea queda en un bucle infinito
    }
    return ESP_OK;
}

esp_err_t CrearTareaPulsador(gpio_int_type_t pulsador){

    BaseType_t res = xTaskCreatePinnedToCore(
        tarea_Pulsador,                    // Funcion de la tarea a ejecutar
        "tareaPulsador",                    // Nombre de la tarea como String amigable para el usuario
        configMINIMAL_STACK_SIZE*3, // Cantidad de stack de la tarea
        (void*) pulsador,                         // Parametros de tarea
        tskIDLE_PRIORITY+PRIORIDAD_PULSADOR,         // Prioridad de la tarea -> Queremos que este un nivel encima de IDLE
        NULL,                         // Puntero a la tarea creada en el sistema
        PROCESADORB                   // Procesador donde se ejecuta
    );
    // Gestion de errores
    if (res == pdFAIL){
        printf("Error al crear la tarea ADC.\r\n");
        while (true)
            ; // si no pudo crear la tarea queda en un bucle infinito
    }

    return ESP_OK;
}

esp_err_t CrearTareaADC(void){
    /////TAREA ADC////
    BaseType_t res = xTaskCreatePinnedToCore(
        tarea_adc,                    // Funcion de la tarea a ejecutar
        "tareaadc",                   // Nombre de la tarea como String amigable para el usuario
        configMINIMAL_STACK_SIZE * 3, // Cantidad de stack de la tarea
        NULL,                         // Parametros de tarea
        tskIDLE_PRIORITY + PRIORIDAD_ADC,         // Prioridad de la tarea -> Queremos que este un nivel encima de IDLE
        NULL,                         // Puntero a la tarea creada en el sistema
        PROCESADORB                   // Procesador donde se ejecuta
    );
    // Gestion de errores
    if (res == pdFAIL){
        printf("Error al crear la tarea ADC.\r\n");
        while (true)
            ; // si no pudo crear la tarea queda en un bucle infinito
    }

   return ESP_OK;
}

esp_err_t CrearTareaSD_SPI(){
    // Nota sobre porque los parametros que se pasan al crear una tarea deben de ser de tipo void
    // https://stackoverflow.com/questions/70290511/warning-passing-argument-1-of-xtaskcreate
    // "El primer parametro TaskFunction_t, al momento de crear la tarea, se define como una
    // función que devuelve void y toma un puntero void como único parámetro. Todas las
    // funciones que implementan una tarea deben ser de este tipo."

    BaseType_t res5 = xTaskCreatePinnedToCore(
            GuardarDatos,                     	// Funcion de la tarea a ejecutar
            "GuardarDatos",   	                // Nombre de la tarea como String amigable para el usuario
            configMINIMAL_STACK_SIZE*6, 		// Cantidad de stack de la tarea
            NULL,                          	    // Parametros de tarea
            tskIDLE_PRIORITY+PRIORIDAD_SD,         	    // Prioridad de la tarea -> Queremos que este un nivel encima de IDLE
            NULL,                          		// Puntero a la tarea creada en el sistema
            PROCESADORB                         // Procesador donde se ejecuta
        );

    if(res5 == pdFAIL){
        printf( "Error al crear la tarea.\r\n" );
        while(true);					// si no pudo crear la tarea queda en un bucle infinito
    }

    return ESP_OK;
}

esp_err_t CrearTareaGPS_UART(void){

    BaseType_t res = xTaskCreatePinnedToCore(
    	rx_task,            // Funcion de la tarea a ejecutar
        "uart_rx_task",     // Nombre de la tarea como String amigable para el usuario
        1024*4, 		    // Cantidad de stack de la tarea
        NULL,               // Parametros de tarea
        tskIDLE_PRIORITY + PRIORIDAD_GPS,   // Prioridad de la tarea -> Queremos que este un nivel encima de IDLE
        NULL,               // Puntero a la tarea creada en el sistema
        PROCESADORB         //El Nucleo Cero se encarga de tareas como el menu, por eso la dedico al 1
    );

    if(res == pdFAIL){
		printf( "Error al crear la tarea GPS.\r\n" );
		while(true);	    //Si no puedo crear la tarea queda en un bucle infinito
	}

    return ESP_OK;
}

esp_err_t CrearTareaWIFI_MQTT(int refrezco){

    BaseType_t res = xTaskCreatePinnedToCore(
            tarea_WIFI,                     	// Funcion de la tarea a ejecutar
            "tarea_WIFI",   	                // Nombre de la tarea como String amigable para el usuario
            configMINIMAL_STACK_SIZE*6, 		// Cantidad de stack de la tarea
            (void*) refrezco,                   // Parametros de tarea
            tskIDLE_PRIORITY+1,         	    // Prioridad de la tarea -> Queremos que este un nivel encima de IDLE
            &punteroTareaWIFI,                  // Puntero a la tarea creada en el sistema
            PROCESADORA                         // Procesador donde se ejecuta
        );

    if(res == pdFAIL){
        printf( "Error al crear la tarea.\r\n" );
        while(true);					// si no pudo crear la tarea queda en un bucle infinito
    }

    return ESP_OK;
}

static void BorrarTareaWIFI_MQTT(){
    esp_wifi_deinit();
    vTaskDelete(punteroTareaWIFI);
}

static void tarea_Pulsador(void* parametros){

    gpio_int_type_t boton = (gpio_int_type_t) parametros; // Convertir el puntero a int

    inicializarBoton(boton);

    //Bucle principal de la tarea
    while (true){

        //Lee el pulsador, si este detecta una pulsación cambio el estado de "estadoModo"
        actualizarBoton(boton);

        //Si "estadoModo" cambio, utiliza semaforo para poder guardar la variable en caso
        //De que otra tarea se encuentre consultandola
        if(estadoModo!=Auxiliar){
            if (Semaforo_Boton != NULL){
		        if (xSemaphoreTake(Semaforo_Boton, portMAX_DELAY) == pdTRUE){
                    printf("Estado Modo: %i \n", estadoModo);

                    Auxiliar = estadoModo;
                }
			}
            xSemaphoreGive(Semaforo_Boton);
        }
        
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

static void tarea_WIFI(void* parametros){

    uint32_t refrezco = (uint32_t) parametros; // Convertir el puntero a int

    // esp_err_t ret = nvs_flash_init();

    // if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    // {
    //     ESP_ERROR_CHECK(nvs_flash_erase());
    //     ret = nvs_flash_init();
    // }
    // ESP_ERROR_CHECK(ret);

    bool AuxiliarWIFIinit = true;

    while (true)
    {
        if (MODO_WIFI){
            if (AuxiliarWIFIinit){
                wifi_init();
                AuxiliarWIFIinit = false;
                vTaskDelay(2000 / portTICK_PERIOD_MS);
            }

            Publisher_Task();
            vTaskDelay((refrezco*100) / portTICK_PERIOD_MS);
        }
        else{
            AuxiliarWIFIinit = true;
            BorrarTareaWIFI_MQTT();
        }

    }
}

/** Tarea "tarea_adc":
 *  Tarea que congifura el ADC2 del microcontrolador y ejecuta un bucle infinito donde lee constantemente
 *  el pin configurado
*/
static void tarea_adc(void *taskParmPtr)
{
    Datos datosADC;
    ColaADC = xQueueCreate(DATA_QUEUE_SIZE, sizeof(int));
    if (ColaADC == NULL) {
        ESP_LOGE("Cola ADC", "Error al crear la cola ADC");
    }

    int LecturaDelCanalADC = 0;
    adc_oneshot_unit_handle_t Adc2Handle;

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
	adc_oneshot_unit_init_cfg_t Adc2InitConfig = {
		.unit_id = ADC_UNIT_2,
	};
	adc_oneshot_new_unit(&Adc2InitConfig, &Adc2Handle);

	adc_oneshot_chan_cfg_t Adc2Config = {
		.bitwidth = ADC_BITWIDTH_12,
		.atten = ADC_ATTEN_DB_11,
	};
    //Configuro el ADC con una atenuacion de 11DB (150 ~ 2450 mV)
	ESP_ERROR_CHECK(adc_oneshot_config_channel(Adc2Handle, ADC_CHANNEL_7, &Adc2Config));

    // bucle del ADC
    while (true)
    {
        //----- Lectura Canal 7 -----
        adc_oneshot_read(Adc2Handle, ADC_CHANNEL_7, &LecturaDelCanalADC);
        voltaje = LecturaDelCanalADC  * (2.6)/4096;// 2,6 es el voltaje maximo

        if (contador < 10){
            suma = suma + voltaje;
            contador++;
        }
        else if (contador == 10){
            //Cada vez que llega el contador a 10 ejecuta semaforo para actualizar
            //los valores medidos
            if (Semaforo_ADC != NULL){
		        if (xSemaphoreTake(Semaforo_ADC, portMAX_DELAY) == pdTRUE){
                    voltaje_promedio = suma / 10;
                    datosADC.VOL = voltaje_promedio;

                    valor_dB = ((voltaje_promedio *18.65) -50)*(-1);
                    datosADC.POT = valor_dB;

                    // printf("midiendo ADC:  %f\n", valor_dB);
                }
            }
            xSemaphoreGive(Semaforo_ADC);//libero semaforo

            //envio los datos a la cola
            if (xQueueSend(ColaADC, &datosADC, portMAX_DELAY) != pdPASS) {
                ESP_LOGE("Cola", "Error al enviar a la cola ADC");
            }
            contador = 0;
            suma = 0;
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

static void tarea_LCD(){

    I2C_init();

    // Inicializo el LCD
    LCDI2C_init();

    lcd_print("HOLA MUNDO");

    vTaskDelay(5000 / portTICK_PERIOD_MS);

    while (true)
    {
        tiempo_actual = esp_timer_get_time() / 1000;

        if (tiempo_actual - tiempo_inicial >= 3000){
        	estadoModo = estadoModo + 1;

			if (estadoModo == 4){
				estadoModo = 0;
			}

            tiempo_inicial = tiempo_actual;
        }

        BorrarPantalla();
        // actualizardisplay(CONTADOR_1,1);
        // actualizardisplay(CONTADOR_2,2);
        if(ESPACIO_EN_SD){
            lcd_print("Memoria SD Llena");
        }
        else if (MODO_WIFI)
        {
            lcd_print("Modo WIFI");
            actualizardisplay(WIFI,2);
        }
        else{
            switch (estadoModo)
            {
            case GPS:
                actualizardisplay(LATITUD,1);
                actualizardisplay(LONGITUD,2);
                break;
            case POT:
                actualizardisplay(POTENCIA,1);
                actualizardisplay(VOLTAJE,2);
                break;
            case DATA:
                actualizardisplay(SDCARD,1);
                actualizardisplay(WIFI,2);
                break;
            case FECHA:
                actualizardisplay(DIA,1);
                actualizardisplay(HORA,2);
                break;
            default:
                break;
            }
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

static void actualizardisplay(int displayModo, int fila){

    switch (displayModo)
    {
        case LATITUD:
            lcd_gotoxy(1, fila);
            lcd_print("LAT: ");
            lcd_gotoxy(6, fila);

            if (Semaforo_GPS != NULL){
		        if (xSemaphoreTake(Semaforo_GPS, portMAX_DELAY) == pdTRUE){
                    Print_Float_LCD(Latitud, 6/*Cantidad de decimas*/);
                }
            }
            xSemaphoreGive(Semaforo_GPS);//libero semaforo

            break;

        case LONGITUD:
            lcd_gotoxy(1, fila);
            lcd_print("LON: ");
            lcd_gotoxy(6, fila);

            if (Semaforo_GPS != NULL){
		        if (xSemaphoreTake(Semaforo_GPS, portMAX_DELAY) == pdTRUE){
                    Print_Float_LCD(Longitud, 6/*Cantidad de decimas*/);
                }
            }
            xSemaphoreGive(Semaforo_GPS);//libero semaforo

            break;

        case POTENCIA:
            lcd_gotoxy(1, fila);
            lcd_print("Pot:");
            lcd_gotoxy(5, fila);

            if (Semaforo_ADC != NULL){
		        if (xSemaphoreTake(Semaforo_ADC, portMAX_DELAY) == pdTRUE){
                    Print_Float_LCD(valor_dB, 4/*Cantidad de decimas*/);
                }
            }
            xSemaphoreGive(Semaforo_ADC);//libero semaforo

            lcd_gotoxy(14, fila);
            lcd_print("dBm");
            break;

        case VOLTAJE:
            lcd_gotoxy(1, fila);
            lcd_print("Vol:");
            lcd_gotoxy(5, fila);
        
            if (Semaforo_ADC != NULL){
		        if (xSemaphoreTake(Semaforo_ADC, portMAX_DELAY) == pdTRUE){
                    Print_Float_LCD(voltaje_promedio, 4/*Cantidad de decimas*/);
                }
            }
            xSemaphoreGive(Semaforo_ADC);//libero semaforo

            lcd_gotoxy(14, fila);
            lcd_print("V");
            break;

        case SDCARD:
            lcd_gotoxy(1, fila);
            lcd_print("SD:");
            if (SD_CONECTADA){
                lcd_print("conect");
            }
            else{
                lcd_print("uncon.");
            }
            break;

        case WIFI:
            lcd_gotoxy(1, fila);
            lcd_print("WIFI-MQTT:");
            if (MQTT_CONNEECTED){
                lcd_print("conect");
            }
            else{
                lcd_print("uncon.");
            }
            break;

        case DIA:
            lcd_gotoxy(1, fila);
            Print_Float_LCD(Dia,0);
            lcd_print("/");
            Print_Float_LCD(Mes,0);
            break;

        case HORA:
            lcd_gotoxy(1, fila);
            Print_Float_LCD(Hora,0);
            lcd_print(":");
            Print_Float_LCD(Minuto,0);
            lcd_print(":");
            Print_Float_LCD(Segundos,0);
            break;

        case CONTADOR_1:
            lcd_gotoxy(1, fila);
            static uint8_t contador1 = 0; // Esta variable solo se define una vez
            contador1++;
            lcd_print("Contador 1:");
            Print_Float_LCD(contador1,0);
            break;
        case CONTADOR_2:
            lcd_gotoxy(1, fila);
            static uint8_t contador2 = 0; // Esta variable solo se define una vez
            contador2++;
            lcd_print("Contador 2:");
            Print_Float_LCD(contador2,0);
            break;

        default:
            break;
    }
}