/* 
Header file for driver for the TSI on the FRDM-KL25Z development board
Jacob Aaronson, Binghamton University
*/

#ifndef TSI0_H
#define TSI0_H

#include <stdint.h>
#include <stdbool.h>
void configure_tsi0();
_Bool tsi0_scan_is_complete();
_Bool tsi0_scan_is_in_progress();
_Bool tsi0_start_scan(uint8_t channel);
uint16_t tsi0_get_scan_result();

#endif