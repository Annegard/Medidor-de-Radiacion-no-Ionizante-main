#include "../include/Wifi.h"

/*               Notas sobre la programacion del WIFI
 *
 *  El modelo de programación WiFi se puede representar como la siguiente imagen:
 *
 *
 *                            default handler              user handler
 *  -------------             ---------------             ---------------
 *  |           |   event     |             | callback or |             |
 *  |   tcpip   | --------->  |    event    | ----------> | application |
 *  |   stack   |             |     task    |  event      |    task     |
 *  |-----------|             |-------------|             |-------------|
 *                                  /|\                          |
 *                                   |                           |
 *                            event  |                           |
 *                                   |                           |
 *                                   |                           |
 *                             ---------------                   |
 *                             |             |                   |
 *                             | WiFi Driver |/__________________|
 *                             |             |\     API call
 *                             |             |
 *                             |-------------|
 */
// El controlador WiFi puede considerarse como una caja negra, no sabe nada sobre el código de capa superior,
// como la pila TCPIP, la tarea de aplicación, la tarea de evento, etc., todo lo que puede hacer es recibir 
// una llamada API desde la capa superior o publicar una cola de eventos en una cola específica, que se inicializa 
// mediante API esp_wifi_init().

// La tarea de evento es una tarea en segundo plano, que recibe eventos del controlador WiFi o de otro subsistema,
// como la pila TCPIP, la tarea de evento llamará a la función de devolución de llamada predeterminada al recibir
// el evento. Por ejemplo, al recibir el evento WIFI_EVENT_STA_CONNECTED, llamará a la API esp_netif para iniciar 
// el cliente DHCP en su controlador predeterminado.

// La aplicación puede registrar su propia función de devolución de llamada de evento mediante API esp_event_init,
// luego se llamará a la función de devolución de llamada de la aplicación después de la devolución de llamada 
// predeterminada. 

// Además, si la aplicación no desea ejecutar la devolución de llamada en la tarea del evento, lo que debe hacer
// es publicar el evento relacionado en la tarea de la aplicación en la función de devolución de llamada de la 
// aplicación. La tarea (código) de la aplicación generalmente combina todo esto, llama a las API para iniciar 
// el sistema/WiFi y manejar los eventos cuando sea necesario.

QueueHandle_t GlobalQueue;
QueueHandle_t GlobalQueue1;

uint32_t MQTT_CONNEECTED = 0;

extern float Latitud;
extern float Longitud;
extern float valor_dB;

/*======================[Prototipos de funciones]============================*/
static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
void wifi_init(void);
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
void mqtt_app_start(void);
void PublicarDatos(void *taskParmPtr);

//Controlador de eventos, para recibir eventos WIFI
static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data){

    switch (event_id)
    {
    case WIFI_EVENT_STA_START:
        esp_wifi_connect();//Conecta el STA (ESP) al AP "punto de acceso" (Router)
        //ESP_LOGI(TAG, "Trying to connect with Wi-Fi\n");
        break;

    case WIFI_EVENT_STA_CONNECTED:
       // ESP_LOGI(TAG, "Wi-Fi connected\n");
        break;

    case IP_EVENT_STA_GOT_IP:
        //ESP_LOGI(TAG, "got ip: startibg MQTT Client\n");
        mqtt_app_start();
        break;

    case WIFI_EVENT_STA_DISCONNECTED:
       // ESP_LOGI(TAG, "disconnected: Retrying Wi-Fi\n");

        esp_wifi_connect();//Conecta el STA (ESP) al AP "punto de acceso" (Router)
        break;

    default:
        break;
    }
}

//Configuracion y arranque del modulo WIFI
void wifi_init(void)
{
    /*======================[Paso 1: Inicializacion del WIFI]============================*/

    //Inicializar la pila TCP/IP
    ESP_ERROR_CHECK(esp_netif_init());

    //Crear bucle de eventos por default
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    //Carga la WIFI_STA por defecto. En caso de cualquier error de inicio, esta API se cancela.
    esp_netif_create_default_wifi_sta();

    //Cargo la configuracion de Wifi por default
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    
    // INICIALIZAR WIFI
    // Asigna recursos para el controlador WiFi, como estructura de control WiFi, búfer RX/TX,
    // estructura WiFi NVS, etc. 
    // también inicia la tarea WiFi...
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    /*======================[Paso 2: Configuracion del wifi]============================*/
    //Como el modulo de WIFI funciona mediante eventos y se comunica mediante APIs se utiliza
    //una funcion que cuando ocurra un evento del tipo WIFI ejecute determinada funcion.
    //(Similar a las interrupciones). 

    //Como para usar esp_event_loop_init() requiero de la libreria esp_event_legacy.h, aun así
    //ocaciona problemas de reconociemiento, por ende utilice la funcion esp_event_handler_register
    //que tambien sirve para los eventos del wifi.

    //Ademas la corroboracion de errores "ESP_ERROR_CHECK" tambien jodia, asi que deje la funcion 
    //wifi_event_handler() como una funcion vacia.

    /* descripcion de la funcion esp_err_t esp_event_handler_register();

        Esta función se puede utilizar para registrar un controlador para:
        
        (1) eventos específicos:
        Especifique event_base y event_id exactos

        (2) todos los eventos de una determinada base de eventos:(Este es el caso)
        Especifique la base de eventos exacta y use ESP_EVENT_ANY_ID como event_id

        (3) todos los eventos conocidos por el bucle de eventos del sistema:
        Use ESP_EVENT_ANY_BASE para event_base y ESP_EVENT_ANY_ID como event_id

        Es posible registrar varios controladores para eventos. También es posible 
        registrar un único controlador para múltiples eventos. Sin embargo, registrar
        el mismo controlador para el mismo evento varias veces provocaría que se 
        sobrescribieran los registros anteriores.
    */

    int nada = 0;//si le pongo NULL solamente ROMPE LOS HUEVOS con los warnings
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, &nada);

    //cargo el modo, id y contraseña del wifi al que se va a conectar el ESP
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
	     .threshold.authmode = WIFI_AUTH_WPA2_PSK,//creo que esto era para la seguridad del wifi
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

    /*======================[Paso 3: Arranque del Wifi]============================*/
    ESP_ERROR_CHECK(esp_wifi_start());

    /*======================[Paso 4: Coneccion del wifi]============================*/
    //En este caso me conecto dentro del wifi_event_handler cuando arranca el modulo 
    //y cada vez que este desconectado
}

