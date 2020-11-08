#include "display.h"

#include "bsp.h"

#define UPDATE_BG 1
#define UPDATE_VOL 2
#define UPDATE_TXT 4

char* btn_texts[5] = { TEXT_0, TEXT_1, TEXT_2, TEXT_3, TEXT_4 };

typedef struct DISP_QUQUE {
	volatile u32 update;
	volatile u32 volume;
	volatile u32 button;

	volatile u32 queue_pending;
	u32 queue_complete;

} DispQueue;

volatile DispQueue queue;

/* Interrupt robust code:
 *
 * This design use the increment - catch-up method to avoid clearing the
 * flag that is set in an interrupt between flag-reading and flag-clearing operation.
 *
 * The variable pending will hold the snapshot queue.queue_pending value.
 * Even if interrupt happens during the middle of execution,
 * they will increment queue.queue_pending, but local variable pending
 * will be left unchanged, which means new instructions will be executed
 * in the next cycle.
 *
 * */
void main_loop(void) {
	for (;;) {
		u32 pending = queue.queue_pending;
		if (queue.queue_complete < pending) {
			queue.queue_complete = pending;

			if (queue.update & UPDATE_BG) {
				COL_BG_LINE
				fillRect(0, 0, 239, 319);
				COL_BG_RECT
				for (int i = 0; i < 8; i++)
					for (int j = 0; j < 6; j++)
						fillRect(5 + j * 40, 5 + i * 40, 35 + j * 40,
								35 + i * 40);
			}
			if (queue.update & UPDATE_VOL) {
				COL_VOL_RECT
				fillRect(36, 220, 204, 300);
				COL_VOL_BACK
				fillRect(56, 240, 184, 280);
				COL_VOL_BAR
				fillRect(56, 240, 56 + 2 * queue.volume, 280);
			}
			if (queue.update & UPDATE_TXT) {
				setFont(BigFont);
				lcdPrint(btn_texts[queue.button], 40, 80);
			}
		}
	}
}

void drawBG(void) {
	queue.update = UPDATE_BG;
}

void drawText(u32 ind) {
	queue.button = ind;
	queue.update |= UPDATE_TXT;
}

void drawVolume(u32 vol) {
	queue.volume = vol;
	queue.update |= UPDATE_VOL;
}

void flush(void) {
	queue.queue_pending++;
}

void lcd_test_update();

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

