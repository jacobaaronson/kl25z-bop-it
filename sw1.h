/* 
Header file for driver for SW1 on the FRDM-KL25Z development board
Jacob Aaronson, Binghamton University
*/

#ifndef SW1_H
#define SW1_H

#include <stdbool.h>

_Bool sw1_is_pressed();
_Bool sw1_is_released();
void configure_sw1();
void wait_for_sw1_press();

#endif