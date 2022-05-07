#include "GPS.h"
#include "LCD.h"

/*================================================VARIABLES================================================*/
SoftwareSerial ss(RX, TX);//Crear objeto para referirse a que pines Comunicarse mediante protocolo UART
TinyGPS gps;//Defino Objeto GPS para comunicarse con el modulo
LiquidCrystal_I2C lcd(DIRECCION, COLUMNAS, FILAS);//crear un objeto lcd (DIRECCIÓN, Tamaño x, Tamño y)
bool newData = false;
unsigned long chars;
unsigned short sentences, failed;
float flat=0, flon=0;
unsigned long age;

/*================================================FUNCIONES================================================*/
void GPS_Load_Data(){
  ss.begin(9600);
  // For one second we parse GPS data and report some key values
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (ss.available())
    {
      char c = ss.read();
      //Serial.write(c); // uncomment this line if you want to see the GPS data flowing
      if (gps.encode(c)){ // Did a new valid sentence come in?
        newData = true;
      }
    }
  }
}

void Mostrar_Datos_GPS(uint8_t PANTALLA, uint8_t DATO_1, uint8_t DATO_2){

  GPS_Load_Data();//
  
  gps.f_get_position(&flat, &flon, &age);
  gps.stats(&chars, &sentences, &failed);

  if (newData){
  switch (PANTALLA){
    case PUERTO_SERIAL://Mostrar datos en el puerto Serial
      Serial.print("LAT=");
      Serial.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
      Serial.print(" LON=");
      Serial.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
      Serial.print(" SAT=");
      Serial.print(gps.satellites() == TinyGPS::GPS_INVALID_SATELLITES ? 0 : gps.satellites());
      Serial.print(" PREC=");
      Serial.print(gps.hdop() == TinyGPS::GPS_INVALID_HDOP ? 0 : gps.hdop());
      
      Serial.print(" CHARS=");
      Serial.print(chars);
      Serial.print(" SENTENCES=");
      Serial.print(sentences);
      Serial.print(" CSUM ERR=");
      Serial.println(failed);

      if (chars == 0){Serial.println("** No characters received from GPS: check wiring **");}
        break;
      case LCD://Mostrar datos en el LCD
        lcd.clear();
        lcd.setCursor (0, 0);
        Imprimir_Datos_LCD(DATO_1);
        lcd.setCursor (0, 1);
        Imprimir_Datos_LCD(DATO_2);
        break;
      default:
        Serial.print("Dato no admitido");
        break;
  }
  }
}

void Imprimir_Datos_LCD(uint8_t DATO){
  switch(DATO){
    case LATITUD:
      lcd.print("LAT=");
      lcd.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
      break;
    case LONGITUD:
      lcd.print("LON=");
      lcd.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
      break;
    case SATELITES:
      lcd.print(" SAT=");
      lcd.print(gps.satellites() == TinyGPS::GPS_INVALID_SATELLITES ? 0 : gps.satellites());
      break;
    case PRECISION:
      lcd.print(" PREC=");
      lcd.print(gps.hdop() == TinyGPS::GPS_INVALID_HDOP ? 0 : gps.hdop());
      break;
  }
  if (chars == 0){
    lcd.clear();
    lcd.setCursor (0, 0);
    lcd.print("NO SE RECIBEN");
    lcd.setCursor (0, 0);
    lcd.print("DATOS");
    delay(100);
  }
}
