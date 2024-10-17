#include "../include/LCDI2C.h"

uint16_t Mil, Centena, Decena, Unidad, Decima, Centesima, milesima, diezmilesima, millonesima, diezmillonesima;
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED; //Inicializa el spinlock desbloqueado


void I2C_init(){
    ESP_ERROR_CHECK(i2c_master_init());// Inicializo el i2c
    vTaskDelay(100 / portTICK_PERIOD_MS);
}

#define retardo 1

uint8_t Send_i2c(uint8_t dato, int8_t direccion){

    int ret; //Variable para saber si se realizo la comunicacion
    i2c_cmd_handle_t cmd = i2c_cmd_link_create(); //Crea una estructura para almacenar los siguientes valores

    ret = i2c_master_start(cmd); //Inicia la comunicacion (Bit de start)
    if (ret != ESP_OK) {
        printf("a");
    }
    vTaskDelay(retardo / portTICK_PERIOD_MS);

    ret= i2c_master_write_byte(cmd, (direccion << 1) | WRITE_BIT, ACK_CHECK_EN); //Primero envia la direccion del esclavo al cual se desea comunicar y activa el chequeo de ack
    if (ret != ESP_OK) {
        printf("b");
    }
    vTaskDelay(retardo / portTICK_PERIOD_MS);

    ret = i2c_master_write(cmd, &dato, sizeof(dato), ACK_CHECK_EN); //Segundo se envia el dato y activa el chequeo de ack
    if (ret != ESP_OK) {
        printf("c");
    }
    vTaskDelay(retardo / portTICK_PERIOD_MS);

    ret = i2c_master_stop(cmd); //Detiene la comunicacion (Bit de stop)
    if (ret != ESP_OK) {
        printf("d");
    }
    vTaskDelay(retardo / portTICK_PERIOD_MS);

    ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS); //Envia toda la estructura con todo lo anterior por los pines establecido de i2c del micro
    if (ret != ESP_OK) {
        printf("e\n");
        // ESP_LOGE(TAG, "Error al instalar el driver I2C: %s", esp_err_to_name(ret));
    }
    vTaskDelay(retardo / portTICK_PERIOD_MS);

    i2c_cmd_link_delete(cmd);//Elimina la estructura

    if (ret == ESP_OK) { //Si el retorno de la funcion de envio es correcto
        // printf("TODO OK\n");
        return 1; //La funcion retorna 1
    }else{ //Sino
        // printf("Error I2C : %s!\n", esp_err_to_name(ret));//Imprime el error por monitor serial
        printf("TODO MAL\n");
        return 0; //La funcion retorna 0
        
    }                     
}

uint8_t Send_array_i2c(uint8_t arreglo[], uint8_t num_caracteres, int8_t direccion){
 
    int ret;//Variable para saber si se realizo la comunicacion
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();//Crea una estructura para almacenar los siguientes valores
    i2c_master_start(cmd);//Inicia la comunicacion (Bit de start)
    i2c_master_write_byte(cmd, (direccion << 1) | WRITE_BIT, ACK_CHECK_EN); //Primero envia la direccion del esclavo al cual se desea comunicar y activa el chequeo de ack
    
    for(uint8_t i=0; i<=num_caracteres; i++) //Recorre el arreglo para enviar de a un dato a la vez
	{
        i2c_master_write(cmd, &arreglo[i], sizeof(arreglo[i]), ACK_CHECK_EN); //Se envian todos los datos del arreglo y activa el chequeo de ack en cada envio
	}
    i2c_master_stop(cmd); //Detiene la comunicacion (Bit de stop)
    ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);//Envia toda la estructura con todo lo anterior por los pines establecido de i2c del micro. Establece un tiempo limite para no clavar el micro si no se recibe nada
    i2c_cmd_link_delete(cmd); //Elimina la estructura

    if (ret == ESP_OK) { //Si el retorno de la funcion de envio es correcto
        return 1; //La funcion retorna 1
    }else{ //Sino
        return 0; //La funcion retorna 0
        printf("Error I2C : %s!\n", esp_err_to_name(ret)); //Imprime el error por monitor serial
     } 
                       
}

