/* 
Header file for IRQ driver for the accelerometer on the FRDM-KL25Z development board
Jacob Aaronson, Binghamton University
*/

#ifndef ACCELEROMETER_IRQ_H
#define ACCELEROMETER_IRQ_H

#include <stdbool.h>

void configure_accelerometer();
_Bool accelerometer_tap_was_detected();
void accelerometer_reset_tap_detect();

#endif