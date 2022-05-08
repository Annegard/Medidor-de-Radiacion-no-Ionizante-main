/*===================Medidor de Radiacion V2 =====================
Autores:
  Casas Alejo
  Pautaso Gaston (con dos "s" en Pautaso)

Fecha:
  09/04/2022

Version:
  V2.0(Unificacion de Modulo GPS, Medidor de Radiacion y LCD)

Descripcion:
  Este medidor de radiacion funciona a partir de un ESP32 con un conjunto de perifericos...

  Falta terminar de explicar que hace, como, etc

Bibliografia:
  http://www.sinaptec.alomar.com.ar/2018/06/esp32-desde-cero-tutorial-4-adc.html //funcionamiento de ADC
*/

/*Librerias*/
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

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

/*Declaracion de objetos*/
LiquidCrystal_I2C lcd(DIRECCION,COLUMNAS,FILAS);  // set the LCD address to 0x27 for a 16 chars and 2 line display

/*Variables Globales*/
float sensorValue = 0;
bool newData = false;
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
  //Inicializacion LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  Serial.begin(115200);//Comunicacion entre ESP32 y Monitor Serie

  //Declaracion de Pulsadores
  pinMode(Pulsador_1, INPUT);//Cambiar valor a mostrar
  pinMode(Pulsador_2, INPUT);//A definir
  pinMode(BLINK, OUTPUT);
}

void loop(){
  Serial.println("Hola");
  //actualizarSemaforo(Pulsador_1);

  Serial.println(digitalRead(Pulsador_1));
  Serial.println(digitalRead(Pulsador_2));

  // if(digitalRead(Pulsador_1)){
  //   Serial.println("LED ENCENDIDO");
  //   digitalWrite(BLINK, HIGH);
  // }
  // else{
  //   Serial.println("LED APAGADO");
  //   digitalWrite(BLINK, LOW);
  // }
  
  delay(100);
}

/*Funcion que se encarga de sensar el pin analogico designado, muestrear el valor recibido y mostrarlo en el LCD*/
void MedidorDeRadiacion_Show_Data(void){
  Serial.println("LED APAGADO");
  digitalWrite(BLINK, LOW);
  return;

}

/*Funcion que sirve para mostrar los datos obtenidos del GPS en el LCD*/
void GPS_Show_Data(void){
  Serial.println("LED ENCENDIDO");
  digitalWrite(BLINK, HIGH);
  return;
}

// Función Actualizar MEF
void actualizarSemaforo(int Pulsador)
{
  switch (estadoActual) {
    case Medidor_De_Radiacion:
    {
      //Codigo
      // lcd.clear();
      // lcd.setCursor(0,0);
      // lcd.print("Medir Radiacion");

      MedidorDeRadiacion_Show_Data();

      if(digitalRead(Pulsador)){
        estadoActual=GPS;
      }
    }
    break;

    case GPS:
    {
      //Codigo
      // lcd.clear();
      // lcd.setCursor(0,0);
      // lcd.print("conectando gps..");
      void GPS_Show_Data();
      if(digitalRead(Pulsador)){
        estadoActual=Medidor_De_Radiacion;
      }
    }
    break;
    
    default:{
      //Si cae en un estado no valido, reinicio
      estadoActual = Medidor_De_Radiacion;
    }
    break;
  } 
}
