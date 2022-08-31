#include "../include/Uart.h"

//========================= Función principal =========================

static const int RX_BUF_SIZE = 2024;
// Configuracion de la UART 0
/*void init(void)
{
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    uart_driver_install(UART_NUM_0, RX_BUF_SIZE*2,0,0,NULL,0);
    uart_param_config(UART_NUM_0, &uart_config);
    uart_set_pin(UART_NUM_0,TXD_PIN,RXD_PIN, UART_PIN_NO_CHANGE,UART_PIN_NO_CHANGE);
}*/

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
    uart_driver_install(UART_NUM_2, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_2, &uart_config_2);
    uart_set_pin(UART_NUM_2, TXD_PIN2, RXD_PIN2, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}
//========================= Funciones =========================
void rx_uart(void *arg)
{
    uint8_t *data = (uint8_t *)malloc(RX_BUF_SIZE + 1);
    const char *str = "GPGGA";
    char *p = NULL;
    char str1[100];
    int j = 0;

    // uint8_t* data = (uint8_t*)malloc(RX_BUF_SIZE+1);
    char *str2 = (char *)malloc(sizeof(str1));
    while (true)
    {
        const int rxbytes = uart_read_bytes(UART_NUM_2, data, RX_BUF_SIZE, 1000 / portTICK_RATE_MS);
        if (rxbytes > 0)
        {
            p = strstr((const char *)data, str);
            if (p)
            {
                for (int i = 0; p[i] != '\n'; i++)
                {
                    str2[j] = p[i];
                    j++;
                }
                str2[j + 1] = '\n';
                const int len = j + 2;
                data[rxbytes] = 0;
                uart_write_bytes(UART_NUM_0, (const char *)str2, len);
                getstring((char*)str2);
            }
            /*data[rxbytes]= 0;
            uart_write_bytes(UART_NUM_0, (const char*)data,rxbytes); */
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    free(data);
    free(str2);
}

void getstring(char *str)
{

    int i = 0;
    int cnt = 0;
    int k = 0;
    int n=0;
    char new[50];
    while (cnt < 2)
    {
        if (str[i] == ',')
        {
            cnt++;
        }
        i++;
    }
    int j = 0;
    str[29] = '\n';
    for (j = i; j < i + 24; j++)
    {
        new[k] = str[j];
        k++;
    }
    new[k] = '\n';
    lcd_gotoxy(1,1);
    while(new[n] != '\n')
    {
        lcdData(new[n]);
        n++;
    }
      lcd_gotoxy(1,2);
    for(int p=n+1;new[p]!= '\n';p++)
    {
       lcdData(new[p]);
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
}
