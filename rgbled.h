/* 
Header file for driver for RGB LED on the FRDM-KL25Z development board
Jacob Aaronson, Binghamton University
*/

#ifndef RGBLED_H
#define RGBLED_H

#include <stdint.h>

void configure_rgbled();
void set_rgbled_color_to(uint8_t red, uint8_t green, uint8_t blue);
void set_red_level(uint8_t);
void set_green_level(uint8_t);
void set_blue_level(uint8_t);
void turn_off_rgbled();

#endif