//Controlador de eventos, para recibir eventos MQTT
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
        MQTT_CONNEECTED=1;//Si consigue conectarse al broker habilita la tarea para publicar datos
        
        msg_id = esp_mqtt_client_subscribe(client, "Potencia_Rni/01", 0);
        if(msg_id<0){printf("Falla al publicar Potencia_RNI");}

        msg_id = esp_mqtt_client_subscribe(client, "Coordenadas_GPS/01", 1);
        if(msg_id<0){printf("Falla al publicar Potencia_RNI");}

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

//Declaro la varible cliente de forma global ya que la utilizo tambien para
//hacer publish al broker
esp_mqtt_client_handle_t client = NULL;

void mqtt_app_start(void)
{
    /* estructura de configuración del cliente MQTT*/
    esp_mqtt_client_config_t mqttConfig = {
        .broker.address.uri = "mqtt://broker.emqx.io",
        .broker.address.port = 1883,
        };
    
    //Inicializa el driver para los eventos MQTT
    client = esp_mqtt_client_init(&mqttConfig);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);
}

void PublicarDatos(void *taskParmPtr)
{
    
    //sinceramente no entiendo para que sirve pero reserva una particion de memoria
    esp_err_t ret6 = nvs_flash_init();

    if (ret6 == ESP_ERR_NVS_NO_FREE_PAGES || ret6 == ESP_ERR_NVS_NEW_VERSION_FOUND){
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret6 = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret6);

    //Configuracion y arranque del modulo WIFI
    wifi_init();

    char str[20];

    while (true){
        //Si consigue conectarse al broker habilita la tarea para publicar datos
        if(MQTT_CONNEECTED)
        {   
            //Publicar Potencia
            if (xQueueReceive(GlobalQueue1,&valor_dB,pdMS_TO_TICKS(100))){

                sprintf(str, "%f", valor_dB);//Paso variable Float a char
                esp_mqtt_client_publish(client, "Potencia_Rni/01",str, 0, 0, 0);
                vTaskDelay(5000 / portTICK_PERIOD_MS);
            }

            //Publicar latitud
            if (xQueueReceive(GlobalQueue,&Latitud,pdMS_TO_TICKS(100))){
                sprintf(str, "%f", Latitud);//Paso variable Float a char
                esp_mqtt_client_publish(client, "Latitud", str, 0, 0, 0);
                vTaskDelay(5000 / portTICK_PERIOD_MS);
            }

            //Publicar longitud
            if (xQueueReceive(GlobalQueue,&Longitud,pdMS_TO_TICKS(100))){
                sprintf(str, "%f", Longitud);//Paso variable Float a char 
                esp_mqtt_client_publish(client, "Longitud", str, 0, 0, 0);
                vTaskDelay(5000 / portTICK_PERIOD_MS);
            }
        }
        else{vTaskDelay(5000 / portTICK_PERIOD_MS);}
    }
}

void CrearTareaMQTT(){
    BaseType_t res4 = xTaskCreatePinnedToCore(
            PublicarDatos,                     	// Funcion de la tarea a ejecutar
            "PublicarDatos",   	                // Nombre de la tarea como String amigable para el usuario
            configMINIMAL_STACK_SIZE*5, 		// Cantidad de stack de la tarea
            NULL,                          	    // Parametros de tarea
            tskIDLE_PRIORITY+1,         	    // Prioridad de la tarea -> Queremos que este un nivel encima de IDLE
            NULL,                          		// Puntero a la tarea creada en el sistema
            PROCESADORA                         // Procesador donde se ejecuta
        );
        
    if(res4 == pdFAIL){
        printf( "Error al crear la tarea.\r\n" );
        while(true);					// si no pudo crear la tarea queda en un bucle infinito
    }
}