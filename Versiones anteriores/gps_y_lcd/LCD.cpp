#include "LCD.h"

void LCD_Init(uint8_t Direccion,uint8_t Columnas, uint8_t Filas ){
  
  LiquidCrystal_I2C lcd(Direccion, Columnas, Filas);//crear un objeto lcd (DIRECCIÓN, Tamaño x, Tamño y)

  lcd.begin();//inicializar la pantalla lcd

  lcd.clear();//Limpiar el cursor
  
  lcd.backlight();//Encender la luz de fondo
  
  lcd.setCursor (0, 0);//poner el cursor en las coordenadas (x,y)
}