void Read_i2c(uint8_t *variable, int8_t direccion){
    i2c_cmd_handle_t cmd = i2c_cmd_link_create(); //Crea una estructura para almacenar los siguientes valores
    i2c_master_start(cmd); //Inicia la comunicacion (Bit de start)
    i2c_master_write_byte(cmd, (direccion << 1) | READ_BIT, ACK_CHECK_EN); //Primero envia la direccion del esclavo al cual se desea comunicar y activa el chequeo de ack
    //i2c_master_read(cmd, variable, 1-1, ACK_VAL); //Lee el puerto i2c para guardarlo en la varible del puntero
    i2c_master_read_byte(cmd, variable, NACK_VAL); //Lee el puerto i2c para guardarlo en la varible del puntero
    i2c_master_stop(cmd); //Detiene la comunicacion (Bit de stop)
    i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS); //Envia toda la estructura con todo lo anterior por los pines establecido de i2c del micro
    i2c_cmd_link_delete(cmd); //Elimina la estructura
}

esp_err_t i2c_master_init(void)
{
    int i2c_master_port = I2C_MASTER_NUM;       //Puerto I2C a utilizar - 0 o 1
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,                //Modo maestro
        .sda_io_num = I2C_MASTER_SDA_IO,        //Pin SDA
        .sda_pullup_en = GPIO_PULLUP_ENABLE,    //Habilita pull up en SDA
        .scl_io_num = I2C_MASTER_SCL_IO,        //Pin SCL
        .scl_pullup_en = GPIO_PULLUP_ENABLE,    //Habilita pull up en SCL
        .master.clk_speed = I2C_MASTER_FREQ_HZ, //Frecuencia de clock
        //.clk_flags = 0,  /*!< Optional, you can use I2C_SCLK_SRC_FLAG_* flags to choose i2c source clock here. */
    };
    //Establece la configuración con los datos previamente cargados
    esp_err_t err = i2c_param_config(i2c_master_port, &conf);
    if (err != ESP_OK) {
        return err;
    }
    //si no hay error, instala el driver sin buffers porque es maestro y devuelve el valor de error correspondiente
    return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

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

void LCDI2C_init(){ 
    Send_i2c(0xFF, ESP_SLAVE_ADDR_LCD); //Limpio la salida de P0 a P7 produciendo el apagado de enable   
	vTaskDelay(50 / portTICK_PERIOD_MS);  // Espera para iniciar
    lcdCommand(0x02); //Posiciona el puntero en 1,1 (HOME)
    vTaskDelay(50 / portTICK_PERIOD_MS);  // Espera para iniciar
    lcdCommand(0x28); //Inicia. LCD 2 lineas, 5x7 matriz, modo 4 bits
	vTaskDelay(50 / portTICK_PERIOD_MS);  // Espera para iniciar
    lcdCommand(0x0C); //Enciende display
    vTaskDelay(50 / portTICK_PERIOD_MS);  // Espera para iniciar
    //lcdCommand(0x0E); //Cursor on
    lcdCommand(0x01); //Limpia LCD
	vTaskDelay(50 / portTICK_PERIOD_MS); //Espera
	lcdCommand(0x06); //Mueve el cursor a la derecha
    vTaskDelay(50 / portTICK_PERIOD_MS);
}

void lcd_gotoxy(unsigned char x, unsigned char y)
{
	unsigned char firstCharAdr[] ={ 0x80,0xC0,0x94,0xD4} ;//Comandos para poscicionarse Tabla 12-5
	lcdCommand (firstCharAdr[y-1] + x - 1); //Envia al display la configuracion del parametro obtenido
	vTaskDelay(2 / portTICK_PERIOD_MS);
}

void lcd_print( char * str )
{
	unsigned char i = 0;
	while (str[i] !=0) //Recorre todo el arreglo
	{
		lcdData(str[i]); //Imprime caracter por caracter
		i++ ;

	}
}

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
