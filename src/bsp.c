/*****************************************************************************
 * bsp.c for Lab2B of ECE 153a at UCSB
 * Date of the Last Update:  November 8,2020
 *****************************************************************************/

#include "bsp.h"

void BSP_init(void) {
	print("---Entering main---\n\r");
	if (setup()) {
		print("Initialization failed\n\r");
		print("---Exiting main---\n\r");
		for(;;);
	} else
		print("Initialization succeed\n\r");
	init_background();
}

void QF_onStartup(void) { /* entered with interrupts locked */
	enable_all();
}

void QF_onIdle(void) { /* entered with interrupts locked */
	QF_INT_UNLOCK()

	update_queue();
}

/* Do not touch Q_onAssert */
/*..........................................................................*/
void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line) {
	(void) file; /* avoid compiler warning */
	(void) line; /* avoid compiler warning */
	QF_INT_LOCK()
	;

	print("\n\r--- Assert Failed ---\n\r");
	log_debug_info();
	for (;;) {
	}
}

/******************************************************************************
 *
 * This is the interrupt handler routine.
 *
 ******************************************************************************/

void intr_timer(u32 time) {
	dispatch_display(TICK, time);
}

void intr_button(u32 flag) {
	u32 btn = -1;
	while (flag) {
		flag >>= 1;
		btn++;
	}
	dispatch_display(BUTTON_CLICK, btn);
}

void intr_encoder(u32 flag) {
	if (flag & ENC_CW)
		dispatch_volume(ENCODER_CW);
	if (flag & ENC_CCW)
		dispatch_volume(ENCODER_CCW);
	if (flag & ENC_PUSH)
		dispatch_volume(ENCODER_CLICK);
}
