#include "xil_cache.h"		// Cache Drivers
#include "xil_printf.h"
#include "qpnano/qpn_port.h"
#include "bsp.h"
#include "lab2b.h"
#include "display.h"

static volatile u32 ready = 0;

void intr_timer(u32 time);
void intr_button(u32 flag);
void intr_encoder(u32 flag);

int main() {
	Xil_ICacheInvalidate()
	Xil_ICacheEnable();
	Xil_DCacheInvalidate()
	Xil_DCacheEnable();
	print("---Entering main---\n\r");
	setTimerInterrupt(&intr_timer);
	setButtonInterrupt(&intr_button);
	setEncoderInterrupt(&intr_encoder);
	if (setup()) {
		print("Initialization failed\n\r");
		print("---Exiting main---\n\r");
		return 0;
	} else
		print("Initialization succeed\n\r");

	initLCD();
	Lab2B_ctor();
	ready = 1;
	print("---Exiting main---\n\r");

	main_loop();

	return 0;
}

void intr_timer(u32 time) {
	if(!ready)
		return;
	dispatch_display(TICK, time);
}

void intr_button(u32 flag) {
	if(!ready)
		return;
	u32 btn = -1;
	while(flag){
		flag >>= 1;
		btn++;
	}
	dispatch_display(BUTTON_CLICK, btn);
}

void intr_encoder(u32 flag) {
	if(!ready)
		return;
	if(flag & ENC_CW)
		dispatch_volume(ENCODER_CW);
	if(flag & ENC_CCW)
		dispatch_volume(ENCODER_CCW);
	if(flag & ENC_PUSH)
		dispatch_volume(ENCODER_CLICK);
}
