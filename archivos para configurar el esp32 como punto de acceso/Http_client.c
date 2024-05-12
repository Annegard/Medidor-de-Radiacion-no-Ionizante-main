/*=============================================================================
 * Author: Burgos Fabian, Casas Alejo, Rubiolo Bruno
 * Date: 01/10/2021 
 * Board: ESP32
 * Entorno de programacion: idf original sin Platformio
*===========================================================================*/

/*==================[ Inclusiones ]============================================*/
#include "../include/Http_client.h"

/*==================[Prototipos de funciones]======================*/
void Navegar_url(char * str );
esp_err_t _http_event_handle(esp_http_client_event_t *evt);

/*==================[Implementaciones]=================================*/


/*========================================================================
Funcion: Navegar_url
Descripcion: Navega por la url indicada
Parametro de entrada:
                        char * str=Puntero hacia la cadena que contiene la url
No retorna nada
========================================================================*/
void Navegar_url( char * str)
{
    
        esp_http_client_config_t config = {                  //Configura el cliente http con la url a la que se desea acceder
            .url = str, //Ademas se agrega la direccion de la funcion a la cual se debe llamara frente a un evento
            .event_handler = _http_event_handle,
            };
        esp_http_client_handle_t client = esp_http_client_init(&config);
        esp_err_t err = esp_http_client_perform(client);

        if (err == ESP_OK) {
                ESP_LOGI(TAG, "Status = %d, content_length = %d",
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
        }
        esp_http_client_cleanup(client);
}


/*========================================================================
Funcion: _http_event_handle
Descripcion: Funcion que se llama cuando ocurre un evento de navegacion HTTP
Parametro de entrada:
                     esp_http_client_event_t *evt= tipo de evento que ocurrio
Retorna: ESP_OK
========================================================================*/
esp_err_t _http_event_handle(esp_http_client_event_t *evt)
{
    switch(evt->event_id) {  //Segun el caso del evento, muestra diferente cosas en el logger
        case HTTP_EVENT_ERROR:
            ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:  //Si se navego y se obtuvo el HEADER del sitio
            ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER");
            printf("%.*s", evt->data_len, (char*)evt->data); //Imprime el HEADER o cabezera recibida
            break;
        case HTTP_EVENT_ON_DATA: //Si se navego y se obtuvo el html del sitio
            ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            if (!esp_http_client_is_chunked_response(evt->client)) { 
                printf("%.*s", evt->data_len, (char*)evt->data); //Muestra el html del sitio
            }

            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            break;
    }
    return ESP_OK;
}