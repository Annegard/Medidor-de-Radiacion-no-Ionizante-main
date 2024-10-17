#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "driver/sdspi_host.h"
#include "driver/spi_common.h"
#include "sdmmc_cmd.h"
#include "esp_vfs_fat.h"

#define TAG "SD_CARD_CSV"
#define MOUNT_POINT "/sdcard"

#define PIN_NUM_MISO 19
#define PIN_NUM_MOSI 25
#define PIN_NUM_CLK  18
#define PIN_NUM_CS   5

// Configuración de la tarjeta SD
sdmmc_host_t host = SDSPI_HOST_DEFAULT();
sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
sdmmc_card_t *card;

QueueHandle_t miCola;

typedef struct {
    int dato1;
    float dato2;
} Datos;

void guardarDatosEnCSV(Datos datos) {
    FILE *f = fopen(MOUNT_POINT"/datos.csv", "a");
    if (f == NULL) {
        ESP_LOGE(TAG, "Error al abrir/crear el archivo CSV");
        return;
    }
    fprintf(f, "%d,%.2f\n", datos.dato1, datos.dato2);
    fclose(f);
}

void enviarACola(void *pvParameters) {
    Datos datos;
    while (true) {
        datos.dato1 = rand() % 100;
        datos.dato2 = (rand() % 1000) / 10.0;
        if (xQueueSend(miCola, &datos, portMAX_DELAY) != pdPASS) {
            ESP_LOGE(TAG, "Error al enviar a la cola");
        }
        ESP_LOGI("EnviarCola", "enviando datos: %i, %f a la cola\n", datos.dato1, datos.dato2);
        vTaskDelay(pdMS_TO_TICKS(1000));  // Esperar 1 segundo antes de enviar el siguiente valor
    }
}

void recibirDeCola(void *pvParameters) {
    Datos datosRecibidos;
    while (true) {
        if (xQueueReceive(miCola, &datosRecibidos, portMAX_DELAY) == pdPASS) {
            guardarDatosEnCSV(datosRecibidos);
        }
        ESP_LOGI("RecibirCola", "Reciendo datos: %i, %f a la cola\n", datosRecibidos.dato1, datosRecibidos.dato2);
        vTaskDelay(pdMS_TO_TICKS(1000));  // Esperar 1 segundo antes de enviar el siguiente valor
    }
}


void app_main(void) {
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

    // Crear la cola
    miCola = xQueueCreate(10, sizeof(Datos));
    if (miCola != NULL) {
        // Crear tareas
        xTaskCreate(enviarACola, "Enviar a Cola", 2048, NULL, 1, NULL);
        xTaskCreate(recibirDeCola, "Recibir de Cola", configMINIMAL_STACK_SIZE*6, NULL, 1, NULL);
    } else {
        ESP_LOGE(TAG, "Error al crear la cola");
    }
}
