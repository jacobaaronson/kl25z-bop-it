# kl25z-bop-it
Final project for an embedded systems course: a bop it game for the FRDM-KL25Z development board

Project Goal and Usage
The goal of this project is to implement a Bop It-style game using several peripherals on the FRDM-KL25Z development board. The game play is as follows:

After programming the board, the player presses the push-button switch to start the game. After 1.5 seconds, the LED on the board lights up in one of three colors: red, green, or blue. The color of the LED indicates the action that needs to be completed by the player: 

Red indicates to touch the touch-slider.
Green indicates to press the push-button switch.
Blue indicates to double tap any side of the board (the double tap requires moderate force).

When it is detected that the player has completed the requested action, the LED will turn off for a certain period of time, and then it will turn on again to indicate a new action. The period of time between requested actions decreases as the game progresses, to make the game more challenging. If the requested action is not completed within two seconds of the LED turning on, or the player completes the wrong action at any point, then the game ends and the player has lost; this is indicated by the LED rapidly flashing white. The flashing is stopped by pressing the push-button switch, and then a new game can be started by pressing the switch again.

General Software Design
The software for the Bop It game relies on hardware drivers for several peripherals on the FRDM-KL25Z board, namely the SysTick timer, the watchdog timer, the RGB LED, the push-button switch, the I2C, the accelerometer, and the touch sensor. The main program utilizes the SysTick timer to make the main function execute once per millisecond, and uses the watchdog timer to automatically reset the system in case the program hangs, as has been done in every assignment for this course. The program uses a finite-state machine to progress through the various states of the game. 

FSM Details
The finite-state machine consists of seven unique states: 
WAIT_TO_START: In this state, the game has not yet started and the LED is off. The program remains in this state until the player presses the push-button switch, which starts the game. Also in this state, the time between action requests is set back to its maximum amount, effectively resetting the game. Upon the press of the push-button, the machine transitions to the next state, WAIT_FOR_TIME_INTERVAL.
WAIT_FOR_TIME_INTERVAL: In this state, the game is in progress, but the LED is off. The program waits for a certain period of time before progressing to the next state, ISSUE_ACTION_REQUEST. This amount of time is 1.5 seconds in the first round, and decreases by 50 milliseconds each round of the game, until it reaches a minimum of 0.3 seconds.
ISSUE_ACTION_REQUEST: In this state, the program generates a random integer and mods that integer with 3. Depending on the result of the mod operation, the program either lights the red LED and sets the next state to WAIT_FOR_TSI_TOUCH, or it lights the green LED and sets the next state to WAIT_FOR_SW1_PRESS, or it lights the blue LED and sets the next state to WAIT_FOR_DOUBLE_TAP.
WAIT_FOR_TSI_TOUCH: In this state, the program waits for the touch sensor to be touched, in which case it sets the next state to WAIT_FOR_TIME_INTERVAL. If two seconds pass or an incorrect action is performed, the next state is LOSER.
WAIT_FOR_SW1_PRESS: In this state, the program waits for the push-button to be pressed, in which case it sets the next state to WAIT_FOR_TIME_INTERVAL. If two seconds pass or an incorrect action is performed, the next state is LOSER.
WAIT_FOR_DOUBLE_TAP: In this state, the program waits for the accelerometer to indicate that the board has been double-tapped, in which case the next state is set to WAIT_FOR_TIME_INTERVAL. If two seconds pass or an incorrect action is performed, the next state is LOSER.
LOSER: In this state, the LED flashes bright white (in order to punish the player’s eyes) and waits for the push-button to be pressed. When the button is pressed, the program goes to the WAIT_TO_START state.

Use of Interrupts
Interrupts were used in a new accelerometer driver that takes advantage of the accelerometer’s INT1 interrupt line connected to pin PTA14 on the development board. In the configure_accelerometer() function of the driver, the registers PORTA->PCR[14] and PORTA->PDDR are written to set PTA14 as a GPIO input with interrupts enabled. The interrupt service routine sets the Boolean variable tap_detected to true. This global variable is shared with the main program by way of two functions: accelerometer_tap_was_detected() and accelerometer_reset_tap_detect(). The main program does a series of writes to the accelerometer registers to enable interrupts on the occurrence of a double tap.

Optimizations
All of the integer variables in the program are the smallest size possible; no size greater than a uint16_t is used. All of the functions in the program that are used in only one file are marked static. The global variables in the main program file are marked static, as they are not intended to be accessed outside the main file. The static variables in the program are not initialized in their declarations, as they need to be initialized in the code. The multiplier used in the random number generation is marked const since it never changes.
