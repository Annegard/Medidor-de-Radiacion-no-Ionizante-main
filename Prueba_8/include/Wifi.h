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
//#include "my_data.h"
//#include "protocol_examples_common.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"

#include "freertos/event_groups.h"

#include "nvs_flash.h"
#include "lwip/sys.h"

//===========================[ Definiciones ]================================
#define  WIFI_SSID "alejocasas"
#define  WIFI_PASS "Cartago97"
#define NAME "SENSOR1"
#define PROCESADORA         0
#define PROCESADORB         1

/*======================[Prototipos de funciones]============================*/
void subir_datos_MQTT(void);
void deshabilitar_MQTT(void);

extern QueueHandle_t ColaADC;
extern QueueHandle_t ColaLAT;
extern QueueHandle_t ColaLON;

extern bool MQTT_CONNEECTED;

#endif