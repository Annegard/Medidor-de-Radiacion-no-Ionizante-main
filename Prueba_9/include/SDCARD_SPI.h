#ifndef SDCARD_SPI_H_
#define SDCARD_SPI_H_

    /*==========================================================================================
        Descripcion:
    
    ==========================================================================================*/
    
    //========================= Inclusiones ==========================
    #include "../include/Global.h"
    #include "esp_vfs_fat.h"

    //=========================== Fuentes ============================
    // https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/storage/fatfs.html
    

    //======================Prototipos de funciones============================
    void TareaSD();
    void inicializarSDCARD();
#endif