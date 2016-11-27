/*
 * OLED_SSD1306_128x64_I2C.c
 *
 *  Created on: 25 lis 2016
 *      Author: Piotr
 */
#include "stm32f10x.h"

#define I2C_SPEED                   50000                     /*!< I2C Speed */
#define I2C1_SSD1306_SLAVE_ADDRESS8 0x78                      // 8 bit slave address (write)
#define I2C_TIMEOUT                 100000

int OLED_COM (I2C_TypeDef *I2Cx, uint8_t slave_data) // wysylanie danych do oled po I2C
{
	// Sends I2C data over I2Cx:
	//  1) Sends Start Condition. Checks for I2C EV5
	//  2) Sends 7 bit address & checks for EV6
	//  3) Sends 8 bit command byte (0x00) & checks for EV8
	//  4) Sends 8 bits (1 byte) of data & checks for EV8
	//  5) Sends Stop Condition
	    int TimeOut;

	    #define COMMAND_BYTE 0x00

	    /* Send I2C1 START condition */
	    I2C_GenerateSTART(I2Cx, ENABLE);

	    /* Test on I2C1 EV5 and clear it */
	    TimeOut = I2C_TIMEOUT;
	    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
	    {
	        TimeOut--;
	        if (TimeOut == 0){
	            turn_on_error_led_pin();       // Error LED
	            return 0;
	        }
	    }

	    /* Send SSD1306 7 bit slave Address for write. Check to make sure ACK received */
	    I2C_Send7bitAddress(I2Cx, I2C1_SSD1306_SLAVE_ADDRESS8, I2C_Direction_Transmitter);

	    //Test on I2C1 EV6 and clear it
	    TimeOut = I2C_TIMEOUT;
	    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
	    {
	        TimeOut--;
	        if (TimeOut == 0){
	            // Send I2C1 STOP Condition
	            I2C_GenerateSTOP(I2Cx, ENABLE);
	            turn_on_error_led_pin();        // Error LED

	            return 0;
	        }
	    }

	    I2C_SendData(I2Cx, COMMAND_BYTE);
	    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)){ // Wait for EV8
	        TimeOut--;
	        if (TimeOut == 0){
	            // Send I2C1 STOP Condition
	            I2C_GenerateSTOP(I2Cx, ENABLE);
	            turn_on_error_led_pin();        // Error LED

	            return 0;
	        }
	    }

	    I2C_SendData(I2Cx, slave_data);
				while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)){ // Wait for EV8
					TimeOut--;
					if (TimeOut == 0){
						// Send I2C1 STOP Condition
						I2C_GenerateSTOP(I2Cx, ENABLE);
						turn_on_error_led_pin();        // Error LED

						return 0;
					}
				}

	    I2C_GenerateSTOP(I2Cx, ENABLE);
	    return 1;
}

void OLED_INIT (I2C_TypeDef *I2Cx)// inicjacja oled / ustawienia wstepne
{
	OLED_COM(I2Cx,0xAE);   						// wylacz oled

	OLED_COM(I2Cx,0x40);   					// linia #0

																/*TRYB ADRESOWANIA*/
	OLED_COM(I2Cx,0x20);   					// Memory Addressing Mode
	OLED_COM(I2Cx,0x00);   					// 0-00 - Horizontal /0x01 - Vertical /0x02 - Page
																/* KONTRAST*/
	OLED_COM(I2Cx,0x81);   					// Kontrast
	OLED_COM(I2Cx,0xCF);   					// 0-255
																/*USTAWIENIA KIERUNKU WYSWIETLANIA*/
	OLED_COM(I2Cx,0xA0);   					// Segremap - 0xA1/0xA0 (odwracanie w kierunku poziomym)
	OLED_COM(I2Cx,0xC0);   					// COMSCAN DEC 0xC8/ 0xC0 (odwracanie w kierunku pionowym)
																/* USTAWIENIA WYSWIETLANIA Z RAM*/
	OLED_COM(I2Cx,0xA6);   					// Wyswietlanie normalne 0xA6/0xA7 (odwrocone kolory)
	OLED_COM(I2Cx,0xA4);   					// wyswietlaj z RAM - 0xA4 / 0xA5 - all on
																/*RESZTA USTAWIEN*/
	OLED_COM(I2Cx,0xA8);   					// Set Multipelx 0xA8
	OLED_COM(I2Cx,0x3F);   						// 1/64 Duty Cycle

	OLED_COM(I2Cx,0xD3);   					// Display Offset 0xD3
	OLED_COM(I2Cx,0x00);   					// no offset

	OLED_COM(I2Cx,0xD5);   					// Display Clk Div 0xD5
	OLED_COM(I2Cx,0x80);   					// Recommended resistor ratio 0x80

	OLED_COM(I2Cx,0xD9);   					// Set Precharge 0xD9
	OLED_COM(I2Cx,0xF1);   						//

	OLED_COM(I2Cx,0xDA);   					// Set COM Pins (remapowanie)
	OLED_COM(I2Cx,0x12);   					//

	OLED_COM(I2Cx,0xDB);   					// Set VCOM Detect 0xDB
	OLED_COM(I2Cx,0x40);   					//

	OLED_COM(I2Cx,0x8D);   					//Charge Pump 0x8D
	OLED_COM(I2Cx,0x14);   					// 0x14 - wlaczona

																/*WLACZ WYSWIETLACZ*/
	OLED_COM(I2Cx,0xAF);   						// Turn on/off oled AF/AE

}

