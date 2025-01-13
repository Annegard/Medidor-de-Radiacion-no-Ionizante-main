#include <string.h>
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#define WIFI_SSID_AP "ESP32_Config"    // Nombre de la red del AP para configurar el ESP32
#define WIFI_PASS_AP "12345678"        // Contraseña del AP (mínimo 8 caracteres)
#define WIFI_MAX_CONN 4                // Número máximo de conexiones simultáneas al AP

static const char *TAG = "wifi_softap_sta";
char ssid_sta[32] = "";                // SSID para modo STA
char pass_sta[64] = "";                // Contraseña para modo STA

// Eventos WiFi
static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STACONNECTED) {
        ESP_LOGI(TAG, "Un dispositivo se ha conectado al AP");
        // Aquí puedes iniciar un servidor para recibir el SSID y password desde el dispositivo móvil
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "Desconectado de la red. Reintentando...");
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "Conectado a la red. IP asignada: " IPSTR, IP2STR(&event->ip_info.ip));
    }
}

// Configura el modo Access Point para recibir credenciales de red
void wifi_init_softap() {
    wifi_config_t wifi_config = {
        .ap = {
            .ssid = WIFI_SSID_AP,
            .password = WIFI_PASS_AP,
            .ssid_len = strlen(WIFI_SSID_AP),
            .max_connection = WIFI_MAX_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
    };

    if (strlen(WIFI_PASS_AP) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "Modo AP inicializado con SSID: %s", WIFI_SSID_AP);
}

// Configura el modo Station con las credenciales ingresadas
void wifi_init_sta(char *ssid, char *password) {
    wifi_config_t wifi_config = {
        .sta = {
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
            .pmf_cfg = {
                .capable = true,
                .required = false
            },
        },
    };
    strcpy((char *)wifi_config.sta.ssid, ssid);
    strcpy((char *)wifi_config.sta.password, password);

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "Modo STA configurado con SSID: %s", ssid);
}

void app_main() {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, NULL));

    wifi_init_softap();  // Inicializa en modo AP

    // Aquí, agrega el código para manejar la conexión de un dispositivo al AP
    // y obtener el SSID y password de la red a la que conectarse.
    // Cuando tengas las credenciales, cambia a modo STA llamando:
    // wifi_init_sta(ssid_sta, pass_sta);
}
