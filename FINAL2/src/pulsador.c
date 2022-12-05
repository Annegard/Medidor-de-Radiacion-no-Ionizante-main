#include "../include/pulsador.h"

#define T_ESPERA 10 //cantidad de veces que va a ingresar antes de cambiar de estado
estadoBoton_t estadoPulsador;
int cambioModo = 0;
//#define button_isr GPIO_NUM_26
#define PULSADOR GPIO_NUM_25
extern SemaphoreHandle_t mux2 ;
/*
esp_err_t init_isr(void){


   gpio_config_t pGPIOConfig;
   pGPIOConfig.pin_bit_mask = (1ULL << button_isr);
   pGPIOConfig.mode = GPIO_MODE_DEF_INPUT;
   pGPIOConfig.pull_up_en = GPIO_PULLUP_DISABLE;
   pGPIOConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;
   pGPIOConfig.intr_type = GPIO_INTR_LOW_LEVEL;

   gpio_config(&pGPIOConfig);
   gpio_install_isr_service(0);
   gpio_isr_handler_add(button_isr,isr_handler,NULL);

   return ESP_OK;
}

void isr_handler(void *arg1){

   cambioModo = cambioModo+1;

    if (cambioModo==4){
      cambioModo=0;
   }

}*/
void inicializarBoton( gpio_int_type_t pulsador )
{
    estadoPulsador = ALTO;  // Se configura estado inicial
    gpio_pad_select_gpio(pulsador);
    gpio_set_direction(pulsador, GPIO_MODE_INPUT); //Configura el pin como entrada digital
    gpio_set_pull_mode(pulsador, GPIO_PULLDOWN_ONLY); //Habilita resistencia de PULLDOWN interna
   //gpio_pad_select_gpio(GPIO_NUM_26);
   // gpio_set_direction(GPIO_NUM_26, GPIO_MODE_OUTPUT);
   BaseType_t res = xTaskCreatePinnedToCore(
    	   tareaPulsador,                     	// Funcion de la tarea a ejecutar
        "tareaPulsador",   	                // Nombre de la tarea como String amigable para el usuario
        configMINIMAL_STACK_SIZE*2, 		// Cantidad de stack de la tarea
        NULL,                          	// Parametros de tarea
        tskIDLE_PRIORITY,         	// Prioridad de la tarea -> Queremos que este un nivel encima de IDLE
        NULL,                          		// Puntero a la tarea creada en el sistema
        0
    );

    // Gestion de errores
	if(res == pdFAIL)
	{
		printf( "Error al crear la tarea.\r\n" );
		while(true);					// si no pudo crear la tarea queda en un bucle infinito
	}
}

void errorBoton( void )
{
   estadoPulsador = BAJO;
}

//Actualización de la MEF
void actualizarBoton( gpio_int_type_t pulsador )
{
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

void botonPresionado( void )
{
    
}

void botonLiberado(void)
{
   if (mux2 != NULL)
   {
      if (xSemaphoreTake(mux2, portMAX_DELAY) == pdTRUE)
      {
         cambioModo = cambioModo + 1;
         if (cambioModo == 5)
         {
            cambioModo = 0;
         }
         actualizardisplay(cambioModo);
      }
   }
   xSemaphoreGive(mux2);
}
void actualizardisplay(int selector)
{
   // char receivedValue[50];

         switch (cambioModo)
         {

         case 0:
            BorrarPantalla();
            lcd_gotoxy(1, 1);
            lcd_print("1-GPS");
            lcd_gotoxy(9, 1);
            lcd_print("2-ADC");
            lcd_gotoxy(1, 2);
            lcd_print("3-SAVE");
            lcd_gotoxy(9, 2);
            lcd_print("4-SEND");

            break;

         case 1:
            BorrarPantalla();
            lcd_gotoxy(1, 1);
            lcd_print("Iniciando GPS...");
            break;

         case 2:

            BorrarPantalla();
            lcd_gotoxy(1, 1);
            lcd_print("Potencia ");
            break;

         case 3:
            BorrarPantalla();
            lcd_gotoxy(1, 1);
            lcd_print("Guardando");
            lcd_gotoxy(1, 2);
            lcd_print("Datos...");
            break;
         case 4:
            BorrarPantalla();
            lcd_gotoxy(1, 1);
            lcd_print("Conectando");
            lcd_gotoxy(1, 2);
            lcd_print("Wi-Fi...");
            break;

         default:
            lcd_gotoxy(1, 1);
            lcd_print("default");
            errorBoton();
            break;
         }
      }


void tareaPulsador(void *taskParmPtr)
{
   while (true)
   {
      actualizarBoton(PULSADOR);
      vTaskDelay(10 / portTICK_PERIOD_MS);
   }
}