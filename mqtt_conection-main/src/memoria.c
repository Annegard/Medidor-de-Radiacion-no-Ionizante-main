
#include "../include/memoria.h"
/*
void iniciarSPI()
{
    esp_err_t ret; // Variable para detectar errores

    //----Configuración del bus
    const spi_bus_config_t configMaster = {
        .miso_io_num = NOT_USED,   //PIN NO USADO YA QUE SOLO NECESITAMOS ENVIAR
        .mosi_io_num = PIN_NUM_MOSI,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = NOT_USED, //pin WP (Write Protect) que es usado como D2 en modo 4-bit
        .quadhd_io_num = NOT_USED, //pin HD (HoID) que es usado como D3 en modo 4-bit
        .max_transfer_sz= MAX_TRANSF_SZ //Tamaño de transferencia máxima, en bytes. 4094 por defecto si es 0.
    };

    ret = spi_bus_initialize(MEMORY, &configMaster, DMA_CHAN); // Inicializa el bus spi
    ESP_ERROR_CHECK(ret); // Verifica que no haya errores
    
    //----Se agrega dispositivo
    spi_device_interface_config_t devConfig ={

        .clock_speed_hz= 1000000,      //Clock 1 MHz
        .mode=0,                      //SPI modo 0
        .spics_io_num=PIN_NUM_CS,    //pin CS
        .cs_ena_posttrans=3,        //Mantiene CS bajo 3 ciclos después de la transacción, para evitar perder datos
        .queue_size=8              //Transacciones pueden estar "en el aire"(en cola) al mismo tiempo.
   };

    ret = spi_bus_add_device(MATRIZ8X8, &devConfig, &spiHandle); // Aloja un dispositivo en el bus SPI
    ESP_ERROR_CHECK(ret); // Verifica que no haya errores
}

// Función para enviar datos
void enviarDatos(spi_device_handle_t spi, const  uint8_t *data, uint8_t len)
{ 
    esp_err_t ret;
    spi_transaction_t t;
    if (len==0) return;                        //No hay datos a enviar
    memset(&t, 0, sizeof(t));                  //Pone a 0 todos los datos de la estructura
    t.length=len*8;                            //Len está en bytes, la transacción está en bits.
    t.tx_buffer= data;                         //Copia la dirección de los datos a enviar
    ret=spi_device_polling_transmit(spi, &t);  //Transmite. Espera a que la transferencia se complete
    assert(ret==ESP_OK);                       //Comprueba errores
}*/