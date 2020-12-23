/* 
Implementation of driver for systick timer on the FRDM-KL25Z development board
Jacob Aaronson, Binghamton University
*/

#include "systick.h"
#include <stdint.h>
#include <stdbool.h>
#ifndef SYSTICK_PERIOD
#define SYSTICK_PERIOD 1000 //in usec
#endif
#define SYSTICK_TOP_VAL ((SYS_CLOCK/SYSTICK_PERIOD))
#include "MKL25Z4.h"

void configure_systick() {
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk;
	SysTick->LOAD = (uint32_t)SYSTICK_TOP_VAL;
}

volatile _Bool systick_flag;

void SysTick_Handler() {
	systick_flag = true;
}

_Bool systick_has_fired(){
	if(systick_flag) {
		systick_flag=false;
		return true;
	}
	return false;
}