int OLED_DRAW_BUFF(I2C_TypeDef *I2Cx, uint8_t *buffer_pointer) //wysylanie calego buffora do oled
{
   #define SSD1306_COLUMNADDR  0x21
   #define SSD1306_PAGEADDR    0x22
   #define DATA_BYTE           0x40

   int TimeOut;

  OLED_COM(I2Cx, SSD1306_COLUMNADDR);
  OLED_COM(I2Cx, 0x00);            // Page Start address
  OLED_COM(I2Cx, 0x7F);            // Page end address



  OLED_COM(I2Cx, SSD1306_PAGEADDR);
  OLED_COM(I2Cx, 0x00);            // Page Start address
  OLED_COM(I2Cx, 0x07);            // Page end address

  OLED_COM(I2Cx, 0x40);

   uint8_t x, y;


   /* Send I2C1 START condition */
   I2C_GenerateSTART(I2Cx, ENABLE);

   /* Test on I2C1 EV5 and clear it */
   TimeOut = I2C_TIMEOUT;
   while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
   {
       TimeOut--;
       if (TimeOut == 0){
           turn_on_error_led_pin();       // Error LED
           return 1;
       }
   }

   /* Send SSD1306 7 bit slave Address for write. Check to make sure ACK received */
   I2C_Send7bitAddress(I2Cx, I2C1_SSD1306_SLAVE_ADDRESS8, I2C_Direction_Transmitter);

   //Test on I2C1 EV6 and clear it
   TimeOut = I2C_TIMEOUT;
   while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
   {
       TimeOut--;
       if (TimeOut == 0){
           // Send I2C1 STOP Condition
           I2C_GenerateSTOP(I2Cx, ENABLE);
           turn_on_error_led_pin();        // Error LED

           return 2;
       }
   }
   TimeOut = I2C_TIMEOUT;
   I2C_SendData(I2Cx, DATA_BYTE);
   while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)){ // Wait for EV8
       TimeOut--;
       if (TimeOut == 0){
           // Send I2C1 STOP Condition
           I2C_GenerateSTOP(I2Cx, ENABLE);
           turn_on_error_led_pin();        // Error LED

           return 2;
       }
   }



   for(y=0; y<8; y++){
       for(x=0; x<128; x++){
           //I2C_SendData(I2Cx, const_global_display_buffer[(128*y)+x]);
           TimeOut = I2C_TIMEOUT;
       	I2C_SendData(I2Cx, buffer_pointer[(128*y)+x]);
           while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)){ // Wait for EV8
               TimeOut--;
               if (TimeOut == 0){
                   // Send I2C1 STOP Condition
                   I2C_GenerateSTOP(I2Cx, ENABLE);
                   turn_on_error_led_pin();        // Error LED

                   return 2;
               }
           }
       }
   }
   I2C_GenerateSTOP(I2Cx, ENABLE);
   return 1;
}

int OLED_RAM(I2C_TypeDef *I2Cx, uint8_t *buffer_pointer, uint8_t COLUMN, uint8_t PAGE,uint8_t COLUMN_START, uint8_t PAGE_START) //wysylanie do RAM | LICZENIE OD 0
{
   #define SSD1306_COLUMNADDR  0x21
   #define SSD1306_PAGEADDR    0x22
   #define DATA_BYTE           0x40

   int TimeOut;
   uint8_t COLUMN_KONIEC = COLUMN_START+COLUMN;
   uint8_t PAGE_KONIEC = PAGE_START+PAGE;

   OLED_COM(I2Cx, SSD1306_COLUMNADDR);
   OLED_COM(I2Cx,COLUMN_START);            // Page Start address
   OLED_COM(I2Cx,COLUMN_KONIEC);            // Page end address



   OLED_COM(I2Cx, SSD1306_PAGEADDR);
   OLED_COM(I2Cx, PAGE_START);            // Page Start address
   OLED_COM(I2Cx, PAGE_KONIEC);            // Page end address
   uint8_t x, y;


   /* Send I2C1 START condition */
   I2C_GenerateSTART(I2Cx, ENABLE);

   /* Test on I2C1 EV5 and clear it */
   TimeOut = I2C_TIMEOUT;
   while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
   {
       TimeOut--;
       if (TimeOut == 0){
           turn_on_error_led_pin();       // Error LED
           return 1;
       }
   }

   /* Send SSD1306 7 bit slave Address for write. Check to make sure ACK received */
   I2C_Send7bitAddress(I2Cx, I2C1_SSD1306_SLAVE_ADDRESS8, I2C_Direction_Transmitter);

   //Test on I2C1 EV6 and clear it
   TimeOut = I2C_TIMEOUT;
   while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
   {
       TimeOut--;
       if (TimeOut == 0){
           // Send I2C1 STOP Condition
           I2C_GenerateSTOP(I2Cx, ENABLE);
           turn_on_error_led_pin();        // Error LED

           return 2;
       }
   }
   TimeOut = I2C_TIMEOUT;
   I2C_SendData(I2Cx, DATA_BYTE);
   while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)){ // Wait for EV8
       TimeOut--;
       if (TimeOut == 0){
           // Send I2C1 STOP Condition
           I2C_GenerateSTOP(I2Cx, ENABLE);
           turn_on_error_led_pin();        // Error LED

           return 2;
       }
   }

   for(y=0; y<PAGE+1; y++){
       for(x=0; x<COLUMN+1; x++){
           //I2C_SendData(I2Cx, const_global_display_buffer[(128*y)+x]);
           TimeOut = I2C_TIMEOUT;
       	I2C_SendData(I2Cx, buffer_pointer[((COLUMN+1)*y)+x]);
           while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)){ // Wait for EV8
               TimeOut--;
               if (TimeOut == 0){
                   // Send I2C1 STOP Condition
                   I2C_GenerateSTOP(I2Cx, ENABLE);
                   turn_on_error_led_pin();        // Error LED

                   return 2;
               }
           }
       }
   }
   I2C_GenerateSTOP(I2Cx, ENABLE);
   return 1;

}

