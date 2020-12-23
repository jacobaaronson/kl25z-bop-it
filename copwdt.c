/* 
Implementation of driver for WDT on the FRDM-KL25Z development board
Jacob Aaronson, Binghamton University
*/

#include "copwdt.h"
#include "MKL25Z4.h"

#define COP_TIMEOUT_SELECT 2

void configure_copwdt() {
	SIM->COPC = SIM_COPC_COPT(COP_TIMEOUT_SELECT) | SIM_COPC_COPW(0) | SIM_COPC_COPCLKS(1);
}

void feed_the_watchdog() {
	SIM->SRVCOP = 0x55;
	SIM->SRVCOP = 0xAA;
}
