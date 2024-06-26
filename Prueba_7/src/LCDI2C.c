/*==================[ Inclusiones ]============================================*/
#include "../include/LCDI2C.h"


/*==================[Prototipos de funciones]======================*/


/*==================[Implementaciones]=================================*/

/*========================================================================
Funcion: lcdCommand
Descripcion: Envia al LCD I2C un comando de configuracion
Parametro de entrada:
       unsigned char cmnd: Valor en binario del comando a configurar
No retorna nada
========================================================================*/
void lcdCommand(unsigned char cmnd){
    //El modulo (pcf8574) que convierte el lcd en I2C no es mas que un multiplexor
    //Este modulo recibe 8 bits por i2c los cuales se visualizan en los 8 pines de conexion que posee
    //Estos pines estan conectados fisicamente con el display 16x2 de la siguiente manera:
    
    //P0=RS P1=RW  P2=EN   P3=luz  P4=D4  P5=D5  P6=D6  P7=D7 

    //De esta forma el LCD debe trabajar en formato 4 Bits, donde es necesario pasar los 8 bits de datos dividido en 2 partes de 4 bits.
    //Como se observa, los primeros 4 bits del dato a enviar al modulo por i2c corresponden a RS, RW, EN, LUZ
    //Los 4 bits superiores corresponden al dato enviado al display.

    // _RW se deja siempre en 0, mientras que RS es 1 cuando el dato a enviar es para imprimir en la pantalla y 0 cuando el dato enviado es 
    // de configuracion.
    // _EN varia de 1 a 0 para validar los datos enviados. 
    // _P3 se mantiene siempre en 1 para dejar prendida la luz del display

    //Por lo que se explico hasta recien, Se realiza enmascaramiento para obtener los 4 bits mas significativos por un lado y los menos 
    //significativos por el otro

    //Envio los primeros 4 bits superiores de dato cmnd
    Send_i2c(((cmnd & 0xF0) & 0b11111100) | 0b00001100, ESP_SLAVE_ADDR_LCD);//RS = 0 para colocar dato 
                                                                            //RW = 0 para colocar en modo escritura
                                                                            //EN = 1 pulso en alto y en bajo para validar
                                                                            //P3=1 luz encendida
    vTaskDelay(1 / portTICK_PERIOD_MS); //espero para que se valide                               
    Send_i2c(((cmnd & 0xF0) & 0b11111000 ) | 0b00001000, ESP_SLAVE_ADDR_LCD); //EN = 0 pulso en alto y en bajo para validar dato
    vTaskDelay(1 / portTICK_PERIOD_MS); //espero para que se valide

    //Envio los 4 bits inferiores del dato cmnd
    Send_i2c(((cmnd <<4) & 0b11111100) | 0b00001100, ESP_SLAVE_ADDR_LCD);   //RS = 0 para colocar dato 
                                                                            //RW = 0 para colocar en modo escritura
                                                                            //EN = 1 pulso en alto y en bajo para validar
                                                                            //P3=1 luz encendida
    vTaskDelay(1 / portTICK_PERIOD_MS); //espero para que se valide                               
    Send_i2c(((cmnd << 4) & 0b11111000 ) | 0b00001000, ESP_SLAVE_ADDR_LCD); //EN = 0 pulso en alto y en bajo para validar dato
    vTaskDelay(1 / portTICK_PERIOD_MS); //espero para que se valide

}

/*========================================================================
Funcion: lcdData
Descripcion: Envia al LCD I2C un dato para imprimir un caracter
Parametro de entrada:
       unsigned char cmnd: Valor en binario del caracter a imprimir
No retorna nada
========================================================================*/
void lcdData(unsigned char data)
{
    //Funciona de la misma forma que LcdComand pero en este caso, RS=1 siempre ya que lo que se envia es un dato para imprimir
    //Envio los primeros 4 bits superiores de dato data
    Send_i2c(((data & 0xF0) & 0b11111101) | 0b00001101, ESP_SLAVE_ADDR_LCD);//RS = 1 para colocar dato 
                                                                            //RW = 0 para colocar en modo escritura
                                                                            //EN = 1 pulso en alto y en bajo para validar
                                                                            //P3=1 luz encendida
    vTaskDelay(1 / portTICK_PERIOD_MS);                //espero para que se valide                               
    Send_i2c(((data & 0xF0) & 0b11111001) | 0b00001001, ESP_SLAVE_ADDR_LCD);//EN = 0 pulso en alto y en bajo para validar dato
    vTaskDelay(1 / portTICK_PERIOD_MS);              //espero para que se valide


    //Envio los 4 bits inferiores del dato data
    Send_i2c(((data << 4) & 0b11111101) | 0b00001101, ESP_SLAVE_ADDR_LCD);//RS = 1 para colocar dato 
                                                                          //RW = 0 para colocar en modo escritura
                                                                          //EN = 1 pulso en alto y en bajo para validar
                                                                          //P3=1 luz encendida
    vTaskDelay(1 / portTICK_PERIOD_MS);                //espero para que se valide                               
    Send_i2c(((data << 4) & 0b11111001) | 0b00001001, ESP_SLAVE_ADDR_LCD);//EN = 0 pulso en alto y en bajo para validar dato
    vTaskDelay(1 / portTICK_PERIOD_MS);              //espero para que se valide
}

