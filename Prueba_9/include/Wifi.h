#ifndef WIFI_H_
#define WIFI_H_

    /*==========================================================================================
        Descripcion:
    
    ==========================================================================================*/

    //============================= Inclusiones ================================
    #include "../include/Global.h"
    #include "esp_wifi.h"
    #include "nvs_flash.h"
    #include "esp_event.h"
    #include "esp_netif.h"

    #include "lwip/sockets.h"
    #include "lwip/dns.h"
    #include "lwip/netdb.h"
    #include "lwip/sys.h"

    #include "mqtt_client.h"

    typedef struct{
        uint16_t num_networks;
        wifi_ap_record_t ap_records[20]; 
    } redesWIFI_t;

    //======================Prototipos de funciones============================
    void Publisher_Task();
    void wifi_init(char ssid_string[MAX_SSID_LEN],char password[MAX_PASSWORD_LEN]);
    void mqtt_app_stop();
    redesWIFI_t wifi_scan_task(void);

    #endif