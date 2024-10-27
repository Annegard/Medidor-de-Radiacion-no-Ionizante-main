#ifndef LCDI2C_H_
#define LCDI2C_H_

    #include "driver/i2c.h"
    #include "esp_log.h"

    #define ESP_SLAVE_ADDR_LCD 0x27                /*!< ESP32 slave address, you can set any 7bit value */

    #define DATA_LENGTH 512                     /*!< Data buffer length of test buffer */
    #define RW_TEST_LENGTH 128                  /*!< Data length for r/w test, [0,DATA_LENGTH] */
    #define DELAY_TIME_BETWEEN_ITEMS_MS 1000    /*!< delay time between different test items */

    #define I2C_MASTER_SCL_IO 21                /*!< gpio number for I2C master clock */
    #define I2C_MASTER_SDA_IO 22                /*!< gpio number for I2C master data  */
    #define I2C_MASTER_NUM I2C_NUM_1            /*!< I2C port number for master dev */
    // #define I2C_MASTER_FREQ_HZ 1000000          /*!< I2C master clock frequency */
    #define I2C_MASTER_FREQ_HZ 50000          /*!< I2C master clock frequency */
    #define I2C_MASTER_TX_BUF_DISABLE 0         /*!< I2C master doesn't need buffer */
    #define I2C_MASTER_RX_BUF_DISABLE 0         /*!< I2C master doesn't need buffer */

    #define WRITE_BIT I2C_MASTER_WRITE          /*!< I2C master write */
    #define READ_BIT I2C_MASTER_READ            /*!< I2C master read */
    #define ACK_CHECK_EN 0x1                    /*!< I2C master will check ack from slave*/
    #define ACK_CHECK_DIS 0x0                   /*!< I2C master will not check ack from slave */
    #define ACK_VAL 0x0                         /*!< I2C ack value */
    #define NACK_VAL 0x1                        /*!< I2C nack value */

    

    void LCDI2C_init();
    void I2C_init();
    void lcd_print( char * str );
    void Print_Float_LCD(float Flotante, int decimas);
    void BorrarPantalla(void);
    void lcd_gotoxy(unsigned char x, unsigned char y);

#endif