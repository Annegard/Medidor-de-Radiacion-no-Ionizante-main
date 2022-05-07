/*Definiciones*/
#define Pulsador_1 25
#define Pulsador_2 26
#define BLINK 33
//Configuracion del LCD
#define DIRECCION 0x27
#define COLUMNAS  16
#define FILAS     2
//Configuracion de pines de GPS
#define RX_GPS  12
#define TX_GPS  13
//Configuracion de pin de ADC
#define ADCpin  34
#define T_ESPERA 10

typedef enum{
  Medidor_De_Radiacion,
  GPS
} estadoProceso_t;

estadoProceso_t EstadoActual; // Variable de estado (global)

//Defino Maquina de estados para valor a mostrar
typedef enum{
   ALTO,
   BAJO,
   DESCENDENTE,
   ASCENDENTE
} estadoBoton_t;
estadoBoton_t estadoPulsador;

/*Prototipos de Funciones*/
void GPS_Comunication(void);
void GPS_Show_Data(void);
void MedidorDeRadiacion_Show_Data(void);
void actualizarProceso(void);
void actualizarBoton(int pulsador);


void myprint(void*){   
  
  VOID SETUP(){
    
  Serial.begin(115200);//Comunicacion entre ESP32 y Monitor Serie
  //Declaracion de Pulsadores
  pinMode(Pulsador_1, INPUT_PULLUP);//Cambiar valor a mostrar
  pinMode(Pulsador_2, INPUT_PULLUP);//A definir
  pinMode(BLINK, OUTPUT);
  }

  VOID LOOP(){    
   actualizarBoton(Pulsador_1);
  //actualizarBoton(Pulsador_2);
  switch(EstadoActual){
    case Medidor_De_Radiacion:
    {
      digitalWrite(BLINK,HIGH);
    }
    break;

    case GPS:
    {
      digitalWrite(BLINK,LOW);
    }
    break;
    
    default:{
      //Por defecto cae en medidor de Radiacion
      EstadoActual = Medidor_De_Radiacion;
    }
    break;
  }
  }
}

void actualizarProceso()
{
  if(EstadoActual == Medidor_De_Radiacion){
    EstadoActual = GPS;
  }
  else{
    EstadoActual = Medidor_De_Radiacion;
  } 
}
void actualizarBoton(int pulsador)
{
  static uint8_t contDescendente = 0;
  static uint8_t contAscendente = 0;

   switch(estadoPulsador){

      case BAJO:

         if( digitalRead(pulsador)==1 ){
            estadoPulsador = ASCENDENTE;
         }
      break;
//D
      case ASCENDENTE:

         if( contAscendente >= T_ESPERA ){
            if( digitalRead(pulsador) ){
               estadoPulsador = ALTO;
            }
            else{
               estadoPulsador = BAJO;
            }
            contAscendente = 0;
         }
         contAscendente++;
         delay(1);
      break;

      case ALTO:
         if( !digitalRead(pulsador) ){
            estadoPulsador = DESCENDENTE;
         }
      break;

      case DESCENDENTE:
     
         if( contDescendente >= T_ESPERA ){
            if( !digitalRead(pulsador) ){
              Serial.println("Cambiar Modo");
              actualizarProceso();
              estadoPulsador = BAJO;
            }
            else{
              estadoPulsador = ALTO;
            }
            contDescendente = 0;
         }
         contDescendente++;
         delay(1);
      break;

      default:
        estadoPulsador = BAJO;
      break;
   }
}
