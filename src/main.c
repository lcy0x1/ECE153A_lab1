#include "xil_cache.h"		// Cache Drivers
#include "xil_printf.h"

#include "lcd/lcd.h"

#include "setup.h"

void intr_timer(u32 time);
void intr_button(u32 flag);
void intr_encoder(u32 flag);

void lcd_test();
void lcd_test_update();

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

void lcd_test() {

	clrScr();

	setColor(255, 0, 0);
	fillRect(20, 20, 220, 300);

	setColor(0, 50, 100);
	fillRect(40, 180, 200, 250);

	setColor(0, 255, 0);
	setColorBg(255, 0, 0);
	lcdPrint("Hello !!!!!", 40, 60);

	setFont(BigFont);
	lcdPrint("<# WORLD #>", 40, 80);

	setFont(SevenSegNumFont);
	setColor(238, 64, 0);
	setColorBg(0, 50, 100);

	timeout(100, &lcd_test_update);

}

void lcd_test_update() {

	static int sec = 0;

	int secTmp;
	char secStr[4];

	secTmp = sec++ % 1000;
	secStr[0] = secTmp / 100 + 48;
	secTmp -= (secStr[0] - 48) * 100;
	secStr[1] = secTmp / 10 + 48;
	secTmp -= (secStr[1] - 48) * 10;
	secStr[2] = secTmp + 48;
	secStr[3] = '\0';

	lcdPrint(secStr, 70, 190);

	timeout(100, &lcd_test_update);

}
