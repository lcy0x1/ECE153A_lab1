/*****************************************************************************
 * bsp.c for Lab2B of ECE 153a at UCSB
 * Date of the Last Update:  November 8,2020
 *****************************************************************************/

#include "bsp.h"
#include "lab3b.h"
#include "../fft/header.h"

static int fft_enable = 0;

void BSP_init(void) {
	print("---Entering main---\n\r");
	if (setup()) {
		print("Initialization failed\n\r");
		print("---Exiting main---\n\r");
		for(;;);
	} else
		print("Initialization succeed\n\r");
	precompute();
	init_background();
}

void set_FFT_enable(int enable){
	fft_enable = enable;
	if(enable)
		stream_grabber_start();
}

void QF_onStartup(void) { /* entered with interrupts locked */
	enable_all();
}

void QF_onIdle(void) { /* entered with interrupts locked */
	QF_INT_UNLOCK()
	if(fft_enable) {
		add_window(auto_range(lab3b.octave));
		int note = find_note(lab3b.a4, get_mean());
		int freq = get_mean();
		if(lab3b.note != note||lab3b.freq != freq){
			lab3b.note = note;
			lab3b.freq = freq;
			post_command(UPDATE, TUNER);
		}
	}
	execute_command(); // LCD update
}

// delay is number of cycles, update LCD for 1024 sampling or more
void stream_wait(int delay){
	if(delay > 600)
		execute_command();
}

void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line) {
	(void) file; /* avoid compiler warning */
	(void) line; /* avoid compiler warning */
	QF_INT_LOCK()
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
	log_slow_mic_value(getMicValue());
}

// parse button flag into HSM signal
void intr_button(u32 flag) {
	if(flag & BTN_UP)
		dispatch(I_UP);
	if(flag & BTN_DOWN)
		dispatch(I_DOWN);
	if(flag & BTN_LEFT)
		dispatch(P_OCTAVE);
	if(flag & BTN_RIGHT)
		dispatch(P_A4);
	if(flag & BTN_CENTER)
		dispatch(P_TUNER);
}

// parse encoder flag intp HSM signal
void intr_encoder(u32 flag) {
	if (flag & ENC_CW)
		dispatch(I_UP);
	if (flag & ENC_CCW)
		dispatch(I_DOWN);
	if (flag & ENC_PUSH)
		dispatch(P_TUNER);
}
