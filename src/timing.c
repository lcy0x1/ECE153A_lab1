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

	u32 flag = getEncPos() & 0xFF;

	static u32 clk = 0;

	clk++;

	u32 i, val = 0;
	if(clk%4000>2000)
	rgbLED();
	else
	rgboff();
	for (i = 0; i < 8; i++) {
		val = flag % 10;
		flag /= 10;
		if ((clk & 7) == i)
			sevenseg_draw_digit(i, val);
	}
}

