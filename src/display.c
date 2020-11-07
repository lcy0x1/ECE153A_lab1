#include "display.h"

#include "setup.h"

char* btn_texts[5] = {TEXT_0, TEXT_1, TEXT_2, TEXT_3, TEXT_4};

void drawBG(void){
	COL_BG_LINE
	fillRect(0,0,240,320);
	COL_BG_RECT
	for(int i=0;i<8;i++)
		for(int j=0;j<6;j++)
			fillRect(5+j*40,5+i*40,30,30);
}

void drawText(u32 ind){
	setFont(BigFont);
	lcdPrint(btn_texts[ind], 40,80);
}

void drawVolume(u32 vol){
	COL_VOL_RECT
	fillRect(36,240,168,80);
	COL_VOL_BACK
	fillRect(56,260,128,40);
	COL_VOL_BAR
	fillRect(56,260,2*vol,40);
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
