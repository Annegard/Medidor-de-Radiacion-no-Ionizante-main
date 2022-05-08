#define SCL 15
#define SDA 14
#define Addres 0x27
#define TRUE  1
#define FALSE 0
// INSTRUCCIONES
#define LINEA1    0x80
#define LINEA2    0xC0
#define CLEAR   0x01
#define CMD   0
#define DATO  1

void I2C_Init();
void Print_LCD(char *s);
void Init_LCD();
void Enable_LCD(uint8_t data);
void Write_LCD(uint8_t type, uint8_t data);
void enviarStartBit(void);
void enviarStoptBit(void);
void enviarBit(char Bit);
void enviarByte(char Byte);
void enviarDeviceAddress(char deviceAddress, char RW);
char recibirAck(void);
void softReset(void);

void setup(){
   I2C_Init();
   Init_LCD();
   
   Print_LCD("Hola Mundo");
}

void loop(){
   //Codigo
}

void I2C_Init(){
  pinMode(SDA, OUTPUT);
  pinMode(SCL, OUTPUT);
  //hacemos un soft reset de la memoria
  softReset();
   
  enviarStartBit();//envio bit de start
  enviarDeviceAddress(Addres,1);//DIRECCION DEL ESCLAVO EN MODO LECTURA
  if(recibirAck()== FALSE) softReset();
}
/*funcion que lee la palabra enviada y la escribe caracter por caracter ya que no se puede
enviar una cadena de texto directamente(sacado de internet)
Ej de imprimir datos sin la funcion print:
*/
void Print_LCD(char *s)
{
  while(*s)
  {
    Write_LCD(DATO, *s);//imprimo el caracter
    s++;//cambio la posicion en la que guardo el caracter
  }
}


void Init_LCD()
{
  //Cumplo con los siguientes pasos (en el datasheet p�g 46)
  delay(100);//espero 100 ms despues de encendido el LCD

  //cargo un 3 en el bus de datos (Ej: 0011****) y lo envio 3 veces
  delay(100);
  Enable_LCD(0b00111000);
  delay(100);
  Enable_LCD(0b00111000);
  delay(100);
  Enable_LCD(0b00111000);
  
  //cargo un 2 en el bus de datos (Ej: 0010****) y lo envio para
  //indicarle al LCD que la comunicacion sera a partir de un BUS
  //de 4 pines
  Enable_LCD(0x20);
  delay(100);
  
  //cargo los siguientes comandos(en el datasheet p?g 24 y 25):
  
  Write_LCD(CMD ,0x2C);//function set(el C esta para habilitar las dos lineas del display)
  Write_LCD(CMD ,0x08);//display ON/OFF(apagar display, apagar el cursor y apagar el blink del cursor)
  Write_LCD(CMD ,0x01);//clear display
  Write_LCD(CMD ,0x06);//entry mode set(la posicion del cursor incrementa en 1)
  Write_LCD(CMD ,0x0C);//display ON/OFF(encender el display)
}

//pinto hacer un enable global para ahorrar lineas de codigo
void Enable_LCD(uint8_t data)
{
   uint8_t auxiliar;

   auxiliar = (data & 0b11111011);
   enviarByte(auxiliar);
  delay(100);
   auxiliar = (data & 0b11111111);
   enviarByte(auxiliar);
  delay(100);
   auxiliar = (data & 0b11111011);
   enviarByte(auxiliar);
  delay(100);
}

/*Funcion de escritura, en la primer variable determina si el valor recibido es
un dato o un comando, en la seguna la palabra a enviar, esta la parte en 2 y la envia
en 2 veces*/
void Write_LCD(uint8_t type, uint8_t data)
{
   uint8_t auxiliar;
  //Si "type" es un 1 pongo RS en 1 para enviar datos, caso contrario pongo
  //RS en 0 para enviar comandos
  if (type){
      auxiliar = (data & 0xF0)|0b00000001;
     enviarByte(auxiliar);
      Enable_LCD(auxiliar);
      delay(100);
      auxiliar = ((data<<4) & 0xF0)|0b00000001;
     enviarByte(auxiliar);
      Enable_LCD(auxiliar);
      delay(100);
   }
  else{
      auxiliar = (data & 0xF0)|0b00000001;
      enviarByte(auxiliar);
      Enable_LCD(auxiliar);
      delay(100);
      auxiliar = ((data<<4) & 0xF0)|0b00000001;
      enviarByte(auxiliar);
      Enable_LCD(auxiliar);
      delay(100);
   }

}

