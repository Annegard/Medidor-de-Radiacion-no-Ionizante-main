#ifndef GPS_H_
#define GPS_H_

  #include <SoftwareSerial.h>
  #include <TinyGPS.h>

  #define LCD           0
  #define PUERTO_SERIAL 1
  #define LATITUD       2
  #define LONGITUD      3
  #define SATELITES     4
  #define PRECISION     5

  #define RX  12
  #define TX  13

  void GPS_Load_Data();
  void Imprimir_Datos_LCD(uint8_t DATO);
  void Mostrar_Datos_GPS(uint8_t PANTALLA, uint8_t DATO_1, uint8_t DATO_2);

#endif
