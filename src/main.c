/*
 * Copyright (c) 2009-2012 Xilinx, Inc.  All rights reserved.
 *
 * Xilinx, Inc.
 * XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS" AS A
 * COURTESY TO YOU.  BY PROVIDING THIS DESIGN, CODE, OR INFORMATION AS
 * ONE POSSIBLE   IMPLEMENTATION OF THIS FEATURE, APPLICATION OR
 * STANDARD, XILINX IS MAKING NO REPRESENTATION THAT THIS IMPLEMENTATION
 * IS FREE FROM ANY CLAIMS OF INFRINGEMENT, AND YOU ARE RESPONSIBLE
 * FOR OBTAINING ANY RIGHTS YOU MAY REQUIRE FOR YOUR IMPLEMENTATION.
 * XILINX EXPRESSLY DISCLAIMS ANY WARRANTY WHATSOEVER WITH RESPECT TO
 * THE ADEQUACY OF THE IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO
 * ANY WARRANTIES OR REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE
 * FROM CLAIMS OF INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include "fft/header.h"
#include <stdio.h>
#include "xil_cache.h"
#include <mb_interface.h>

#include "xparameters.h"
#include <xil_types.h>
#include <xil_assert.h>

#include <xio.h>
#include "xtmrctr.h"
#include "fft/fft.h"
#include "fft/note.h"
#include "fft/stream_grabber.h"
#include "extra.h"

#define CLOCK 100000000.0 //clock speed

static int q[SAMPLES];
static int w[SAMPLES];

void intr(void);

//void print(char *str);

void analysis(){
	u32 a;
	asm("add %0, r0, r14":"=r"(a));
}

void read_fsl_values() {
	int i, j, sum;
	for (i = 0; i < SAMPLES; i++) {
		sum = 0;
		for (j = 0; j < SKIPS; j++) {
			sum += stream_grabber_read_sample(i * SKIPS + j) / SKIPS;
		}
		q[i] = sum;
		w[i] = 0;

	}
}

int main() {
	float sample_f;
	int ticks; //used for timer
	uint32_t Control;
	float frequency;
	float tot_time; //time to run program

	Xil_ICacheInvalidate()
	;
	Xil_ICacheEnable();
	Xil_DCacheInvalidate()
	;
	Xil_DCacheEnable();

	//set up timer
	XTmrCtr timer;
	XTmrCtr_Initialize(&timer, XPAR_AXI_TIMER_0_DEVICE_ID);
	Control = XTmrCtr_GetOptions(&timer,
			0) | XTC_CAPTURE_MODE_OPTION | XTC_INT_MODE_OPTION;
	XTmrCtr_SetOptions(&timer, 0, Control);

	print("Hello World\n\r");

	if (extra_method(intr) == XST_SUCCESS)
			print("Initialization succeed\n\r");
		else
			print("Initialization failed\n\r");

	precompute(); // precompute the sine and cosine table
	stream_grabber_start();
	stream_grabber_wait_enough_samples(SAMPLES * SKIPS);
	while (1) {
		XTmrCtr_Start(&timer, 0);

		//Read Values from Microblaze buffer, which is continuously populated by AXI4 Streaming Data FIFO.
		read_fsl_values();
		// start to grab the data for next cycle
		stream_grabber_start();

		// do fft
		sample_f = 100000000 / 2048.0 / SKIPS;
		frequency = fft(q, w, sample_f);
		findNote(frequency);

		// wait until the sampling for next cycle finished
		stream_grabber_wait_enough_samples(SAMPLES * SKIPS);

		//get time to run program
		ticks = XTmrCtr_GetValue(&timer, 0);
		XTmrCtr_Stop(&timer, 0);
		tot_time = ticks / CLOCK;
		//xil_printf("f = %4d Hz, t = %3d ms \r\n", (int) (frequency + .5),
		//		(int) (1000 * tot_time));
	}

	return 0;
}


void intr(void) {

}

