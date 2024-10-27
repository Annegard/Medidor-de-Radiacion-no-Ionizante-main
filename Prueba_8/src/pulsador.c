/*===========================[ Inclusiones ]================================*/
#include "../include/pulsador.h"
#include "../include/LCDI2C.h"

//===========================[ Definiciones ]================================
#define T_ESPERA 10 //cantidad de veces que va a ingresar antes de cambiar de estado

//===========================[ Variables ]===================================
estadoBoton_t estadoPulsador;

extern SemaphoreHandle_t mux2 ;
extern estadoMODO_t estadoModo;

int64_t tiempo_inicial=0, tiempo_actual=0;
/*======================[Prototipos de funciones]============================*/
static void errorBoton( void );
static void botonPresionado( void );
static void botonLiberado(gpio_int_type_t pulsador);

void inicializarBoton(gpio_int_type_t pulsador)
{
   estadoPulsador = ALTO;  // Se configura estado inicial

   gpio_set_direction(pulsador, GPIO_MODE_INPUT); //Configura el pin como entrada digital
   gpio_set_pull_mode(pulsador, GPIO_PULLDOWN_ONLY); //Habilita resistencia de PULLDOWN interna
}

static void errorBoton( void ){
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
         tiempo_inicial = esp_timer_get_time() / 1000;
         if( contAscendente >= T_ESPERA ){
            if( gpio_get_level(pulsador) ){
               estadoPulsador = ALTO;
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
         botonPresionado();
      break;

      case DESCENDENTE:
         //tiempo_actual=0;creo que no hace falta
         if( contDescendente >= T_ESPERA ){
            if( !gpio_get_level(pulsador) ){
               estadoPulsador = BAJO;
               botonLiberado(pulsador);
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

static void botonPresionado( void ){
    int64_t tiempo_actual = esp_timer_get_time() / 1000;

   // Verificar si se cumple la condición (por ejemplo, si han pasado 3000 ms)
   if (tiempo_actual - tiempo_inicial >= 3000){
      if (mux2 != NULL){
	      if (xSemaphoreTake(mux2, portMAX_DELAY) == pdTRUE){
            MODO_WIFI = !MODO_WIFI;
         }
      }
      xSemaphoreGive(mux2);
   }
}

static void botonLiberado(gpio_int_type_t pulsador){
   printf("botonLiberado\n");
   //Utilizo semaforo para poder guardar la variable en caso de que otra
   //tarea se encuentre consultandola 
   if (mux2 != NULL){
		if (xSemaphoreTake(mux2, portMAX_DELAY) == pdTRUE){
			estadoModo = estadoModo + 1;

			if (estadoModo == 4){
				estadoModo = 0;
			}
		}
	}
	xSemaphoreGive(mux2);
   
}