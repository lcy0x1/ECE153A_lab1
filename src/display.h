#ifndef display_h
#define display_h

#include "lcd/lcd.h"

#define TEXT_0 "button UP"
#define TEXT_1 "button LEFT"
#define TEXT_2 "button RIGHT"
#define TEXT_3 "button DOWN"
#define TEXT_4 "button CENTER"

#define COL_BG_LINE setColor(127,127,255);
#define COL_BG_RECT setColor(0,0,127);

#define COL_VOL_RECT setColor(127,255,127);
#define COL_VOL_BACK setColor(0,0,0);
#define COL_VOL_BAR setColor(0,127,0);

#define COL_TEXT setColor(0,0,0);

/* the main loop to execute LCD instructions.
 * Since LCD instructions take a relatively long time to execute,
 * It is better to run it in the main loop instead of in interrupts.
 * Call this function at the end of main
 *
 * Here I use an interrupt-robust implementation to make sure that
 * all the content displayed on the LCD display visible to user is correct.
 * Any incorrect render caused by interrupts will be overwritten immediately. */
void main_loop(void);

/* Queue an instruction to draw background */
void drawBG(void);

/* Queue an instruction to draw text message */
void drawText(u32 ind);

/* Queue an instruction to draw volume bar*/
void drawVolume(u32 vol);

/* Flush the instructions and notify the main loop.
 * Call this after calling any of the 3 functions above to make them executable,
 * otherwise they will stay in the queue forever*/
void flush(void);

void lcd_test();

#endif
