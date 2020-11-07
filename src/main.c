#include "xil_cache.h"		// Cache Drivers
#include "xil_printf.h"

#include "display.h"
#include "setup.h"

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

	print("---Exiting main---\n\r");

	lcd_test();

	return 0;
}

void intr_timer(u32 time) {

}

void intr_button(u32 flag) {
	setLEDs(flag << 4);
}

void intr_encoder(u32 flag) {
	setLEDs(flag);
}
