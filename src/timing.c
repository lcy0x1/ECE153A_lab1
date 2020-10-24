/*
 *timing.c: simple starter application for lab 1A and 1B
 *
 * The General Approach:
 * Use the interrupt handler for timer has the grand loop
 * Make the interrupt handler for button as small as possible
 * Handle button operation in the grand loop
 *
 *
 */

#include <stdio.h>		// Used for printf()
#include <stdlib.h>		// Used for rand()
#include "xparameters.h"	// Contains hardware addresses and bit masks
#include "xil_cache.h"		// Cache Drivers
#include "xintc.h"		// Interrupt Drivers
#include "xtmrctr.h"		// Timer Drivers
#include "xtmrctr_l.h" 		// Low-level timer drivers
#include "xil_printf.h" 	// Used for xil_printf()
#include "extra.h" 		// Provides a source of bus contention
#include "xgpio.h" 		// LED driver, used for General purpose I/O

#include "sevenseg_new.h"

#define BTN_UP 1
#define BTN_LEFT 2
#define BTN_RIGHT 4
#define BTN_DOWN 8
#define BTN_CENTER 16

#define STATUS_PREPARE 0
#define STATUS_TIMER 1
#define STATUS_STOP 2

static volatile u32 timer = 0;
static volatile u32 status = STATUS_PREPARE;

void intr(void);

int main() {
	Xil_ICacheInvalidate()
	Xil_ICacheEnable();
	Xil_DCacheInvalidate()
	Xil_DCacheEnable();
	print("---Entering main---\n\r");
	if (extra_method(intr) == XST_SUCCESS)
		print("Initialization succeed\n\r");
	else
		print("Initialization failed\n\r");

	// finish preparation stage and set timer to state STOP
	status = STATUS_STOP;

	print("---Exiting main---\n\r");

}

void intr(void) {
	if (status == STATUS_PREPARE)
		return;

	u32 flag = getAndClearBtnFlag();

	// when start and stop are pressed at the same time, prioritize START over STOP
	if (flag & BTN_LEFT)
		timer = 0; // reset
	if (flag & BTN_CENTER) {
		// start
		if (!MULTI_FUNCTION_START) {
			// start button version 1
			// executed only when the start button only has one function
			status = STATUS_TIMER;
		} else if (status == STATUS_TIMER) {
			status = STATUS_STOP;
		} else {
			status = STATUS_TIMER;
			timer = 0;
		}
	} else if (flag & BTN_RIGHT)
		status = STATUS_STOP; // stop

	// global clock for this grand loop
	static u32 clk = 0;

	// frequency = 80kHz / granularity
	clk++;

	if (status == STATUS_TIMER)
		timer++;

	// refreshing LED takes 8 calls
	// effectively the LED display has 80kHz/granularity/8 = 10kHz/granularity refresh frequency
	// desired clock frequency of 10KHz, thus each time it steps up by the granularity
	u32 time = timer / 8 * GRANULARITY;

	u32 i, val = 0;

	for (i = 0; i < 8; i++) {
		val = time % 10;
		time /= 10;
		if ((clk & 7) == i)
			sevenseg_draw_digit(i, val);
	}
}
