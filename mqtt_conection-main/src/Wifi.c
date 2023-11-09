#include "../include/Wifi.h"
uint32_t MQTT_CONNEECTED = 0;
static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch (event->event_id)
    {
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
        //ESP_LOGI(TAG, "Trying to connect with Wi-Fi\n");
        break;

    case SYSTEM_EVENT_STA_CONNECTED:
       // ESP_LOGI(TAG, "Wi-Fi connected\n");
        break;

    case SYSTEM_EVENT_STA_GOT_IP:
        //ESP_LOGI(TAG, "got ip: startibg MQTT Client\n");
        mqtt_app_start();
        break;

    case SYSTEM_EVENT_STA_DISCONNECTED:
       // ESP_LOGI(TAG, "disconnected: Retrying Wi-Fi\n");
        esp_wifi_connect();
        break;

    default:
        break;
    }
    return ESP_OK;
}

void wifi_init(void)
{
    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .password = EXAMPLE_ESP_WIFI_PASS,
	     .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start());
}

/*
 * @brief Event handler registered to receive MQTT events
 *
 *  This function is called by the MQTT client event loop.
 *
 * @param handler_args user data registered to the event.
 * @param base Event base for the handler(always MQTT Base in this example).
 * @param event_id The id for the received event.
 * @param event_data The data for the event, esp_mqtt_event_handle_t.
 */
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    //ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        //ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        MQTT_CONNEECTED=1;
        
        msg_id = esp_mqtt_client_subscribe(client, "Potencia_Rni/01", 0);
        //ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

        msg_id = esp_mqtt_client_subscribe(client, "Coordenadas_GPS/01", 1);
        //ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_DISCONNECTED:
        //ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        MQTT_CONNEECTED=0;
        break;
    case MQTT_EVENT_SUBSCRIBED:
        //ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        //ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        //ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        //ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        break;
    case MQTT_EVENT_ERROR:
        //ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        break;
    default:
        //ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

esp_mqtt_client_handle_t client = NULL;

void mqtt_app_start(void)
{
    //ESP_LOGI(TAG, "STARTING MQTT");
    esp_mqtt_client_config_t mqttConfig = {
        .uri = "mqtt://broker.emqx.io",
        .port = 1883
        };
/*
        .client_id = "mqttx_d34688a2",
        .username = "gpauta",
void mqtt_init() {
  esp_mqtt_client_config_t mqttConfig;
  mqttConfig.uri = "ws://test.mosquitto.org";
  mqttConfig.port = 8080;
  mqttConfig.transport = MQTT_TRANSPORT_OVER_WS;
  esp_mqtt_client_start(client);
}*/
    
    client = esp_mqtt_client_init(&mqttConfig);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);
}

void Publisher_Task(void *params)
{
    char *GPS;
    char *START = "MQTT CONECT - OK";
    char *POT;
    
    
    esp_err_t ret6 = nvs_flash_init();
    if (ret6 == ESP_ERR_NVS_NO_FREE_PAGES || ret6 == ESP_ERR_NVS_NEW_VERSION_FOUND) 
                            {
                                ESP_ERROR_CHECK(nvs_flash_erase());
                                ret6 = nvs_flash_init();
                            }
    ESP_ERROR_CHECK(ret6);
    wifi_init();
    
  while (true)
  {
    if(MQTT_CONNEECTED)
    {
        //esp_mqtt_client_publish(client, "System/01",START, 0, 0, 0);

        if (xQueueReceive(GlobalQueue,&GPS,pdMS_TO_TICKS(100)))
        {   
            esp_mqtt_client_publish(client, "Datosgps/01", GPS, 0, 0, 0);
            vTaskDelay(5000 / portTICK_PERIOD_MS);
            //GPS = NULL;
        /*
            //vTaskDelay(5000 / portTICK_PERIOD_MS);
            //vTaskDelay(1000 / portTICK_PERIOD_MS);
            //esp_mqtt_client_publish(client, "Datosgps/01", GPS, 0, 0, 0);
            //vTaskDelay(2000 / portTICK_PERIOD_MS);
        */
        }
        if (xQueueReceive(GlobalQueue1,&POT,pdMS_TO_TICKS(100)))
        {
            esp_mqtt_client_publish(client, "Potencia_Rni/01",POT, 0, 0, 0);
            vTaskDelay(5000 / portTICK_PERIOD_MS);

        }
    }
  }
}

void subir_datos(){

BaseType_t res4 = xTaskCreatePinnedToCore(
    	 Publisher_Task,                     	// Funcion de la tarea a ejecutar
        "Publisher_Task",   	                // Nombre de la tarea como String amigable para el usuario
        configMINIMAL_STACK_SIZE*5, 		// Cantidad de stack de la tarea
        NULL,                          	// Parametros de tarea
        tskIDLE_PRIORITY+1,         	// Prioridad de la tarea -> Queremos que este un nivel encima de IDLE
        NULL,                          		// Puntero a la tarea creada en el sistema
        1                                   // Procesador donde se ejecuta
    );
    
        if(res4 == pdFAIL)
	{
		printf( "Error al crear la tarea.\r\n" );
		while(true);					// si no pudo crear la tarea queda en un bucle infinito
	}
}
