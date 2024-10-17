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

    extern SemaphoreHandle_t Semaforo_GPS;
    
    void init_Uart2();//9600 baudios a GPS
    void rx_task();

#endif