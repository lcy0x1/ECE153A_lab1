#include "display.h"

#include <math.h>
#include "bsp.h"

#define MAX(A,B) A>B?A:B
#define MIN(A,B) A<B?A:B
#define RECT(X,Y,W,H) if(W>0&&H>0)fillRect(MAX(0,X), MAX(0,Y), MIN(239,X+W-1), MIN(319,Y+H-1));

char* btn_texts[5] = { TEXT_0, TEXT_1, TEXT_2, TEXT_3, TEXT_4 };

typedef struct DISP_QUQUE {
	volatile u32 update;
	volatile u32 volume;
	volatile u32 button;

	volatile u32 queue_pending;
	u32 queue_complete;

} DispQueue;

volatile DispQueue queue;

void draw_background(void);
void draw_volume(u32 volume);
void draw_text(u32 index);
void erase_volume(void);
void erase_text(void);

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
	draw_background();
	for (;;) {
		u32 pending = queue.queue_pending;
		if (queue.queue_complete < pending) {
			queue.queue_complete = pending;
			if (queue.update & UPDATE_VOL_OFF)
				erase_volume();
			else if (queue.update & UPDATE_VOL_ON)
				draw_volume(queue.volume);
			if (queue.update & UPDATE_TXT_OFF)
				erase_text();
			else if (queue.update & UPDATE_TXT_ON)
				draw_text(queue.button);
		}
	}
}

void draw_background(void) {
	COL_BG_RECT
	RECT(0, 0, 240, 320)
	COL_BG_LINE
	for (int i = 0; i <= 6; i++)
		RECT(-5+i*40,0,10,320)
	for (int i = 0; i <= 8; i++)
		RECT(0,-5+i*40,240,10)
}

void draw_volume(u32 volume) {
	COL_VOL_RECT
	RECT(45, 245, 150, 10);
	RECT(45, 255, 12, 10);
	RECT(183, 255, 12, 10);
	RECT(45, 265, 150, 10);
	COL_VOL_BACK
	RECT(57+2*volume, 255, 126-2*volume, 10);
	COL_VOL_BAR
	RECT(57, 255, 2*volume, 10);
}

void draw_text(u32 index) {
	COL_TEXT_BG
	COL_TEXT
	setFont(BigFont);
	lcdPrint(btn_texts[index], 16, 92);
}

void erase_volume(void) {
	COL_BG_RECT
	for(int i=1;i<=4;i++)
		RECT(5+i*40,245,30,30);
	COL_BG_LINE
	RECT(75,245,10,30);
	RECT(115,245,10,30);
	RECT(155,245,10,30);
}

void erase_text(void) {
	COL_BG_RECT
	for(int i=0;i<=5;i++)
		RECT(5+i*40,92,30,16);
	COL_BG_LINE
	for(int i=0;i<=4;i++)
	RECT(35+i*40,92,10,16);
}

void flush(u32 flags, u32 vol, u32 txt) {
	queue.update = flags;
	queue.volume = vol;
	queue.button = txt;
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

