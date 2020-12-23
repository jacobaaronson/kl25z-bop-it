/* 
Header file for the IRQ driver for the I2C on the FRDM-KL25Z development board
Jacob Aaronson, Binghamton University
*/

#ifndef I2C0_H
#define I2C0_H

#include <stdint.h>
#include <stdbool.h>

void configure_i2c0();
_Bool i2c0_write_byte(uint8_t device, uint8_t reg, uint8_t data);
_Bool i2c0_read_bytes(uint8_t device, uint8_t reg, uint8_t count, uint8_t * data);
_Bool i2c0_last_transaction_complete();
_Bool i2c0_last_transaction_had_error();


#endif