#ifndef Http_client_H_
#define Http_client_H_
/*==================[ Inclusiones ]============================================*/
#include "esp_netif.h"
#include "esp_http_client.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "openssl/ssl.h"
//=========================== Definiciones ================================

/*==================[Prototipos de funciones]======================*/
void Navegar_url(char * str );

//=========================== Variables ================================



#include "../main/Http_client.c"
#endif