#ifndef GLOBAL_H_
#define GLOBAL_H_
    /*==========================================================================================
        Descripcion:
    
    ==========================================================================================*/

    //========================= Inclusiones ==========================
    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>
    #include <stdint.h>
    #include <stddef.h>

    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "freertos/semphr.h"    
    #include "freertos/queue.h"
    #include "freertos/event_groups.h"

    #include "esp_system.h"
    #include "esp_log.h"
    #include "sdmmc_cmd.h"
    #include "esp_vfs_fat.h"
    #include "esp_timer.h"

    #include "driver/uart.h"
    #include "driver/gpio.h"    //pines generales
    #include "driver/sdspi_host.h"
    #include "driver/spi_common.h"

    #include "esp_adc/adc_oneshot.h"     //ADC

    //=========================ESTRUCTURAS========================
    typedef struct {
        uint8_t HORA;
        int MIN;
        int SEG;

        int DIA;
        int MES;

        float LAT;
        float LON;

        float POT;
        float VOL;
    } Datos;

    typedef enum{
        ALTO,
        BAJO,
        DESCENDENTE,
        ASCENDENTE
    } estadoBoton_t;

    typedef enum{
        LATITUD,
        LONGITUD,
        POTENCIA,
        VOLTAJE,
        SDCARD_TOTAL,
        SDCARD_LIBRE,
        WIFI,
        MQTT,
        DIA,
        HORA,
        CONTADOR_1,
        CONTADOR_2
    } displayMODO_t;

    typedef enum{
        GPS,
        POT,
        DATA,
        FECHA
    } estadoMODO_t;

    ////========================= DEFINICIONES ======================////

    //==========================CPU y MAIN=========================
    #define PROCESADORA         0
    #define PROCESADORB         1

    #define PRIORIDAD_LCD       4
    #define PRIORIDAD_ADC       3
    #define PRIORIDAD_GPS       2
    #define PRIORIDAD_PULSADOR  1
    #define PRIORIDAD_SD        0
    #define PRIORIDAD_COLAS     4

    //==========================PULSADOR==========================
    #define BOTON_A GPIO_NUM_36
    // #define BOTON_B GPIO_NUM_26

    //=============================SD=============================
    #define TAG "SD_CARD_CSV"
    #define MOUNT_POINT "/sdcard"

    //============================SPI=============================
    #define PIN_NUM_MISO (GPIO_NUM_19)
    #define PIN_NUM_MOSI (GPIO_NUM_25)
    #define PIN_NUM_CLK  (GPIO_NUM_18)
    #define PIN_NUM_CS   (GPIO_NUM_5)

    //============================I2C=============================
    #define I2C_MASTER_SCL_IO   (GPIO_NUM_21)                /*!< gpio number for I2C master clock */
    #define I2C_MASTER_SDA_IO   (GPIO_NUM_22)                /*!< gpio number for I2C master data  */
    #define I2C_MASTER_FREQ_HZ  50000            /*!< I2C master clock frequency */
    #define ESP_SLAVE_ADDR_LCD  0x27             /*!< ESP32 slave address, you can set any 7bit value */

    //============================GPS=============================
    #define TXD_PIN     (GPIO_NUM_1)
    #define RXD_PIN     (GPIO_NUM_3)
    #define TXD_PIN2    (GPIO_NUM_16)
    #define RXD_PIN2    (GPIO_NUM_17)
    
    //============================WIFI============================
    #define WIFI_SSID   "alejocasas"
    #define WIFI_PASS   "Cartago97"
    #define MQTT_URI    "mqtt://broker.emqx.io"
    #define MQTT_PORT   1883
    #define NAME        "SENSOR1"

    //========================== VARIABLES ============================
    extern bool MQTT_CONNEECTED;
    extern bool SD_CONECTADA;
    extern bool ESPACIO_EN_SD;
    extern bool MODO_WIFI;
    extern bool ESTADO_WIFI;

    extern uint64_t total;
    extern uint64_t libre;
    //========================== SEMAFOROS ============================
    extern SemaphoreHandle_t Semaforo_MQTT;
    extern SemaphoreHandle_t Semaforo_MODO_WIFI;
    extern SemaphoreHandle_t Semaforo_ESTADO_WIFI;
    extern SemaphoreHandle_t Semaforo_SD_Conectada;
    extern SemaphoreHandle_t Semaforo_SD_Espacio;

    //============================ COLAS ============================
    extern QueueHandle_t ColaGPS;
    extern QueueHandle_t ColaPulsador;
    extern QueueHandle_t ColaADC;
    extern QueueHandle_t ColaDatos;


#endif