#include "../include/Uart.h"

static const int RX_BUF_SIZE = 1024;

//========================= Funciones =========================

// Configuracion del puerto UART 2
void Uart_init2(void)
{
    /*
    Primero se instala el controlador UART.
    La función uart_driver_install() recibe los siguientes parametros:

        uart_driver_install(uart_port_t, int rx_buffer_size, int tx_buffer_size, int queue_size, QueueHandle_t *uart_queue, int intr_alloc_flags);

        @uart_port_t, el UART_NUM que está utilizando, UART2 en este ejemplo
        @int rx_buffer_size, el tamaño del búfer RX, 1024 (RX_BUF_SIZE) bytes en este ejemplo
        @int tx_buffer_size, el tamaño del búfer TX, 0 en este ejemplo
        @int queue_size, el tamaño de la cola UART, 0 en este caso, ya que no estamos usando ninguna cola en este tutorial
        @QueueHandle_t *uart_queue, la cola UART, NULL en este ejemplo ya que no estamos usando la cola
        @int intr_alloc_flags, The interrupt FLAG, 0 en este ejemplo ya que no estamos usando interrupciones
    */
    uart_driver_install(UART_NUM_2, RX_BUF_SIZE * 2, 0, 0, NULL, 0);

    /*
    Luego configuraremos los parámetros de la UART.
    La configuración utilizada es 8-N-1 con una velocidad en baudios de 9600. El reloj de fuente UART está configurado en APB.
    */

    // Guardo la configuracion en una variable y despues la añado como parametro
    const uart_config_t uart_config_2 = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };

    uart_param_config(UART_NUM_2, &uart_config_2);

    /*
    Finalmente configuraremos los pines para la UART.
    La función uart_set_pin() toma los siguientes parámetros

        uart_set_pin(uart_port_t, int tx_io_num, int rx_io_num, int rts_io_num, int cts_io_num);

        @uart_port_t, el UART_NUM que está utilizando, UART2 en este ejemplo
        @int tx_io_num, el número de PIN de TX, GPIO_NUM_17 en este ejemplo
        @int rx_io_num, el número de pin RX, GPIO_NUM_16 en este ejemplo
        @int rts_io_num y @int cts_io_num, los números pin de RTS y CTS. No usamos control de flujo de hardware, por lo que estos parámetros se establecen en UART_PIN_NO_CHANGE (-1).
    */

    uart_set_pin(UART_NUM_2, TXD_PIN2, RXD_PIN2, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

/*RX_Uart_Available
    Funcion que revisa si existen datos en el buffer y, de haberlos, los compara con el parametro recibido,
    de ser iguales devuelve un true develando que se encuentra la trama buscada y asi saber si realizar o no
    la lectura de la trama de datos entera.
*/
bool rx_uart_Available(char *str, char *buffertocopyinto)
{
    /*  Acotaciones sobre las funciones malloc() y free()
        Los variables y vectores en C ocupan un tamaño prefijado, no pueden variarlo durante la ejecución del programa.
        Por medio de punteros se puede reservar o liberar memoria dinámicamente.

        La función malloc sirve para solicitar un bloque de memoria del tamaño suministrado como parámetro.
        Devuelve un puntero a la zona de memoria concedida.

        Cuando una zona de memoria reservada con malloc ya no se necesita, puede ser liberada mediante la función free.

        http://labsopa.dis.ulpgc.es/fso/cpp/intro_c/introc75.htm
    */

    // Creo el puntero *data y con malloc se mide el tamaño maximo del buffer de datos que se puede recibir del puerto UART, le suma 1 para
    // que nunca este lleno al 100% y le asigna el bloque de memoria solicitado por malloc a la direccion del puntero *data.
    uint8_t *data = (uint8_t *)malloc(*str);

    /*
    Aquí primero crearemos un búfer para almacenar los datos recibidos (rxbytes). Recibiremos los datos mediante la función
    uart_read_bytes la cual toma los siguientes parámetros:

        int uart_read_bytes(uart_port_t uart_num, uint8_t *buf, uint32_t length, TickType_t ticks_to_wait);

        @uart_port_t, el UART_NUM que está utilizando es el del puerto UART2
        @uint8_t *buf, el puntero al búfer donde se guardarán los datos, *data en este caso
        @uint32_t length, la longitud del búfer, 1024 bytes en este caso
        @TickType_t ticks_to_wait, el tiempo de espera para que lleguen los datos, 500 ms para este caso
    */
    const int rxbytes = uart_read_bytes(UART_NUM_2, data, RX_BUF_SIZE, 100 / portTICK_RATE_MS);

    int j = 0; // contador de caracteres de la trama

    /*El famoso AVAILABLE*/
    if (rxbytes > 0) // Si rxbytes es distinto de cero ejecuta el siguiente bloque de codigo
    {
        // declaro un puntero de momento vacio donde se guardara la posicion de la trama de datos GGA dentro del puntero *data
        char *p = NULL;

        /*LOCALIZACION DE GPGGA EN LA TRAMA DE DATOS RECIBIDA*/
        /*
        La funcion strstr() localiza una subcadena dentro de una cadena

        La función retorna un puntero a la cadena encontrada, o un puntero nulo si no se encontró la cadena.
        Si s2 apunta a una cadena de longitud cero, la función retorna s1.
        */
        p = strstr((const char *)data, str); // Localizo la posicion de la palabra definida en str en el puntero *data y la guardo en *p

        if (p) // Si *p es distinto de NULL ejecuto el codigo a continuacion
        {
            // Recorro *p hasta detectar un salto de linea y guardo cada caracter en str2
            for (int i = 0; p[i] != '\n'; i++)
            {
                buffertocopyinto[j] = p[i]; // Guarda cada caracter en el array str2
                j++;                        // Cuenta el largo de la trama
            }
            buffertocopyinto[j + 1] = '\n'; // Le añado un salto de linea al final de la trama

            data[rxbytes] = 0; // se esta colocando un buffer dentro de un puntero y se lo iguala a cero, desconozco si eso vacia la
                               // el bloque de memoria donde se ubica el puntero, el buffer o ambos

            
        }
        return true; //Si se encontraron datos la funcion devolvera un verdadero
    }
    return false;//si no se hallaron datos en el buffer devuelve un false
}