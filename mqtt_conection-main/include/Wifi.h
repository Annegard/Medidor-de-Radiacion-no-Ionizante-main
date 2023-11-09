#ifndef WIFI_H_
#define WIFI_H_


#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
//#include "protocol_examples_common.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "../include/I2C.h"
#include "../include/LCDI2C.h"
#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"

//static const char *TAG = "MQTT_EXAMPLE";
#define  EXAMPLE_ESP_WIFI_SSID "CLAUSENDAILOFF"
#define  EXAMPLE_ESP_WIFI_PASS "CONFIAENVOS2022"
void mqtt_app_start(void);
void subir_datos(void);
QueueHandle_t GlobalQueue;
QueueHandle_t GlobalQueue1;
#endif