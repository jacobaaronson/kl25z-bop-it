/*
Program for the FRDM-KL25Z development board that implements a Bop It-style game using the LED, pushbutton switch, accelerometer, and the touch slider.
Jacob Aaronson, Binghamton University
*/

#include "systick.h"
#include "copwdt.h"
#include "rgbled.h"
#include "sw1.h"
#include "accelerometer_irq.h"
#include "i2c0_irq.h"
#include "tsi0.h"
#include <stdint.h>
#include <stdbool.h>

#define BOUNCE_INTERVAL 100
#define LED_BRIGHTNESS 0x07
#define WAIT_TIME 1999
#define TSI_SENSITIVITY 45
#define MAX_BRIGHTNESS 255
#define MAX_TIME_INTERVAL 1499
#define MIN_TIME_INTERVAL 299
#define TIME_DECREMENT 50

#define CTRL_REG_1 0x2a
#define PULSE_CFG_REG 0x21
#define PULSE_THSX_REG 0x23
#define PULSE_THSY_REG 0x24
#define PULSE_THSZ_REG 0x25
#define PULSE_TMLT_REG 0x26
#define PULSE_LTCY_REG 0x27
#define PULSE_WIND_REG 0x28
#define CTRL_REG_4 0x2d
#define CTRL_REG_5 0x2e

static uint8_t rng_cntr;
static uint16_t tsi_scan_result;
enum SWITCH_PRESS_TYPE {NO_PRESS, PRESS};

static void play_bop_it();
static void run_tsi_scan();
static enum SWITCH_PRESS_TYPE sw1_press_type();
static void set_accelerometer_to_detect_double_tap();

void main(){
	asm("CPSID i");
	configure_systick();
	configure_copwdt();
	configure_rgbled();
	configure_sw1();
	configure_i2c0();
	configure_accelerometer();
	configure_tsi0();
	asm("CPSIE i");
	
	set_accelerometer_to_detect_double_tap();

	while(1) {
		asm("wfi");
		if(!systick_has_fired())
			continue;
		rng_cntr++;
		feed_the_watchdog();
		play_bop_it();
	}
}

static void play_bop_it() {
	static enum {WAIT_TO_START, WAIT_FOR_TIME_INTERVAL, ISSUE_ACTION_REQUEST, WAIT_FOR_DOUBLE_TAP, WAIT_FOR_SW1_PRESS, WAIT_FOR_TSI_TOUCH, LOSER} state = WAIT_TO_START;
	static uint16_t ms_cntr;
	static uint8_t random_number; 
	static uint16_t time_interval;
	const uint8_t multiplier = 126;
	switch(state) {
		default:
		case WAIT_TO_START:
			turn_off_rgbled();
			time_interval=MAX_TIME_INTERVAL;
			if(sw1_press_type()==PRESS)
				state=WAIT_FOR_TIME_INTERVAL;
			break;
		case WAIT_FOR_TIME_INTERVAL:
			accelerometer_reset_tap_detect();
			turn_off_rgbled();
			ms_cntr++;
			if(ms_cntr==time_interval) {
				ms_cntr=0;
				state=ISSUE_ACTION_REQUEST;
				if(time_interval>MIN_TIME_INTERVAL)
					time_interval-=TIME_DECREMENT;
			}
			break;
		case ISSUE_ACTION_REQUEST:
			random_number = rng_cntr+(random_number*multiplier);
			if((random_number%3)==0){
				set_rgbled_color_to(LED_BRIGHTNESS,0,0);
				state=WAIT_FOR_TSI_TOUCH;
			}
			else if ((random_number%3)==1){
				set_rgbled_color_to(0,LED_BRIGHTNESS,0);
				state=WAIT_FOR_SW1_PRESS;
			}
			else if ((random_number%3)==2){
				set_rgbled_color_to(0,0,LED_BRIGHTNESS);
				state=WAIT_FOR_DOUBLE_TAP;
			}
			ms_cntr=0;
			accelerometer_reset_tap_detect();
			break;
		case WAIT_FOR_TSI_TOUCH:
			if(ms_cntr==WAIT_TIME)
				state=LOSER;
			else{
				ms_cntr++;
				if((sw1_press_type()==PRESS) || accelerometer_tap_was_detected())
					state=LOSER;
				tsi_scan_result=0;
				run_tsi_scan();
				if((tsi_scan_result>TSI_SENSITIVITY)&&(ms_cntr>100)){
					state=WAIT_FOR_TIME_INTERVAL;
					ms_cntr=0;
				}
			}
			break;
		case WAIT_FOR_SW1_PRESS:
			if(ms_cntr==WAIT_TIME)
				state=LOSER;
			else{
				ms_cntr++;
				if(sw1_press_type()==PRESS){
					state=WAIT_FOR_TIME_INTERVAL;
					ms_cntr=0;
				}
				tsi_scan_result=0;
				run_tsi_scan();
				if(((tsi_scan_result>TSI_SENSITIVITY)&&(ms_cntr>100)) || accelerometer_tap_was_detected())
					state=LOSER;
			}
			break;
		case WAIT_FOR_DOUBLE_TAP:
			if(ms_cntr==WAIT_TIME) 
				state=LOSER;
			else {
				ms_cntr++;
				if(accelerometer_tap_was_detected()){
					state=WAIT_FOR_TIME_INTERVAL;
					accelerometer_reset_tap_detect();
					ms_cntr=0;
				}
				tsi_scan_result=0;
				run_tsi_scan();
				if(((tsi_scan_result>TSI_SENSITIVITY)&&(ms_cntr>100)) || sw1_press_type()==PRESS)
					state=LOSER;
			}
			break;
		case LOSER:
			if(ms_cntr>MAX_BRIGHTNESS)
				ms_cntr=0;
			ms_cntr++;
			set_rgbled_color_to(ms_cntr,ms_cntr,ms_cntr);
			if(sw1_press_type()==PRESS) {
				state=WAIT_TO_START;
				turn_off_rgbled();
			}
			break;
	}
	
}

