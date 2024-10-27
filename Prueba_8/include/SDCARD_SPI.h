#ifndef SDCARD_SPI_H_
#define SDCARD_SPI_H_
    //========================= Inclusiones =========================
    #include <stdio.h>
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "freertos/queue.h"
    #include "esp_log.h"
    #include "driver/sdspi_host.h"
    #include "driver/spi_common.h"
    #include "sdmmc_cmd.h"
    #include "esp_vfs_fat.h"

    // https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/storage/fatfs.html

    //========================= Definiciones ========================

    #define TAG "SD_CARD_CSV"
    #define MOUNT_POINT "/sdcard"

    #define PIN_NUM_MISO 19
    #define PIN_NUM_MOSI 25
    #define PIN_NUM_CLK  18
    #define PIN_NUM_CS   5

    #define PROCESADORA         0
    #define PROCESADORB         1
    
    /*======================[Prototipos de funciones]============================*/

    void TareaSD();

    typedef struct {
        int HORA;
        int MIN;

        int DIA;
        int MES;

        float LAT;
        float LON;
    } GPSData;

    typedef struct {

        float POT;
        float VOL;
    } ADCData;

    extern bool SD_CONECTADA;
    extern bool ESPACIO_EN_SD;

    extern QueueHandle_t ColaGPS;
    extern QueueHandle_t ColaADC;

    void configurarSDCARD(void);
#endif