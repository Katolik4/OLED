/*
 * OLED_SSD1306_128x64_I2c.h
 *
 *  Created on: 25 lis 2016
 *      Author: Piotr
 */

#ifndef OLED_SSD1306_128X64_I2C_H_
#define OLED_SSD1306_128X64_I2C_H_
#define BUFFER
extern int OLED_COM (I2C_TypeDef *I2Cx, uint8_t slave_address, uint8_t slave_data);

extern void OLED_INIT (I2C_TypeDef *I2Cx, uint8_t slave_address); // inicjacja oled / ustawienia wstepne

extern int OLED_DRAW_BUFF(I2C_TypeDef *I2Cx, uint8_t slave_address, uint8_t *buffer_pointer); //wysylanie calego buffera do oled

extern int OLED_RAM(I2C_TypeDef *I2Cx, uint8_t slave_address, uint8_t *buffer_pointer, uint8_t COLUMN, uint8_t PAGE,uint8_t COLUMN_START, uint8_t PAGE_START); //wysylanie do RAM | LICZENIE OD 0

#endif /* OLED_SSD1306_128X64_I2C_H_ */
