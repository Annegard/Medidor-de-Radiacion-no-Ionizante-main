#include "GPS.h"
#include "LCD.h"

unsigned long Tiempo1=0;
int retardo=1000, Lat=0,Lon=0;

void setup()
{ 
  //Serial.begin(115200);
  //GPS_Serial_Init(9600, 12, 13);//GPS_Serial_Init(Baudios, RX, TX)
  delay(1000);
}

void loop()
{
  if((millis() - Tiempo1) > retardo){
    Mostrar_Datos_GPS(LCD, LATITUD, LONGITUD);//Mostrar_Datos_GPS(LCD/SERIAL, DATO_1, DATO_2)
    Tiempo1=millis();
  }
}
