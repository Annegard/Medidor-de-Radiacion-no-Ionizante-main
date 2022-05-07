/*Definiciones*/
#define Pulsador_1 25
#define Pulsador_2 26
#define BLINK 2
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

/*Variables Globales*/
bool EstadoPulsador=false;
bool EstadoAnterior=false;
float medicion=0;

//Defino Maquina de estados para valor a mostrar
typedef enum{
  Medidor_De_Radiacion,
  GPS
} estadoProceso_t;

estadoProceso_t EstadoActual; // Variable de estado (global)

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
void actualizarBoton(int Pulsador);

void setup()
{
  Serial.begin(115200);//Comunicacion entre ESP32 y Monitor Serie

  //Declaracion de Pulsadores
  pinMode(Pulsador_1, INPUT_PULLUP);//Cambiar valor a mostrar
  pinMode(Pulsador_2, INPUT_PULLUP);//A definir
  pinMode(BLINK, OUTPUT);
}

void loop(){

  actualizarBoton(Pulsador_1);

  switch(EstadoActual){
    case Medidor_De_Radiacion:
    {
      //Codigo
      Serial.println("Ver Medidor de Radiacion");
    }
    break;

    case GPS:
    {
      //Codigo
      Serial.println("Ver GPS");
    }
    break;
    
    default:{
      //Por defecto cae en medidor de Radiacion
      EstadoActual = Medidor_De_Radiacion;
    }
    break;
  }
}

// Función Actualizar MEF
void actualizarProceso()
{
  if(EstadoActual == Medidor_De_Radiacion){
    EstadoActual = GPS;
  }
  else{
    EstadoActual = Medidor_De_Radiacion;
  }
  
}

//Actualización de la MEF
void actualizarBoton(int pulsador)
{
  uint8_t contDescendente = 0;
  uint8_t contAscendente = 0;

   switch(estadoPulsador){

      case BAJO: 
         if( digitalRead(pulsador) ){
            estadoPulsador = ASCENDENTE;
         }
      break;

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
      break;

      default:
        estadoPulsador = BAJO;
      break;
   }
}