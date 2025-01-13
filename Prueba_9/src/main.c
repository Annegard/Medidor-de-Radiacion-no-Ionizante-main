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

//============================= Inclusiones ================================
// librerias propias
#include "../include/Global.h"
#include "../include/LCDI2C.h"
#include "../include/pulsador.h"
#include "../include/GPS_UART.h"
#include "../include/Wifi.h"
#include "../include/SDCARD_SPI.h"

// //=========================== Definiciones ================================
// DEFINICIONES
//#define T 10 / portTICK_PERIOD_MS // tiempo en ms de espera para volver a ingresar a la función de actualizar semaforo
#define Promedio 10
#define DATA_QUEUE_SIZE 10

// //=========================== Variables ================================
uint64_t total;
uint64_t libre;

static int64_t tiempo_actual = 0;
static int64_t tiempo_inicial = 0;   

bool MQTT_CONNEECTED = false;
bool SD_CONECTADA = false;
bool ESPACIO_EN_SD = false;
bool MODO_WIFI = false;
bool ESTADO_WIFI = false;
bool HabilitaActualizarDisplay=true;

char password[MAX_PASSWORD_LEN];

redesWIFI_t REDES;

//====================== Prototipos de funciones ============================
esp_err_t CrearTareaADC(void);
esp_err_t CrearTareaPulsador(gpio_int_type_t pulsador);
esp_err_t CrearTareaSD_SPI();
esp_err_t CrearTareaLCD(void);
esp_err_t CrearTareaGPS_UART(void);
esp_err_t CrearTareaWIFI_MQTT(int refrezco);
esp_err_t CrearTareaColas();

static void BorrarTareaWIFI_MQTT();
static void tarea_ADC(void *taskParmPtr);
// static void tarea_Pulsador(void *taskParmPtr);
static void tarea_LCD();
static void tarea_WIFI();
static void actualizardisplay(displayMODO_t displayModo, int fila, Datos Datos);
static void TareaColas();
void init_buttons();
bool is_button_pressed(gpio_num_t button);
void enter_password(char *password, size_t max_len);

uint8_t ActualizarLCDporTiempo(estadoMODO_t estadoModoLCD);

void navigate_networks(int num_networks);
void enter_password(char *password, size_t max_len);
void actualizarPulsadores( gpio_int_type_t pulsador1, gpio_int_type_t pulsador2 );
//============================= Colas ============================
QueueHandle_t ColaADC;
QueueHandle_t ColaGPS;
QueueHandle_t ColaDatos;
QueueHandle_t ColaPulsador;

//============================= Semaforos ============================
SemaphoreHandle_t Semaforo_MQTT;
SemaphoreHandle_t Semaforo_WIFI_MODE;
SemaphoreHandle_t Semaforo_ESTADO_WIFI;
SemaphoreHandle_t Semaforo_SD_Conectada;
SemaphoreHandle_t Semaforo_SD_Espacio;

//============================= Punteros ============================
TaskHandle_t punteroTareaWIFI;

// //=========================== Función principal ================================
void app_main()
{
    ColaGPS = xQueueCreate(DATA_QUEUE_SIZE, sizeof(Datos));
    ColaADC = xQueueCreate(DATA_QUEUE_SIZE, sizeof(Datos));
    ColaDatos = xQueueCreate(DATA_QUEUE_SIZE, sizeof(Datos));
    ColaPulsador = xQueueCreate(DATA_QUEUE_SIZE, sizeof(estadoMODO_t));

    Semaforo_MQTT = xSemaphoreCreateMutex();
    Semaforo_ESTADO_WIFI = xSemaphoreCreateMutex();
    Semaforo_WIFI_MODE = xSemaphoreCreateMutex();
    Semaforo_SD_Conectada = xSemaphoreCreateMutex();
    Semaforo_SD_Espacio = xSemaphoreCreateMutex();

    //Creo Tareas en el procesador B
    CrearTareaColas();
    CrearTareaLCD();
    CrearTareaGPS_UART();
    CrearTareaADC();
    // CrearTareaPulsador(BOTON_A);//Recibe de parametro el boton a leer
    init_buttons();
    //CrearTareaSD_SPI();//Recibe de parametro cada cuantos segundos guarda los datos

    //Creo Tareas en el procesador A
    CrearTareaWIFI_MQTT(1);//Recibe de parametro cada cuantos segundos guarda los datos
}

