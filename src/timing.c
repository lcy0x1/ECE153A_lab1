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

#include "xil_cache.h"		// Cache Drivers
#include "extra.h" 		// Provides a source of bus contention

int main() {
	Xil_ICacheInvalidate()
	Xil_ICacheEnable();
	Xil_DCacheInvalidate()
	Xil_DCacheEnable();
	print("---Entering main---\n\r");
	if (extra_method() == XST_SUCCESS)
		print("Initialization succeed\n\r");
	else
		print("Initialization failed\n\r");

	print("---Exiting main---\n\r");

	while (1) {
		updateBaseLoop();
	}

}
