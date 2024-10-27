#include "../include/SDCARD_SPI.h"

void TareaSD();
static void guardarDatosEnCSV(ADCData adcData, GPSData gpsData);

// Configuración de la tarjeta SD
sdmmc_host_t host = SDSPI_HOST_DEFAULT();
sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
sdmmc_card_t *card;

void configurarSDCARD(void) {
    // Montar la tarjeta SD
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };

    sdmmc_card_t* card;
    const char mount_point[] = MOUNT_POINT;

    ESP_LOGI(TAG, "Initializing SD card");

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
    if (ret1 != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize bus.");
        return;
    }

    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = PIN_NUM_CS;
    slot_config.host_id = host.slot;

    esp_err_t ret2 = esp_vfs_fat_sdspi_mount(mount_point, &host, &slot_config, &mount_config, &card);
    if (ret2 != ESP_OK) {
        ESP_LOGE(TAG, "Error al montar la tarjeta SD: %s", esp_err_to_name(ret2));
        return;
    }
    ESP_LOGI(TAG, "Sistema de archivos montado");
}

void TareaSD(){

    printf("TareaSD creada\n");

    GPSData DatosGPSRecibidos;
    ADCData DatosADCRecibidos;

    while (true){
        // printf("Hola desde el SD\n");
        if(xQueueReceive(ColaGPS, &DatosGPSRecibidos, portMAX_DELAY) != pdPASS) {
            ESP_LOGE(TAG, "Error al recibir la cola GPS");
        }
        if(xQueueReceive(ColaADC, &DatosADCRecibidos, portMAX_DELAY) != pdPASS) {
            ESP_LOGE(TAG, "Error al recibir la cola ADC");
        }
        
        // printf("%f,%.6f,%.6f\n", DatosADCRecibidos.POT, DatosGPSRecibidos.LAT, DatosGPSRecibidos.LON);

        guardarDatosEnCSV(DatosADCRecibidos, DatosGPSRecibidos);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
                                                   
}

static void guardarDatosEnCSV(ADCData adcData, GPSData gpsData) {

    FILE *f = fopen(MOUNT_POINT"/datos.csv", "a");
    
    if (f == NULL) {
        ESP_LOGE(TAG, "Error al abrir/crear el archivo CSV");
        return;
    }

    printf("%d,%d,%d,%d,%.2f,%.2f,%.2f,%.2f\n"  ,gpsData.MES, gpsData.DIA
                                                    ,gpsData.HORA, gpsData.MIN
                                                    ,gpsData.LAT, gpsData.LON
                                                    ,adcData.POT, adcData.VOL);
    
    fprintf(f, "%d,%d,%d,%d,%.2f,%.2f,%.2f,%.2f\n"  ,gpsData.MES, gpsData.DIA
                                                    ,gpsData.HORA, gpsData.MIN
                                                    ,gpsData.LAT, gpsData.LON
                                                    ,adcData.POT, adcData.VOL);
    fclose(f);

}
