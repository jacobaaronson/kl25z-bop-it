/* 
Implementation of IRQ driver for the accelerometer on the FRDM-KL25Z development board
Jacob Aaronson, Binghamton University
*/
#include "accelerometer_irq.h"
#include "MKL25Z4.h"
#include <stdbool.h>

#define INTERRUPT_PIN_LOC 14 

_Bool tap_detected = false;

void configure_accelerometer() {
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK; 
	PORTA->PCR[INTERRUPT_PIN_LOC] =  PORT_PCR_MUX(1) |  
									 PORT_PCR_IRQC(0x0a) |              
									 PORT_PCR_PE_MASK | 				
									 PORT_PCR_PS_MASK; 					 
	PTA->PDDR &= ~(1<<INTERRUPT_PIN_LOC); 								
	NVIC_SetPriority(PORTA_IRQn, 1);
	NVIC_ClearPendingIRQ(PORTA_IRQn);
	NVIC_EnableIRQ(PORTA_IRQn);
	
}

_Bool accelerometer_tap_was_detected() {
	return tap_detected;
}

void accelerometer_reset_tap_detect() {
	tap_detected=false;
}

void PORTA_IRQHandler() {
	tap_detected = true;
	PORTA->PCR[INTERRUPT_PIN_LOC] = (PORTA->PCR[INTERRUPT_PIN_LOC] | PORT_PCR_ISF_MASK);
}

