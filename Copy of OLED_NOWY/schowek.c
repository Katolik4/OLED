int i2c_send_command2(I2C_TypeDef *I2Cx, uint8_t slave_address, uint8_t slave_data1, uint8_t slave_data2){
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

    I2C_SendData(I2Cx, COMMAND_BYTE);
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)){ // Wait for EV8
        TimeOut--;
        if (TimeOut == 0){
            // Send I2C1 STOP Condition
            I2C_GenerateSTOP(I2Cx, ENABLE);
            turn_on_error_led_pin();        // Error LED

            return 2;
        }
    }

    I2C_SendData(I2Cx, slave_data1);
			while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)){ // Wait for EV8
				TimeOut--;
				if (TimeOut == 0){
					// Send I2C1 STOP Condition
					I2C_GenerateSTOP(I2Cx, ENABLE);
					turn_on_error_led_pin();        // Error LED

					return 2;
				}
			}

		 I2C_SendData(I2Cx, slave_data2);
					while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)){ // Wait for EV8
						TimeOut--;
						if (TimeOut == 0){
							// Send I2C1 STOP Condition
							I2C_GenerateSTOP(I2Cx, ENABLE);
							turn_on_error_led_pin();        // Error LED

							return 2;
						}
					}


    I2C_GenerateSTOP(I2Cx, ENABLE);
    return 1;
}