esp_err_t CrearTareaColas(){
        BaseType_t res = xTaskCreatePinnedToCore(
        TareaColas,                    // Funcion de la tarea a ejecutar
        "TareaColas",                   // Nombre de la tarea como String amigable para el usuario
        configMINIMAL_STACK_SIZE*2, // Cantidad de stack de la tarea
        NULL,                         // Parametros de tarea
        tskIDLE_PRIORITY+PRIORIDAD_COLAS,         // Prioridad de la tarea -> Queremos que este un nivel encima de IDLE
        NULL,                         // Puntero a la tarea creada en el sistema
        PROCESADORB                   // Procesador donde se ejecuta
    );
    // Gestion de errores
    if (res == pdFAIL)
    {
        printf("Error al crear la tarea Colas.\r\n");
        while (true)
            ; // si no pudo crear la tarea queda en un bucle infinito
    }
    return ESP_OK;
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
        printf("Error al crear la tarea LCD.\r\n");
        while (true)
            ; // si no pudo crear la tarea queda en un bucle infinito
    }
    return ESP_OK;
}

/*esp_err_t CrearTareaPulsador(gpio_int_type_t pulsador){

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
}*/

esp_err_t CrearTareaADC(void){
    /////TAREA ADC////
    BaseType_t res = xTaskCreatePinnedToCore(
        tarea_ADC,                    // Funcion de la tarea a ejecutar
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

    inicializarSDCARD();

    vTaskDelay(pdMS_TO_TICKS(1000));

    BaseType_t res5 = xTaskCreatePinnedToCore(
            TareaSD,                     	// Funcion de la tarea a ejecutar
            "GuardarDatos",   	                // Nombre de la tarea como String amigable para el usuario
            configMINIMAL_STACK_SIZE*4, 		// Cantidad de stack de la tarea
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
        2048, 		    // Cantidad de stack de la tarea
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

static void TareaColas(){
    
    printf("Tarea COLAS creada\n");

    Datos DatosGPSRecibidos;
    Datos DatosADCRecibidos;
    Datos Datos_Combinados;

    //Predefino
    Datos_Combinados.MES=0;
    Datos_Combinados.DIA=0;
    Datos_Combinados.HORA=0;
    Datos_Combinados.MIN=0;
    Datos_Combinados.LAT=0;
    Datos_Combinados.LON=0;
    Datos_Combinados.VOL=0;
    Datos_Combinados.POT=0;

    while (true){
        if(xQueueReceive(ColaGPS, &DatosGPSRecibidos, 100/portTICK_PERIOD_MS) != pdPASS) {
            //ESP_LOGE("TareaColas", "Error al recibir la cola GPS");
        }
        else{
            Datos_Combinados.MES=DatosGPSRecibidos.MES;
            Datos_Combinados.DIA=DatosGPSRecibidos.DIA;
            Datos_Combinados.HORA=DatosGPSRecibidos.HORA;
            Datos_Combinados.MIN=DatosGPSRecibidos.MIN;
            Datos_Combinados.LAT=DatosGPSRecibidos.LAT;
            Datos_Combinados.LON=DatosGPSRecibidos.LON;
        }
        
        if(xQueueReceive(ColaADC, &DatosADCRecibidos, 100/portTICK_PERIOD_MS) != pdPASS) {
            //ESP_LOGE("TareaColas", "Error al recibir la cola ADC");
        }
        else{
            Datos_Combinados.VOL=DatosADCRecibidos.VOL;
            Datos_Combinados.POT=DatosADCRecibidos.POT;
        }

        if (xQueueSend(ColaDatos, &Datos_Combinados, 100/portTICK_PERIOD_MS) != pdPASS) {
            //ESP_LOGE("TareaColas", "Error al enviar a la cola Datos");
        }

        // vTaskDelay(30 / portTICK_PERIOD_MS);
    }
}

static void tarea_WIFI(){

    esp_err_t ret = nvs_flash_init();
    uint16_t contador=0;

    bool AuxiliarWIFIstop = false;
    bool AuxiliarWIFIinit = true;
    bool auxiliarBOTONWIFI = false;
    bool auxiliarMODOWIFI = false;

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    // BorrarTareaWIFI_MQTT();

    while (true)
    {   
        if (auxiliarMODOWIFI){
            
            if (AuxiliarWIFIinit){
                if (xSemaphoreTake(Semaforo_WIFI_MODE, portMAX_DELAY)){
                    HabilitaActualizarDisplay = false;
                    xSemaphoreGive(Semaforo_WIFI_MODE);
                }

                AuxiliarWIFIinit = false;
                AuxiliarWIFIstop = true;

                REDES = wifi_scan_task();

                navigate_networks(REDES.num_networks);

                vTaskDelay(2000 / portTICK_PERIOD_MS);

                if (xSemaphoreTake(Semaforo_WIFI_MODE, portMAX_DELAY)){
                    HabilitaActualizarDisplay = true;
                    MODO_WIFI=true;
                    xSemaphoreGive(Semaforo_WIFI_MODE);
                }

            }
            else{
                Publisher_Task();
            }
        }
        else{
            if (AuxiliarWIFIstop) {
                MODO_WIFI=false;
                AuxiliarWIFIinit = true;
                AuxiliarWIFIstop = false;

                BorrarTareaWIFI_MQTT();
            }
        }

        if (is_button_pressed(BUTTON_NEXT) && is_button_pressed(BUTTON_SELECT)) {
            contador++;
            ESP_LOGI("tarea_WIFI", "MODO_WIFI_ON");
            if (contador >= (30)) {  // 3 segundos
                if(auxiliarBOTONWIFI){
                    auxiliarMODOWIFI=true;
                    auxiliarBOTONWIFI=false;
                    //MODO_WIFI=!MODO_WIFI;
                }
            }
        }
        else {
            auxiliarBOTONWIFI=true;
            contador = 0;  // Reinicia el temporizador si los botones se sueltan
        }
        vTaskDelay((100) / portTICK_PERIOD_MS);
    }
}

static void tarea_ADC(void *taskParmPtr)
{
    // Tarea "tarea_ADC":
    // Tarea que congifura el ADC2 del microcontrolador y ejecuta un bucle infinito donde lee constantemente
    // el pin configurado

    printf("TareaADC creada\n");

    Datos datosADC;

    if (ColaADC == NULL) {
        ESP_LOGE("Cola ADC", "Error al crear la cola ADC");
    }

    int LecturaDelCanalADC = 0;
    adc_oneshot_unit_handle_t Adc2Handle;

    uint8_t contador = 0;
    float suma = 0;
    float voltaje = 0;

    float voltaje_promedio = 0;
    float valor_dB = 0;

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
            voltaje_promedio = suma / 10;
            datosADC.VOL = voltaje_promedio;

            valor_dB = ((voltaje_promedio *18.65) -50)*(-1);
            datosADC.POT = valor_dB;

            // ESP_LOGE("tarea_ADC","midiendo ADC:  %f\n", valor_dB);

            //envio los datos a la cola
            if (xQueueSend(ColaADC, &datosADC, portMAX_DELAY) != pdPASS) {
                ESP_LOGE("tarea_ADC", "Error al enviar a la cola ADC");
            }

            contador = 0;
            suma = 0;
        }
        

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

static void tarea_LCD(){

    Datos Datos_LCD;
    estadoMODO_t estadoModoLCD;

    
    bool AuxModoWIFI=false;

    I2C_init();

    // Inicializo el LCD
    LCDI2C_init();

    lcd_print("HOLA MUNDO");

    vTaskDelay(3000 / portTICK_PERIOD_MS);

    // tarea_WIFI();

    //Limpio la pantalla antes de todo
    BorrarPantalla();
    while (true)
    {   
        if (HabilitaActualizarDisplay){
            if(xQueueReceive(ColaDatos, &Datos_LCD, 100/portTICK_PERIOD_MS) != pdPASS) {
                ESP_LOGE("tarea_LCD", "Error al recibir la ColaDatos");
            }

            //Actualizar estadoModoLCD por pulsador usando Colas
            if(xQueueReceive(ColaPulsador, &estadoModoLCD, 100/portTICK_PERIOD_MS) != pdPASS) {
                //ESP_LOGE(TAG, "Error al recibir la cola Datos_LCD");
            }

            //Actualizar estadoModoLCD por tiempo
            estadoModoLCD=ActualizarLCDporTiempo(estadoModoLCD);

            //VERIFICA CONECCION CON SD
            // if (xSemaphoreTake(Semaforo_SD_Conectada, portMAX_DELAY)) {
            //     BorrarPantalla();

            //     if (SD_CONECTADA){
            //         HabilitaActualizarDisplay = true;

            //         //VERIFICA ESPACIO EN SD
            //         if (xSemaphoreTake(Semaforo_SD_Espacio, portMAX_DELAY)) {
            //             if (ESPACIO_EN_SD){
            //                 HabilitaActualizarDisplay = true;
            //             }
            //             else{
            //                 HabilitaActualizarDisplay = false;
                            
            //                 lcd_gotoxy(1, 1);
            //                 lcd_print("Memoria SD");
            //                 lcd_gotoxy(1, 2);
            //                 lcd_print("Llena");
            //                 vTaskDelay(pdMS_TO_TICKS(3000));
            //             }
            //             xSemaphoreGive(Semaforo_SD_Espacio);
            //         }
            //     }
            //     else{
            //         HabilitaActualizarDisplay = false;
                    
            //         lcd_gotoxy(1, 1);
            //         lcd_print("Memoria SD");
            //         lcd_gotoxy(1, 2);
            //         lcd_print("desconectada");
            //         vTaskDelay(pdMS_TO_TICKS(3000));
            //     }
            //     xSemaphoreGive(Semaforo_SD_Conectada);
            // }

            //VERIFICA MODO WIFI
            if (xSemaphoreTake(Semaforo_WIFI_MODE, portMAX_DELAY)){
                AuxModoWIFI = MODO_WIFI;
                xSemaphoreGive(Semaforo_WIFI_MODE);
            }//hago esto sino me quedaba un semaforo anidado bloqueando el pulsador

            if (AuxModoWIFI){
                actualizardisplay(WIFI,1, Datos_LCD);
                actualizardisplay(MQTT,2, Datos_LCD);
            }
            else{
                // actualizardisplay(CONTADOR_1,1);
                // actualizardisplay(CONTADOR_2,2);
                // estadoModoLCD=ActualizarLCDporTiempo(estadoModoLCD);

                switch (estadoModoLCD)
                {
                case GPS:
                    actualizardisplay(LATITUD,1, Datos_LCD);
                    actualizardisplay(LONGITUD,2, Datos_LCD);
                    break;
                case POT:
                    actualizardisplay(POTENCIA,1, Datos_LCD);
                    actualizardisplay(VOLTAJE,2, Datos_LCD);
                    break;
                case DATA:
                    actualizardisplay(SDCARD_TOTAL,1, Datos_LCD);
                    actualizardisplay(SDCARD_LIBRE,2, Datos_LCD);
                    break;
                case FECHA:
                    actualizardisplay(DIA,1, Datos_LCD);
                    actualizardisplay(HORA,2, Datos_LCD);
                    break;
                default:
                    break;
                }
            }
        }
        
        vTaskDelay(100 / portTICK_PERIOD_MS);//reduzco carga al procesador
    }
}

/*static void tarea_Pulsador(void* parametros){

    gpio_int_type_t boton = (gpio_int_type_t) parametros; // Convertir el puntero a int

    inicializarBoton(boton);

    //Bucle principal de la tarea
    while (true){

        //Lee el pulsador, si este detecta una pulsación cambio el estado de "estadoModo"
        actualizarBoton(boton);

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}*/

uint8_t ActualizarLCDporTiempo(estadoMODO_t estadoModoLCD){

    tiempo_actual = esp_timer_get_time() / 1000;

    if (tiempo_actual - tiempo_inicial >= 3000){
        BorrarPantalla();
        estadoModoLCD = estadoModoLCD + 1;

        if (estadoModoLCD == 4){
            estadoModoLCD = 0;
        }

        tiempo_inicial = tiempo_actual;
    }

    return estadoModoLCD;
}

static void actualizardisplay(displayMODO_t displayModo, int fila, Datos Datos){

    switch (displayModo)
    {
        case LATITUD:
            lcd_gotoxy(1, fila);
            lcd_print("LAT: ");
            lcd_gotoxy(6, fila);

            Print_Float_LCD(Datos.LAT, 6/*Cantidad de decimas*/);

        break;

        case LONGITUD:
            lcd_gotoxy(1, fila);
            lcd_print("LON: ");
            lcd_gotoxy(6, fila);

            Print_Float_LCD(Datos.LON, 6/*Cantidad de decimas*/);

        break;

        case POTENCIA:
            lcd_gotoxy(1, fila);
            lcd_print("Pot:");
            lcd_gotoxy(5, fila);

            Print_Float_LCD(Datos.POT, 4/*Cantidad de decimas*/);

            lcd_gotoxy(14, fila);
            lcd_print("dBm");
        break;

        case VOLTAJE:
            lcd_gotoxy(1, fila);
            lcd_print("Vol:");
            lcd_gotoxy(5, fila);
        
            Print_Float_LCD(Datos.VOL, 4/*Cantidad de decimas*/);

            lcd_gotoxy(14, fila);
            lcd_print("V");
        break;

        case SDCARD_TOTAL:
            lcd_gotoxy(1, fila);
            lcd_print("SD Total:");

            if (xSemaphoreTake(Semaforo_SD_Espacio, portMAX_DELAY)) {
                Print_Float_LCD(total, 7/*Cantidad de decimas*/);
                xSemaphoreGive(Semaforo_SD_Espacio);
            }
        break;

        case SDCARD_LIBRE:
            lcd_gotoxy(1, fila);
            lcd_print("SD Libre:");

            if (xSemaphoreTake(Semaforo_SD_Espacio, portMAX_DELAY)) {
                Print_Float_LCD(total, 7/*Cantidad de decimas*/);
                xSemaphoreGive(Semaforo_SD_Espacio);
            }
        break;

        case WIFI:
            lcd_gotoxy(1, fila);
            lcd_print("WIFI:");

            if (xSemaphoreTake(Semaforo_ESTADO_WIFI, portMAX_DELAY)) {
                if (ESTADO_WIFI){
                    lcd_print("conect");
                }
                else{
                    lcd_print("uncon.");
                }
                xSemaphoreGive(Semaforo_ESTADO_WIFI);
            }
        break;

        case MQTT:
            lcd_gotoxy(1, fila);
            lcd_print("MQTT:");

            if (xSemaphoreTake(Semaforo_MQTT, portMAX_DELAY)) {
                if (MQTT_CONNEECTED){
                    lcd_print("conect");
                }
                else{
                    lcd_print("uncon.");
                }
                xSemaphoreGive(Semaforo_MQTT);
            }

        break;

        case DIA:
            lcd_gotoxy(1, fila);
            Print_Float_LCD(Datos.DIA,0);
            lcd_print("/");
            Print_Float_LCD(Datos.MES,0);
        break;

        case HORA:
            lcd_gotoxy(1, fila);
            Print_Float_LCD(Datos.HORA,0);
            lcd_print(":");
            Print_Float_LCD(Datos.MIN,0);
            lcd_print(":");
            Print_Float_LCD(Datos.SEG,0);
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
            lcd_print("Error LCD");
        break;
    }
}

void init_buttons() {
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL << BUTTON_NEXT) | (1ULL << BUTTON_SELECT),
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE
    };
    gpio_config(&io_conf);
}

bool is_button_pressed(gpio_num_t button) {
    return gpio_get_level(button) == 0;
}

void navigate_networks(int num_networks) {
    int current_index = 0, index_auxiliar=1;
    char ssid_string[33]; // Máximo 32 caracteres para SSID + 1 para terminador nulo
    //int contador=0;
    char dato;
    ESP_LOGI("Total de Redes", "%d", num_networks);

    ESP_LOGI("WIFI_TAG", "SSID: %s", REDES.ap_records[current_index].ssid);

    BorrarPantalla();
    lcd_print("MODO WIFI");
    vTaskDelay(pdMS_TO_TICKS(2000));

    while (true)
    {   
        if (current_index != index_auxiliar){

            index_auxiliar=current_index;

            ESP_LOGI("Indice", "%d", current_index);

            ESP_LOGI("WIFI_TAG", "SSID: %s", REDES.ap_records[current_index].ssid);
                
            BorrarPantalla();
            lcd_print("Network:");
                
            lcd_gotoxy(0, 2);

            unsigned int i = 0;
            while (REDES.ap_records[current_index].ssid[i] !=0) //Recorre todo el arreglo
            {    
                //ESP_LOGI("WIFI_TAG", "SSID: %c", (char)REDES.ap_records[current_index].ssid[i]);

                dato= (char)REDES.ap_records[current_index].ssid[i]; //Imprime caracter por caracter

                lcd_print(&dato);
                ssid_string[i] = dato;

                i++;

                // vTaskDelay(pdMS_TO_TICKS(10));

                ssid_string[i] = dato;
            }
            ssid_string[i] = '\0';  // Agregar el terminador nulo al final del string
            // contador=0;
        }

        if (is_button_pressed(BUTTON_NEXT)) {

            if (current_index != (num_networks-1)){
                current_index = current_index + 1;
                
            }
            else{
                current_index=0;
            }
        }

        if (is_button_pressed(BUTTON_SELECT)) {
            for (int i = 0; i < REDES.num_networks; i++) {
                ESP_LOGI("NETWORK_SELECT", "SSID: %s", REDES.ap_records[i].ssid);
            }

            ESP_LOGI("PASSWORD", "Entering password for network...");
            enter_password(password, MAX_PASSWORD_LEN);
            
            // Ahora puedes usar `ssid_string` como un string en C
            ESP_LOGI("WIFI_TAG", "SSID Guardada: %s", ssid_string);
            ESP_LOGI("PASSWORD", "Password entered: %s", password);

            wifi_init(ssid_string, password);

            break;
        }

        vTaskDelay(pdMS_TO_TICKS(100));
        // contador++;
    }
}

void enter_password(char *password, size_t max_len) {
    const char characters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    int char_index = 0;  // Índice del carácter seleccionado
    int pass_index = 0;  // Índice en la contraseña
    char current_char=characters[0];
    bool refrezcarPantalla=true;
    bool CLEAR_REALIZADO=false, BACK_REALIZADO=false, BUCLE_REALIZADO=false;

    bool BOTON_NEXT, BOTON_SELECT, BOTON_NEXT_previo = false, BOTON_SELECT_previo = false, SELECT_HAB = false;
    int contador = 0, hold= 0, contador_NEXT=0, contador_SELECT=0;

    // Inicializa la contraseña vacía
    memset(password, 0, max_len);

    while (true) {
        
        BOTON_NEXT = is_button_pressed(BUTTON_NEXT);
        BOTON_SELECT = is_button_pressed(BUTTON_SELECT);

        current_char = characters[char_index];  // Obtén el carácter actual

        if (refrezcarPantalla){
            // Muestra la contraseña y el carácter seleccionado en el LCD
            BorrarPantalla();
            lcd_gotoxy(1, 1);
            lcd_print("CARACTER:");
            lcd_print_char(current_char);
            lcd_gotoxy(1, 2);  // Línea 2
            lcd_print(password);
            refrezcarPantalla= false;
        }

        // Detecta flanco descendente (soltar botón)
        if (BOTON_NEXT_previo == true && BOTON_NEXT == false) {
            
            refrezcarPantalla = true;
            
            if (!BACK_REALIZADO){
                if (!is_button_pressed(BUTTON_SELECT)) {  // Si botón 2 no está presionado, es "NEXT"
                    if(!BUCLE_REALIZADO){
                        ESP_LOGI("PASSWORD:", "NEXT");
                        char_index = (char_index + 1) % strlen(characters);  // Avanza al siguiente carácter
                    }
                    else
                    {
                        BUCLE_REALIZADO=false;
                    }
                    
                }

                else {  // Si botón 2 está presionado, es "CLEAR"
                    ESP_LOGI("PASSWORD:", "CLEAR");
                    CLEAR_REALIZADO=true;
                    memset(password, 0, max_len);
                    pass_index = 0;
                }
            }
            else{
                BACK_REALIZADO = false;
            }
        }

        // Detecta flanco descendente (soltar botón)
        if (BOTON_SELECT_previo == true && BOTON_SELECT == false) {
            
            refrezcarPantalla = true;

            if (!CLEAR_REALIZADO){
                if (!is_button_pressed(BUTTON_NEXT)&&SELECT_HAB) {  // Si botón 1 no está presionado, es "SELECT"
                    ESP_LOGI("PASSWORD:", "SELECT");
                    if (pass_index < max_len - 1) {
                        password[pass_index++] = current_char;  // Agrega el carácter a la contraseña
                        password[pass_index] = '\0';           // Asegura terminación
                    }
                }
                
                else {  // Si botón 1 está presionado, es "BACK"
                    ESP_LOGI("PASSWORD:", "BACK");
                    BACK_REALIZADO=true;
                    char_index = (char_index - 1 + strlen(characters)) % strlen(characters);  // Retrocede
                }
            }
            else{
                CLEAR_REALIZADO = false;
            }
        }


        if (is_button_pressed(BUTTON_NEXT) && !is_button_pressed(BUTTON_SELECT))
        {
            contador_NEXT++;
            ESP_LOGI("contador_NEXT", "%i", contador_NEXT);
        }
        else
        {
            contador_NEXT=0;
        }
        
        if (contador_NEXT>=10)
        {  
            BUCLE_REALIZADO=true;
            while (is_button_pressed(BUTTON_NEXT) && !is_button_pressed(BUTTON_SELECT))
            {
                if (!BACK_REALIZADO){
                    if (hold>=5000)
                    {
                        hold=0;

                        ESP_LOGI("BUCLE WHILE", "NEXT");
                        char_index = (char_index + 1) % strlen(characters);  // Avanza al siguiente carácter
                        current_char = characters[char_index];  // Obtén el carácter actual

                        BorrarPantalla();
                        lcd_gotoxy(1, 1);
                        lcd_print("CARACTER:");
                        lcd_print_char(current_char);
                        lcd_gotoxy(1, 2);  // Línea 2
                        lcd_print(password);
                    }
                    vTaskDelay(pdMS_TO_TICKS(5));
                    hold++;
                }
            }
        }

        // Detecta si ambos botones están presionados durante más de 3 segundos ("SAVE")
        if (BOTON_SELECT == true && BOTON_NEXT == true) {
            contador++;
            if (contador >= (3000 / 50)) {  // 3 segundos
                BorrarPantalla();
                lcd_gotoxy(1, 1);
                lcd_print("SAVE PASSWORD");
                lcd_gotoxy(1, 2);
                lcd_print(password);
                ESP_LOGI("PASSWORD", "Contraseña guardada: %s", password);
                
                break;  // Salir de la función
            }
        } 
        else {
            contador = 0;  // Reinicia el temporizador si los botones se sueltan
        }

        // Actualiza los estados anteriores
        BOTON_NEXT_previo = BOTON_NEXT;
        BOTON_SELECT_previo = BOTON_SELECT;

        vTaskDelay(pdMS_TO_TICKS(50));  // Reduce la carga del microcontrolador

        contador_SELECT++;
        
        if (contador_SELECT>10)
        {
            SELECT_HAB = true;
        }
        
    }

    ESP_LOGI("PASSWORD", "Final password: %s", password);
}

static void BorrarTareaWIFI_MQTT(){

    //vTaskDelete(punteroTareaWIFI);

    ESP_LOGI("WIFI", "Deteniendo el Wi-Fi...");
    
    mqtt_app_stop();

    // ESP_ERROR_CHECK(esp_wifi_stop());

    wifi_mode_t mode;
    esp_err_t err = esp_wifi_get_mode(&mode);


    if (err == ESP_OK) {
        err = esp_wifi_stop();
        if (err != ESP_OK) {
            printf("Error al detener WiFi: %s\n", esp_err_to_name(err));
        }
        else{
            if (err == ESP_ERR_WIFI_NOT_INIT) {
                printf("WiFi no está inicializado, no se puede desinicializar.\n");
                return;
            }

            err = esp_wifi_deinit();

            if (err != ESP_OK) {
                printf("Error al desinicializar WiFi: %s\n", esp_err_to_name(err));
            }
        }
    }
    else {
        printf("WiFi no está inicializado: %s\n", esp_err_to_name(err));
    }

    ESP_LOGI("WIFI", "Wi-Fi deshabilitado");
}