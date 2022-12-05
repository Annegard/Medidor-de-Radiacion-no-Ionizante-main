/**
 * @file main.c
 * @author Alejo Casas, Gaston Pautasso
 * @brief
 * @version 4.1
 * @date 2022-09-03
 *
 * @copyright Copyright (c) 2022
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <driver/adc.h> //ADC
#include "driver/gpio.h"
#include <SoftwareSerial.h>
#include <TinyGPS.h>

// //=========================== Definiciones ================================// //

#define T_REBOTE_MS 40
#define T_REBOTE pdMS_TO_TICKS(T_REBOTE_MS)
#define T_ESPERA 10 // cantidad de veces que va a ingresar antes de cambiar de estado

// //=========================== Variables ================================// //

float LecturaADC = 0;//Valor donde guardo las lecturas del ADC
const uint8_t ENTRADA_1 = 12;//Pin para interrupcion y cambio de estado del LCD
bool EstadoLCD = true, EstadoLED = true, Auxiliar1 = true, Auxiliar2 = true; // Variables Auxiliares
bool newData = false;
unsigned long chars;
unsigned short sentences, failed;
unsigned long Tiempo1 =0;
uint16_t Retardo=1000;

//Objetos
LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display
TinyGPS gps;
SoftwareSerial ss(15, 0);//ss(Rx,Tx)

// //=========================== Prototipos de funciones ================================// //
void GPS_LCD();
void IRAM_ATTR isr();
void BLINK();

void setup()
{
  //Configuracion monitor serie
  Serial.begin(115200);
  ss.begin(9600);
  
  //Configuracion LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Hello, world!");

  // Configuracion de ADC
  adc1_config_width(ADC_WIDTH_12Bit); // configuran la resoluncion
  adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_11db);
    
  delay(1000);
  attachInterrupt(ENTRADA_1, isr, FALLING); // hook isr for specific gpio pin

  pinMode(2, OUTPUT);
}

void loop()
{
  

  while (ss.available()){
      char c = ss.read();
      // Serial.write(c); // uncomment this line if you want to see the GPS data flowing
        if (gps.encode(c)) // Did a new valid sentence come in?
        newData = true;
  }
  
  
  // For one second we parse GPS data and report some key values
  if((millis()-Tiempo1)>=Retardo){
    LecturaADC = adc1_get_raw(ADC1_CHANNEL_0); // Leo el ADC
    Tiempo1 = millis();
    BLINK();
    }
    
  if (EstadoLCD)//ADC
  {
    if (Auxiliar1) // Hago esto para que se ejecute por unica vez
    {
      Auxiliar1 = false; // Lo paso a false para que no se ejecute de nuevo lo que esta dentro del if del ADC
      Auxiliar2 = true;  // Lo paso a True para que cuando vuelva al GPS ejecute lo que esta dentro del IF

      lcd.clear(); // Limpio la pantalla del LCD
      delay(100);

      lcd.setCursor(0, 0);
      lcd.print("LEYENDO ADC");
    }

    lcd.setCursor(0, 1);
    lcd.print(LecturaADC); // Imprimo los datos del ADC
  }
  
  else//GPS
  {
    if (Auxiliar2) // Hago esto para que se ejecute por unica vez
    {
      Auxiliar1 = true;
      Auxiliar2 = false;

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("LEYENDO GPS");
      
      delay(1000);
    }
    lcd.setCursor(0, 1);
    GPS_LCD();
  }
}


void GPS_LCD(){
  String Cadena = "0";
  if (newData)
  {
    
    float flat, flon;
    unsigned long age;
    gps.f_get_position(&flat, &flon, &age);
    
    //Serial.print("LAT=");
    //Serial.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
    flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6;
    Cadena=String(flat, 6);
    
    //imprimo Latitud en el LCD
    lcd.setCursor(0, 0);
    lcd.print("LAT ");
    
    lcd.setCursor(4, 0);
    lcd.print(Cadena);
    
    
    //Serial.print(" LON=");
    //Serial.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
    flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6;
    Cadena=String(flon, 6);

    //imprimo Longitud en el LCD
    lcd.setCursor(0, 1);
    lcd.print("LON ");
    
    lcd.setCursor(4, 1);
    lcd.print(Cadena);
    
//    Serial.print(" SAT=");
//    Serial.print(gps.satellites() == TinyGPS::GPS_INVALID_SATELLITES ? 0 : gps.satellites());
//    Serial.print(" PREC=");
//    Serial.print(gps.hdop() == TinyGPS::GPS_INVALID_HDOP ? 0 : gps.hdop());
  }
  
//  gps.stats(&chars, &sentences, &failed);
//  Serial.print(" CHARS=");
//  Serial.print(chars);
//  Serial.print(" SENTENCES=");
//  Serial.print(sentences);
//  Serial.print(" CSUM ERR=");
//  Serial.println(failed);
//  if (chars == 0)
//    Serial.println("** No characters received from GPS: check wiring **");
}

void BLINK()
{
   if (EstadoLED)
   {
      digitalWrite(2, HIGH);
      EstadoLED = false;
   }
   else
   {
      digitalWrite(2, LOW);
      EstadoLED = true;
   }
}

//interrupcion de pulsador
void IRAM_ATTR isr()
{
   if (EstadoLCD)
   {
      EstadoLCD = false;
   }
   else
   {
      EstadoLCD = true;
   }
}
