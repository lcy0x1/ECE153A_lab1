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

void drawBG(void);

void drawText(u32 ind);

void drawVolume(u32 vol);

void lcd_test(void);

#endif
