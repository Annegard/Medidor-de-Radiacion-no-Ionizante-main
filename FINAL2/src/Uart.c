#include "../include/Uart.h"

//========================= Función principal =========================



// Configuracion de la UART 0
void init_uart_0(void)
{
    uart_config_t uart_config = {
        .baud_rate = 115200
        ,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    uart_driver_install(UART_NUM_0, 1024*3,1024*3,0,NULL,0);
    uart_param_config(UART_NUM_0, &uart_config);
    uart_set_pin(UART_NUM_0,TXD_PIN,RXD_PIN, UART_PIN_NO_CHANGE,UART_PIN_NO_CHANGE);
}

// Configuracion de la UART 2
void Uart_init2(void)
{
    const uart_config_t uart_config_2 = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    uart_driver_install(UART_NUM_2, 1024*3,1024*3, 0, NULL, 0);
    uart_param_config(UART_NUM_2, &uart_config_2);
    uart_set_pin(UART_NUM_2, TXD_PIN2, RXD_PIN2, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}
//========================= Funciones =========================

