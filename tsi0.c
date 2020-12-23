/* 
Implementation of driver for the TSI on the FRDM-KL25Z development board
Jacob Aaronson, Binghamton University
*/

#include "tsi0.h"
#include <stdint.h>
#include <stdbool.h>
#include "MKL25Z4.H"

uint16_t tsi0_get_scan_result(){
	return TSI0->DATA & TSI_DATA_TSICNT_MASK;
}

_Bool tsi0_start_scan(uint8_t channel) {
	if(tsi0_scan_is_in_progress())
		return false;
	TSI0->GENCS |= TSI_GENCS_EOSF_MASK;
	TSI0->DATA = TSI_DATA_TSICH(channel) | TSI_DATA_SWTS_MASK;
	return true;
}

_Bool tsi0_scan_is_in_progress(){
	return TSI0->GENCS & TSI_GENCS_SCNIP_MASK;
}

_Bool tsi0_scan_is_complete(){
	return TSI0->GENCS & TSI_GENCS_EOSF_MASK;
}

void configure_tsi0() {
	SIM->SCGC5 |= SIM_SCGC5_TSI_MASK;
	TSI0->GENCS = TSI_GENCS_REFCHRG(0)
				| TSI_GENCS_DVOLT(2)
				| TSI_GENCS_EXTCHRG(0)
				| TSI_GENCS_PS(0)
				| TSI_GENCS_NSCN(1)
				| TSI_GENCS_TSIIEN(0)
				| TSI_GENCS_STM(0);
				
	TSI0->GENCS |= TSI_GENCS_TSIEN_MASK;
}