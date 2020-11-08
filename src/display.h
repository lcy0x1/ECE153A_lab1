#ifndef display_h
#define display_h

#include "lcd/lcd.h"

#define UPDATE_VOL_ON 1
#define UPDATE_VOL_OFF 2
#define UPDATE_TXT_ON 4
#define UPDATE_TXT_OFF 8

#define TEXT_0 "  button UP  "
#define TEXT_1 " button LEFT "
#define TEXT_2 "button RIGHT "
#define TEXT_3 " button DOWN "
#define TEXT_4 "button CENTER"

#define COL_BG_LINE setColor(127,127,255);
#define COL_BG_RECT setColor(0,0,127);

#define COL_VOL_RECT setColor(127,255,127);
#define COL_VOL_BACK setColor(0,0,0);
#define COL_VOL_BAR setColor(0,127,0);

#define COL_TEXT_BG setColorBg(127,255,127);
#define COL_TEXT setColor(238,64,0);

/* the main loop to execute LCD instructions.
 * Since LCD instructions take a relatively long time to execute,
 * It is better to run it in the main loop instead of in interrupts.
 * Call this function at the end of main
 *
 * Here I use an interrupt-robust implementation to make sure that
 * all the content displayed on the LCD display visible to user is correct.
 * Any incorrect render caused by interrupts will be overwritten immediately. */
void main_loop(void);


void flush(u32 flags, u32 vol, u32 txt);

void lcd_test();

#endif
