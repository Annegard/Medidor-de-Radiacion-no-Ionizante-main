/*===========================[ Inclusiones ]================================*/
#include "../include/pulsador.h"
#include "../include/LCDI2C.h"

//===========================[ Definiciones ]================================
#define T_ESPERA 10 //cantidad de veces que va a ingresar antes de cambiar de estado

//===========================[ Variables ]===================================
estadoBoton_t estadoPulsador;

extern SemaphoreHandle_t mux2 ;
extern estadoMODO_t estadoModo;

/*======================[Prototipos de funciones]============================*/
//void tareaPulsador( void* taskParmPtr );


void inicializarBoton(gpio_int_type_t pulsador)
{
   estadoPulsador = ALTO;  // Se configura estado inicial

   gpio_set_direction(pulsador, GPIO_MODE_INPUT); //Configura el pin como entrada digital
   gpio_set_pull_mode(pulsador, GPIO_PULLDOWN_ONLY); //Habilita resistencia de PULLDOWN interna
}

void errorBoton( void ){
   estadoPulsador = BAJO;
}

//Actualización de la MEF
void actualizarBoton( gpio_int_type_t pulsador ){
   static uint8_t contDescendente = 0;
   static uint8_t contAscendente = 0;

   switch( estadoPulsador ){

      case BAJO: 
         if( gpio_get_level(pulsador) ){
            estadoPulsador = ASCENDENTE;
         }
      break;

      case ASCENDENTE:      
         if( contAscendente >= T_ESPERA ){
            if( gpio_get_level(pulsador) ){
               estadoPulsador = ALTO;
               botonPresionado();
            } else{
               estadoPulsador = BAJO;
            }
            contAscendente = 0;
         }
         contAscendente++;
      break;

      case ALTO:
         if( !gpio_get_level(pulsador) ){
            estadoPulsador = DESCENDENTE;
         }
      break;

      case DESCENDENTE:      
         if( contDescendente >= T_ESPERA ){
            if( !gpio_get_level(pulsador) ){
               estadoPulsador = BAJO;
               botonLiberado();
            } else{
               estadoPulsador = ALTO;
            }
            contDescendente = 0;
         }
         contDescendente++;
      break;

      default:
         errorBoton();
      break;
   }
   
}

void botonPresionado( void ){
    
}

void botonLiberado(void){
   if (mux2 != NULL){
		if (xSemaphoreTake(mux2, portMAX_DELAY) == pdTRUE){
			estadoModo = estadoModo + 1;
			if (estadoModo == 5){
				estadoModo = 0;
			}

         switch(estadoModo){
            case MENU_PRINCIPAL:
               BorrarPantalla();
               //lcd_print("Hola Nicole");
               printf("cambio modo MENU PRINCIPAL\n");
            break;
            case CONECTAR_GPS:
               printf("cambio modo GPS\n");
            break;
            case MEDIR_POTENCIA:
               printf("cambio modo ADC\n");
            break;
            case GUARDAR_DATOS:
               printf("cambio modo SAVE DATA\n");
            break;
            case CONECTAR_WIFI:
               printf("cambio modo WIFI\n");
            break;

         }
         //printf("cambio modo");
		}
	}
	xSemaphoreGive(mux2);
   //actualizarModo();
   
}

// void tareaPulsador(void *parametros )
// {
//    gpio_int_type_t parametro = *((int*)parametros); // Convertir el puntero a int
//    while (true)
//    {
//       actualizarBoton(parametro);
//       vTaskDelay(50 / portTICK_PERIOD_MS);
//    }
// }