/* 
Implementation of driver for SW1 on the FRDM-KL25Z development board
Jacob Aaronson, Binghamton University
*/

#include "sw1.h"
#include "MKL25Z4.h"
#include <stdbool.h>

#define SW1_LOC 20

void configure_sw1() {
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
	PORTA->PCR[SW1_LOC] = (PORTA->PCR[SW1_LOC] &~ (PORT_PCR_MUX_MASK)) | PORT_PCR_MUX(1);
	PTA->PDDR &= ~(1<<SW1_LOC);
}

_Bool sw1_is_pressed() {
	return !(PTA->PDIR & (1<<SW1_LOC));
}

_Bool sw1_is_released() {
	return !sw1_is_pressed();
}

void wait_for_sw1_press() {
	while(!sw1_is_pressed());
	while(!sw1_is_released());
}