/*========================================================================
Funcion: LCDI2C_init
Descripcion: Inicializa y configura el LCD 
No retorna nada
========================================================================*/
void LCDI2C_init(){
    
    Send_i2c(0xFF, ESP_SLAVE_ADDR_LCD); //Limpio la salida de P0 a P7 produciendo el apagado de enable
	vTaskDelay(30 / portTICK_PERIOD_MS);  // Espera para iniciar
    lcdCommand(0x02); //Psociona el puntero en 1,1 (HOME)
    lcdCommand(0x28); //Inicia. LCD 2 lineas, 5x7 matriz, modo 4 bits
	lcdCommand(0x0C); //Enciende display
    //lcdCommand(0x0E); //Cursor on
    lcdCommand(0x01); //Limpia LCD
	vTaskDelay(2 / portTICK_PERIOD_MS); //Espera
	lcdCommand(0x06); //Mueve el cursor a la derecha
}

/*========================================================================
Funcion: lcd_gotoxy
Descripcion: Posiciona el cursor en la posicion indicada
Parametro de entrada:
        unsigned char x: Posicion en x donde se desea colocar el cursor
        unsigned char y: Posicion en y donde se desea colocar el cursor
No retorna nada
========================================================================*/
void lcd_gotoxy(unsigned char x, unsigned char y)
{
	unsigned char firstCharAdr[] ={ 0x80,0xC0,0x94,0xD4} ;//Comandos para poscicionarse Tabla 12-5
	lcdCommand (firstCharAdr[y-1] + x - 1); //Envia al display la configuracion del parametro obtenido
	vTaskDelay(2 / portTICK_PERIOD_MS);
}

/*========================================================================
Funcion: lcd_print
Descripcion: Funcion para imprimir cadena de caracteres en el lcd
Parametro de entrada:
        char * str: recibe el puntero del string a imprimir
No retorna nada
========================================================================*/
void lcd_print( char * str )
{
	unsigned char i = 0;
	while (str[i] !=0) //Recorre todo el arreglo
	{
		lcdData(str[i]); //Imprime caracter por caracter
		i++ ;

	}
}

//declaro variables globales para trabajar con ellas siempre
uint16_t Mil, Centena, Decena, Unidad, Decima, Centesima, milesima, diezmilesima, millonesima, diezmillonesima;

/*	Funcion que recibe un numero flotante, desplaza la coma de la 
	variable de tipo flotante y guarda cada unidad En forma de 
	enteros y despues los imprime
	
	esta funcion solo guarda las decenas, unidades, decimales y
	centesimas*/
void Print_Float_LCD(float Flotante, int decimas)
{

	if (Flotante<0){
		lcd_print("-");
		Flotante = Flotante*-1;
	}

    /*	si flotante no es mayor a 999 Mil es cero,
		caso contrario obtengo el valor y lo guardo
		en la variable Mil*/
	if (Flotante>999)
	{
		/*
		113.14/10 --> 1.1314
		Decena = 1 
		*/
		Mil = Flotante/1000;
		/*
		113.14 - 1*100 --> 13.14 
		*/
		Flotante = Flotante - (Mil*1000);

		lcdData(Mil+48);//Desplazo 48 valores por el codigo ascci
	}
	else
	{
		Mil=0;
	}

	/*	si flotante no es mayor a 99 Centena es cero,
		caso contrario obtengo el valor y lo guardo
		en la variable Centena*/
	if (Flotante>99)
	{
		/*
		113.14/10 --> 1.1314
		Decena = 1 
		*/
		Centena = Flotante/100;
		/*
		113.14 - 1*100 --> 13.14 
		*/
		Flotante = Flotante - (Centena*100);

		lcdData(Centena+48);//Desplazo 48 valores por el codigo ascci
	}
	else
	{
		Centena=0;
	}
	
	/*	si flotante no es mayor a 9 Decena es cero,
		caso contrario obtengo el valor y lo guardo
		en la variable decena*/
	if (Flotante>9)
	{
		/*
		13.14/10 --> 1.314
		Decena = 1 
		*/
		Decena = Flotante/10;
		/*
		13.14 - 1*10 --> 3.14 
		*/
		Flotante = Flotante - (Decena*10);

		lcdData(Decena+48);//Desplazo 48 valores por el codigo ascci
	}
	else
	{
		Decena=0;
	}
	/*
		Unidad --> 3
	*/
	Unidad = Flotante;

	lcdData(Unidad+48);//Desplazo 48 valores por el codigo ascci

	//decima
	if(decimas>0){
		Flotante=Flotante/10;
		lcd_print(".");
		Flotante = Flotante*10 - Unidad;
		Decima = Flotante*10;
		lcdData(Decima+48);//Desplazo 48 valores por el codigo ascci
	}
	//centesima
	if (decimas > 1){
		
		Flotante = Flotante*10 - Decima;
		Centesima = Flotante*10;
		lcdData(Centesima+48);//Desplazo 48 valores por el codigo ascci
	}
	//milesima
	if (decimas > 2){
		Flotante = Flotante*10 - Centesima;
		milesima = Flotante*10;
		lcdData(milesima+48);//Desplazo 48 valores por el codigo ascci
	}
	//diezmilesima
	if (decimas > 3){
		Flotante = Flotante *10 - milesima;
		diezmilesima = Flotante*10;
		lcdData(diezmilesima+48);//Desplazo 48 valores por el codigo ascci
	}
	//millonesima
	if (decimas > 4){
		Flotante = Flotante *10 - diezmilesima;
		millonesima = Flotante*10;
		lcdData(millonesima+48);//Desplazo 48 valores por el codigo ascci
	}
	//diezmillonesima
	if (decimas > 5){
		Flotante = Flotante *10 - millonesima;
		diezmillonesima =Flotante*10;
		lcdData(diezmillonesima+48);//Desplazo 48 valores por el codigo ascci
	}
}

void BorrarPantalla(void){
	lcdCommand(0x01);
	vTaskDelay(10 / portTICK_PERIOD_MS);
}
