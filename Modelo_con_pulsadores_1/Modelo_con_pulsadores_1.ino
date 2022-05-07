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

void setup()
{
  Serial.begin(115200);//Comunicacion entre ESP32 y Monitor Serie

  //Declaracion de Pulsadores
  pinMode(Pulsador_1, INPUT_PULLUP);//Cambiar valor a mostrar
  pinMode(Pulsador_2, INPUT_PULLUP);//A definir
  pinMode(BLINK, OUTPUT);
}

void loop(){

  Serial.println(digitalRead(Pulsador_1));
  Serial.println(digitalRead(Pulsador_2));

  if(digitalRead(Pulsador_1)){
    Serial.println("PULSADOR 1 PRESIONADO");
    digitalWrite(BLINK, HIGH);
  }
  else if(digitalRead(Pulsador_2)){
    Serial.println("LED APAGADO");
    digitalWrite(BLINK, LOW);
  }
  else{
    Serial.println("Ningun Pulsador es oprimido");
  }
  
  delay(100);
}