/*funcion para enviar el bit de start necesario para iniciar cualquier comunicacion I2C*/
void enviarStartBit(void)
{
   //Almacena la configuracion de SDA_Config tal como la recibe la funcion
   //char SDA_Config_oldValue = SDA_Config;
    
   //configuracion BIT START
   pinMode(SDA, OUTPUT);//SDA como salida
   digitalWrite(SCL, 0);
   delay(2);
   digitalWrite(SDA,1);//SDA HIGH
   delay(2); 
   digitalWrite(SCL, 1); //CLK HIGH
   delay(2); //delay para evitar errores de lectura en los cambios de tension
   digitalWrite(SDA,0);
   delay(2);
   digitalWrite(SCL, 0);//pongo en cero el reloj para enviar datos
   delay(2);
    
   //SDA_Config = SDA_Config_oldValue;

   return; //volver al main
}

/*funcion para enviar el bit de stop 
 * necesario para terminar cualquier comunicacion I2C*/
void enviarStoptBit(void){
   //Almacena la configuracion de SDA_Config tal como la recibe la funcion
   //char SDA_Config_oldValue = SDA_Config;
    
   pinMode(SDA, OUTPUT);//SDA como salida
   digitalWrite(SCL, 0);
   delay(2);//delay para estabilizacion de tensiones

   digitalWrite(SDA,0);//coloco el bit en el pin SDA
   delay(2);
   digitalWrite(SCL, 1);//fijo que envie el bit poniendo el reloj a uno y despues a cero
   delay(2);
    digitalWrite(SDA,1);//coloco el bit en el pin SDA
   delay(2);
   digitalWrite(SCL, 0);
   delay(2);//delay para estabilizacion de tensiones
    
   //SDA_Config = SDA_Config_oldValue;
   return;
}

/*Funcion para enviar bit a bit*/
void enviarBit(char Bit)
{
   //Almacena la configuracion de SDA_Config tal como la recibe la funcion
   //char SDA_Config_oldValue = SDA_Config;
    
   pinMode(SDA, OUTPUT);//SDA como salida
   digitalWrite(SCL, 0);

   delay(2);//delay para estabilizacion de tensiones
   digitalWrite(SDA,Bit);//coloco el bit en el pin SDA
   delay(2);
   digitalWrite(SCL, 1);//fijo que envie el bit poniendo el reloj a uno y despues a cero
   delay(2);
   digitalWrite(SCL, 0);//pongo para enviar el bit
   delay(2);
    
   //devuelvo el valor de la salida al que tenia antes de 
   //llamar a la funcion para evitar modificarla
   //SDA_Config = SDA_Config_oldValue;
    
   return; //volver al main
}

/*Funcion para enviar un byte*/
void enviarByte(char Byte)
{
    for(char i=0; i < 8; i++)
    {
        //Funcion de rotacion para que el MSB quede en el LSB
        Byte = (Byte << 1) | (Byte >> 7);
        enviarBit(Byte);
    }
    return;
}

/*Funcion para enviar la direccion del esclavo y el modo de comunicacion (R/W)*/
void enviarDeviceAddress(char deviceAddress, char RW)
{
   enviarByte((deviceAddress << 1) | RW);

   return;
}

/*funcion para recibir el ACK y devolver un true(ACK) o false(NOT ACK)*/
char recibirAck(void)
{
   //char SDA_Config_oldValue = SDA_Config;
   pinMode(SDA, INPUT_PULLUP);
   digitalWrite(SCL, 1);//la lectura la hace en el ciclo alto del reloj(leer pdf)
   delay(2);
   uint8_t ACK = digitalRead(SDA); // Leo ACK del esclavo
   digitalWrite(SCL, 0);
   delay(2);
    
   //SDA_Config = SDA_Config_oldValue;
    
    if(ACK==0) return TRUE; //devuelve el TRUE
    else if(ACK==1) return FALSE; //devuelve el FALSE
}

/*funcion para resetear el esclavo cuando este no responde*/
void softReset(void)
{
   enviarStartBit();
    
   for(char i=0; i<10; i++)
   {
      digitalWrite(SCL, 1);
      delay(2);
      digitalWrite(SCL, 0);
      delay(2);
   }
    
   enviarStartBit();
   enviarStoptBit();
    
   return;
}
