#include "../include/SDCARD_SPI.h"

static void guardarDatosEnCSV(Datos Datos);
static void verificarEspacioSDCard(uint64_t *total, uint64_t *libre);

// // Configuración de la tarjeta SD
// sdmmc_host_t host = SDSPI_HOST_DEFAULT();
// sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
// sdmmc_card_t *card;

void inicializarSDCARD(void) {
    // Montar la tarjeta SD
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };

    sdmmc_card_t* card;
    const char mount_point[] = MOUNT_POINT;

    ESP_LOGI("SDCARD", "Initializing SD card");

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();

    spi_bus_config_t bus_cfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = PIN_NUM_MISO,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,
    };
    
    host.max_freq_khz = SDMMC_FREQ_PROBING; // Reduce la velocidad para la inicialización

    vTaskDelay(pdMS_TO_TICKS(100));

    esp_err_t ret1 = spi_bus_initialize(host.slot, &bus_cfg, SDSPI_DEFAULT_DMA);
    
    while (ret1!=ESP_OK)
    {
        if (ret1 != ESP_OK) {
            ESP_LOGE("SDCARD", "Falla al inicializar el SPI.");
        }

        ret1 = spi_bus_initialize(host.slot, &bus_cfg, SDSPI_DEFAULT_DMA);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    ESP_LOGI("SDCARD", "SPI inicializado");

    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = PIN_NUM_CS;
    slot_config.host_id = host.slot;

    esp_err_t ret2 = esp_vfs_fat_sdspi_mount(mount_point, &host, &slot_config, &mount_config, &card);

    while (ret1!=ESP_OK)
    {
        if (ret2 != ESP_OK) {
            ESP_LOGE("SDCARD", "Error al montar la tarjeta SD: %s", esp_err_to_name(ret2));
        }

        ret2 = esp_vfs_fat_sdspi_mount(mount_point, &host, &slot_config, &mount_config, &card);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    if(ret2 == ESP_OK){
        if (xSemaphoreTake(Semaforo_SD_Conectada, portMAX_DELAY)) {
            SD_CONECTADA=1;
            ESP_LOGI("SDCARD", "Sistema de archivos montado");

            xSemaphoreGive(Semaforo_SD_Conectada);
        }
    }
}

void TareaSD(){

    printf("Tarea SD creada\n");

    Datos DatosSD;

    while (true)
    {   
        if (xSemaphoreTake(Semaforo_SD_Espacio, portMAX_DELAY)) {
            verificarEspacioSDCard(&total, &libre);
            xSemaphoreGive(Semaforo_SD_Espacio);
        }   

        if(xQueueReceive(ColaDatos, &DatosSD, portMAX_DELAY) != pdPASS) {
            ESP_LOGE("SDCARD", "Error al recibir la cola Datos");
        }
        else{
            guardarDatosEnCSV(DatosSD);
        }
    }                          
}

int contador = 0;

static void guardarDatosEnCSV(Datos Datos) {

 FILE *f = fopen(MOUNT_POINT"/datos.csv", "a");
    
    if (f == NULL) {
        ESP_LOGE("SDCARD", "Error al abrir/crear el archivo CSV");
        
        contador++;

        if (contador==5){
            contador=0;

            if (xSemaphoreTake(Semaforo_SD_Conectada, portMAX_DELAY)) {
                SD_CONECTADA=0;
                ESP_LOGI("SDCARD", "Sistema de archivos montado");
                
                xSemaphoreGive(Semaforo_SD_Conectada);
            }

            inicializarSDCARD();
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
        return;
    }
    // printf("%d,%d,%d,%d,%.2f,%.2f,%.2f,%.2f\n"  ,Datos.MES, Datos.DIA
    //                                             ,Datos.HORA, Datos.MIN
    //                                             ,Datos.LAT, Datos.LON
    //                                             ,Datos.POT, Datos.VOL);
    
    fprintf(f, "%d,%d,%d,%d,%.2f,%.2f,%.2f,%.2f\n"  ,Datos.MES, Datos.DIA
                                                    ,Datos.HORA, Datos.MIN
                                                    ,Datos.LAT, Datos.LON
                                                    ,Datos.POT, Datos.VOL);
    fclose(f);

}

static void verificarEspacioSDCard(uint64_t *total, uint64_t *libre) {
    FATFS* fs;
    DWORD fre_clust, fre_sect, tot_sect;

    // Obtener información sobre el sistema de archivos
    if (f_getfree(MOUNT_POINT, &fre_clust, &fs) != FR_OK) {
        ESP_LOGE(TAG, "Error al obtener la información del sistema de archivos");
        return;
    }

    // Obtener el tamaño total de sectores y los sectores libres
    tot_sect = (fs->n_fatent - 2) * fs->csize;
    fre_sect = fre_clust * fs->csize;

    // Calcular el espacio total y libre en la SDCARD
    *total = tot_sect * fs->ssize;
    *libre = fre_sect * fs->ssize;

    static float relacion;
    
    relacion=(*libre)/(*total);

    if (xSemaphoreTake(Semaforo_SD_Espacio, portMAX_DELAY)) {
        if (relacion<=(0.05)){
            ESPACIO_EN_SD=false;
        }
        else{
            ESPACIO_EN_SD=true;
        }
        xSemaphoreGive(Semaforo_SD_Espacio);
    }

    // ESP_LOGI(TAG, "Espacio total: %llu bytes", total);
    // ESP_LOGI(TAG, "Espacio libre: %llu bytes", libre);

    return;
}