/* 
Header file for driver for WDT on the FRDM-KL25Z development board
Jacob Aaronson, Binghamton University
*/

#ifndef COPWDT_H
#define COPWDT_H

void configure_copwdt();
void feed_the_watchdog();

#endif