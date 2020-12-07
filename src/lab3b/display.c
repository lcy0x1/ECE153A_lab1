#include "display.h"
#include "lab3b.h"
#include "../fft/header.h"

static u32 pending_cmd = 0;

void post_command(enum ACTION act, enum PAGE p){
	if(act == DRAW)
		pending_cmd &= 0b001100;
	if(act == UPDATE)
		pending_cmd &= 0b001111;
	if(act == ERASE) {
		if(pending_cmd & 0b001100)
			return;
		else
			pending_cmd = 0;
	}
	pending_cmd |= (p + 1) << (act * 2);
}

void draw_octave(void);
void draw_tuner(void);
void draw_a4(void);
void erase_octave(void);
void erase_tuner(void);
void erase_a4(void);
void update_octave(void);
void update_tuner(void);
void update_a4(void);

void execute_command(void){
	if((pending_cmd & 0b001100) == 0b000100)
		erase_octave();
	if((pending_cmd & 0b001100) == 0b001000)
		erase_tuner();
	if((pending_cmd & 0b001100) == 0b001100)
		erase_a4();

	if((pending_cmd & 0b000011) == 0b000001)
		draw_octave();
	if((pending_cmd & 0b000011) == 0b000010)
		draw_tuner();
	if((pending_cmd & 0b000011) == 0b000011)
		draw_a4();

	if((pending_cmd & 0b110000) == 0b010000)
		update_octave();
	if((pending_cmd & 0b110000) == 0b100000)
		update_tuner();
	if((pending_cmd & 0b110000) == 0b110000)
		update_a4();

	pending_cmd = 0;
}

//--------------------------------------
// screen initialization
//--------------------------------------

#define MAX(A,B) A>B?A:B
#define MIN(A,B) A<B?A:B
#define RECT(X,Y,W,H) if(W>0&&H>0)fillRect(MAX(0,X), MAX(0,Y), MIN(239,X+W-1), MIN(319,Y+H-1));

#define COL_BG 0215, 0306, 0163
#define COL_TXT 0377, 0377, 0377
#define COL_TUNER 0115, 0206, 0145

#define RECTBG(R,G,B,X,Y,W,H) setColor(R,G,B); RECT(X, Y, W, H) setColor(COL_TXT); setColorBg(R,G,B);

#define RECT_OCT(COL) RECTBG(COL, 80, 110, 80, 80)
#define RECT_TUNER(COL) RECTBG(COL, 80, 110, 80, 80)
#define RECT_A4(COL) RECTBG(COL, 70, 110, 100, 60)

#define PRINT_C(C,R,Y) printChar(C,120+cfont.x_size*R/2,Y);
#define PRINT_S(S,R,Y) lcdPrint(S,120+cfont.x_size*R/2,Y);

void init_background(void) {
	initLCD();
	setColor(COL_BG);
	RECT(0, 0, 240, 320)
	setColorBg(COL_BG);
	setColor(COL_TXT);
	setFont(BigFont);
	lcdPrint("Project Tuner", 10, 10);
	setFont(SmallFont);
	lcdPrint(" Arthur Wang & Tianrui Hu", 10, 40);
}

//--------------------------------------
// octave selection screen
//--------------------------------------

void draw_octave(void){
	RECT_OCT(COL_TUNER)

	PRINT_S("Current", -7, 120);
	PRINT_S("octave", -6, 130);

	update_octave();
}

void update_octave(void){
	u32 a = lab3b.octave;
	setFont(BigFont);
	if(a<10){
		PRINT_S("    ",-4,150);
		PRINT_C(a + 0x30, -1, 150);
	}
	else {
		PRINT_S("Auto",-4,150);
	}
	setFont(SmallFont);
}

void erase_octave(void){
	RECT_OCT(COL_BG)
}


//--------------------------------------
// tuner screen
//--------------------------------------

void draw_tuner(void){
	RECT_TUNER(COL_TUNER)

	update_tuner();
}

void update_tuner(void){
	u32 note = lab3b.note + 50;
	int cent = note % 100 - 50;
	u32 octave = note / 1200;
	char* str = note_char(note / 100 % 12);
	setFont(BigFont);
	PRINT_S(str, -3, 130);
	PRINT_C(octave+0x30, 1, 130);
	setFont(SmallFont);
	PRINT_C(cent < 0 ? '-' : ' ',-3, 150);
	cent = cent < 0 ? -cent : cent;
	u8 cten = cent / 10;
	PRINT_C(cten == 0 ? ' ' : cten + 0x30, -1, 150);
	PRINT_C(cent % 10 + 0x30, 1, 150);
}

void erase_tuner(void){
	RECT_TUNER(COL_BG)
}


//--------------------------------------
// A4 note adjustment screen
//--------------------------------------

void draw_a4(void){
	RECT_A4(COL_TUNER)

	PRINT_S("Current A4", -10, 120);
	setFont(BigFont);
	PRINT_S("Hz", 1, 140);

	update_a4();
}

void update_a4(void){
	u32 freq = lab3b.a4;
	PRINT_C(0x34, -5, 140); //4
	PRINT_C((freq-400)/10+0x30, -3, 140); //tenth
	PRINT_C((freq-400)%10+0x30, -1, 140); //decimal
}

void erase_a4(void){
	RECT_A4(COL_BG)
	setFont(SmallFont);
}
