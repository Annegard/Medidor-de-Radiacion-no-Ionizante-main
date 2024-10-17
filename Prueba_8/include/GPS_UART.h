#ifndef GPS_UART_H_
#define GPS_UART_H_
    //========================= Inclusiones =========================
    #include<stdio.h>
    #include<string.h>
    #include<stdlib.h>
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "esp_system.h"
    #include "esp_log.h"
    #include "driver/gpio.h"
    #include "driver/uart.h"

    //#include "../include/I2C.h"
    //#include "../include/LCDI2C.h"

    // Fuentes:
    // https://www.sigmaelectronica.net/trama-gps/
    // https://github.com/Tinyu-Zhao/TinyGPSPlus-ESP32/tree/master/src
    // https://github.com/Tinyu-Zhao/TinyGPSPlus-ESP32/tree/master/src

    //==================================================
    #define PROCESADORA         0
    #define PROCESADORB         1

    void init_Uart0(void);
    void init_Uart2(void);
    esp_err_t CrearTareaGPS_UART(void);

#endif