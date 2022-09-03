/*==================[ Inclusiones ]============================================*/
#include "pulsador.h"

/*==================[ Definiciones ]===================================*/

#define T_REBOTE_MS     40
#define T_REBOTE        pdMS_TO_TICKS(T_REBOTE_MS)
#define T_ESPERA 10 //cantidad de veces que va a ingresar antes de cambiar de estado

/*==================[Prototipos de funciones]======================*/

static void errorPulsador( uint8_t  indice );
void botonLiberado();
void tareaPulsadores( void* taskParmPtr );

/*==================[Variables]==============================*/
gpio_int_type_t pulsadorPines[N_PULSADOR] = {GPIO_NUM_12, GPIO_NUM_14};

pulsadorInfo pulsador [N_PULSADOR];

estadoLCD EstadoLCD;
/*==================[Implementaciones]=================================*/

void inicializarPulsador( void )
{
    for(int i = 0; i < N_PULSADOR; i++)
    {
        pulsador[i].tecla             = pulsadorPines[i];
        pulsador[i].estado            = ALTO;                     //Estado inicial

        gpio_pad_select_gpio(pulsador[i].tecla);
        gpio_set_direction(pulsador[i].tecla , GPIO_MODE_INPUT);
        gpio_set_pull_mode(pulsador[i].tecla, GPIO_PULLDOWN_ONLY); //Habilita resistencia de PULLDOWN interna
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}

static void errorPulsador( uint8_t  indice )
{
    pulsador[indice].estado = ALTO;
}


// pulsador_ Update State Function
bool actualizarPulsador( uint8_t  indice)
{
   static uint8_t contDescendente = 0;
   static uint8_t contAscendente = 0;

   switch(pulsador[indice].estado){

      case BAJO: 
         if( gpio_get_level(pulsador[indice].tecla) ){
            pulsador[indice].estado = ASCENDENTE;
         }
      break;

      case ASCENDENTE:      
         if( contAscendente >= T_ESPERA ){
            if( gpio_get_level(pulsador[indice].tecla) ){
               pulsador[indice].estado = ALTO;
            } else{
               pulsador[indice].estado = BAJO;
            }
            contAscendente = 0;
         }
         contAscendente++;
      break;

      case ALTO:
         if( !gpio_get_level(pulsador[indice].tecla) ){
            pulsador[indice].estado = DESCENDENTE;
         }
      break;

      case DESCENDENTE:      
         if( contDescendente >= T_ESPERA ){
            if( !gpio_get_level(pulsador[indice].tecla) ){
               pulsador[indice].estado = BAJO;
               botonLiberado();
            } else{
               pulsador[indice].estado = ALTO;
            }
            contDescendente = 0;
         }
         contDescendente++;
      break;

      default:
         errorPulsador(indice);
      break;
   }

   return(EstadoLCD);
}

void botonLiberado(){

    if (EstadoLCD == ADC)
    {
        EstadoLCD = GPS;
    }
    else
    {
        EstadoLCD = ADC;
    }
}