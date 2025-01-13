#ifndef GPS_UART_H_
#define GPS_UART_H_
    /*==========================================================================================
        Descripcion:
    
    ==========================================================================================*/
    
    //========================= Inclusiones ==========================
    #include "../include/Global.h"
    #include "../include/lwgps.h"

    //=========================== Fuentes ============================
    // https://www.sigmaelectronica.net/trama-gps/
    // https://github.com/Tinyu-Zhao/TinyGPSPlus-ESP32/tree/master/src
    // https://github.com/Tinyu-Zhao/TinyGPSPlus-ESP32/tree/master/src


    //==================[Prototipos de funciones]=====================
    void init_Uart2();//9600 baudios a GPS
    void rx_task();

#endif