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
#include <TinyGPS.h>

/*Definiciones*/
#define Pulsador_1 25
#define Pulsador_2 26
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
SoftwareSerial ss(RX_GPS,TX_GPS);//(RX,TX)
TinyGPS gps;

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

  //Inicializacion GPS
  ss.begin(9600);//Comunicacion entre modulo GPS y ESP32
  //Serial.begin(115200);//Comunicacion entre ESP32 y Monitor Serie

  //Declaracion de Pulsadores
  pinMode(Pulsador_1, INPUT_PULLUP);//Cambiar valor a mostrar
  pinMode(Pulsador_2, INPUT_PULLUP);//A definir
}

void loop()
{
  //void GPS_Comunication();
  actualizarSemaforo(Pulsador_1);
  delay(100);
}

/*Funcion que se encarga de sensar el pin analogico designado, muestrear el valor recibido y mostrarlo en el LCD*/
void MedidorDeRadiacion_Show_Data(void){
  lcd.setCursor(0,0);
  medicion=analogRead(ADCpin);//guardo la medicion Realizada en una variable global
                              //por si deceo utilizarla en otras funciones
  lcd.print("Medicion:");
  lcd.print(medicion);

}

/*Funcion que sirve para mostrar los datos obtenidos del GPS en el LCD*/
void GPS_Show_Data(void){
  float flat, flon;
  unsigned long age;
  gps.f_get_position(&flat, &flon, &age);
  
  lcd.setCursor(0,0);
  lcd.print("LAT=");
  lcd.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);

  lcd.setCursor(1,0);
  lcd.print("LON=");
  lcd.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
  
  //lcd.print(" SAT=");
  //lcd.print(gps.satellites() == TinyGPS::GPS_INVALID_SATELLITES ? 0 : gps.satellites());
  //lcd.print(" PREC=");
  //lcd.print(gps.hdop() == TinyGPS::GPS_INVALID_HDOP ? 0 : gps.hdop());

  newData = false;//Coloca newData en False hasta que se reciba nueva informacion valida del satelite

  return;
}

/*Funcion que sirve para comunicarse con el modulo GPS y cambia newData a "true" si esta es valida*/
void GPS_Comunication(void){

  unsigned long chars;
  unsigned short sentences, failed;

  // For one second we parse GPS data and report some key values
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (ss.available())
    {
      char c = ss.read();
      //Serial.write(c); //Datos que recibe el GPS mediante protocolo NMEA-0183 del satelite
      if (gps.encode(c)) //Si se logra una nueva sentencia newData cambia a true
        newData = true;
    }
  }
  
  gps.stats(&chars, &sentences, &failed);
  //lcd.print(" CHARS=");//Imprimir Palabras recibidas
  //lcd.print(chars);
  //lcd.print("Sentencias=");//Imprimir Sentencias Recibidas
  //lcd.print(sentences);
  //lcd.print(" CSUM ERR=");//Imprimir Errores recibidos
  //lcd.println(failed);*/
  if (chars == 0)//Si no se logra comunicacion
    lcd.print("GPS no connect");
}

// Función Actualizar MEF
void actualizarSemaforo(int Pulsador)
{
	switch (estadoActual) {
		case Medidor_De_Radiacion:
    {
      //Codigo
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Medir Radiacion");

      //MedidorDeRadiacion_Show_Data();

      if(digitalRead(Pulsador)){
        estadoActual=GPS;
      }
		}
		break;

		case GPS:
    {
      //Codigo
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("conectando gps..");
			/*
      if(newData)//Si hay nuevos datos recibidos los imprime en el LCD
      {
        GPS_Show_Data();
      }*/
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
