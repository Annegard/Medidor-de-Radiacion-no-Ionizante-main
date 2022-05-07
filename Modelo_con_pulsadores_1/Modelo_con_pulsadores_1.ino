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

/*Variables Globales*/
bool EstadoPulsador=false;
bool EstadoAnterior=false;
float medicion=0;

//Defino Maquina de estados para valor a mostrar
typedef enum{
  Medidor_De_Radiacion,
  GPS
} estadoMEF_t;

estadoMEF_t estadoActual, estadoAnterior; // Variable de estado (global)


/*Prototipos de Funciones*/
void GPS_Comunication(void);
void GPS_Show_Data(void);
void MedidorDeRadiacion_Show_Data(void);
void actualizarSemaforo(int Pulsador);


void setup()
{
  Serial.begin(115200);//Comunicacion entre ESP32 y Monitor Serie

  //Declaracion de Pulsadores
  pinMode(Pulsador_1, INPUT_PULLUP);//Cambiar valor a mostrar
  pinMode(Pulsador_2, INPUT_PULLUP);//A definir
  pinMode(BLINK, OUTPUT);
}

void loop(){

  actualizarSemaforo(Pulsador_1);
  
  delay(100);
}

// Función Actualizar MEF
void actualizarSemaforo(int Pulsador)
{
  switch (estadoActual){
    case Medidor_De_Radiacion:
    {
      //Codigo
      Serial.println("PULSADOR 1 PRESIONADO");
      digitalWrite(BLINK, HIGH);

      if(digitalRead(Pulsador)){
        estadoActual=GPS;
      }
    }
    break;

    case GPS:
    {
      //Codigo
      Serial.println("LED APAGADO");
      digitalWrite(BLINK, LOW);

      if(digitalRead(Pulsador)){
        estadoActual=Medidor_De_Radiacion;
      }
    }
    break;
    
    default:{
      //Por defecto cae en medidor de Radiacion
      estadoActual = Medidor_De_Radiacion;
    }
    break;
  } 
}