static enum SWITCH_PRESS_TYPE sw1_press_type() {
	static enum {ST_NO_PRESS, ST_BOUNCE, ST_SHORT} state = ST_NO_PRESS;
	static uint16_t ms_cntr_sw;
	switch(state) {
		default:
		case ST_NO_PRESS:
			if(sw1_is_pressed()) {
				state = ST_BOUNCE;
				ms_cntr_sw = 0;
			}
			break;
		case ST_BOUNCE:
			if(sw1_is_pressed()) {
				if(ms_cntr_sw<BOUNCE_INTERVAL)
					ms_cntr_sw++;
				else {
					state = ST_SHORT;
					ms_cntr_sw++;
				}
			}
			else {
				state = ST_NO_PRESS; 
				ms_cntr_sw++;
			}
			break;
		case ST_SHORT:
			if(!sw1_is_pressed()) {
				state = ST_NO_PRESS;
				return PRESS;
			}
			break;
	}
	return NO_PRESS;
}

static void run_tsi_scan(){
	if(tsi0_scan_is_complete())
		tsi_scan_result = tsi0_get_scan_result();
		
	if(!tsi0_scan_is_in_progress())
		tsi0_start_scan(10);
}

static void set_accelerometer_to_detect_double_tap(){
	while(!(i2c0_write_byte(0x1d<<1, CTRL_REG_1, 0x08)));
	while(!(i2c0_write_byte(0x1d<<1, PULSE_CFG_REG, 0x2a)));
	while(!(i2c0_write_byte(0x1d<<1, PULSE_THSX_REG, 0x30)));
	while(!(i2c0_write_byte(0x1d<<1, PULSE_THSY_REG, 0x30)));
	while(!(i2c0_write_byte(0x1d<<1, PULSE_THSZ_REG, 0x4f)));
	while(!(i2c0_write_byte(0x1d<<1, PULSE_TMLT_REG, 0x30)));
	while(!(i2c0_write_byte(0x1d<<1, PULSE_LTCY_REG, 0x50)));
	while(!(i2c0_write_byte(0x1d<<1, PULSE_WIND_REG, 0x78)));
	while(!(i2c0_write_byte(0x1d<<1, CTRL_REG_4, 0x08)));
	while(!(i2c0_write_byte(0x1d<<1, CTRL_REG_5, 0x09)));
	while(!(i2c0_write_byte(0x1d<<1, CTRL_REG_1, 0x09)));
}