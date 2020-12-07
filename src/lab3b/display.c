#include "display.h"
#include "lab3b.h"
#include "../fft/header.h"

#define MAX(A,B) A>B?A:B
#define MIN(A,B) A<B?A:B
#define RECT(X,Y,W,H) if(W>0&&H>0)fillRect(MAX(0,X), MAX(0,Y), MIN(239,X+W-1), MIN(319,Y+H-1));

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


void init_background(void) {
	initLCD();
	drawBG(0x215,0x306,0x163,0,0,239,329);
	setColorBg(0x215,0x306,0x163);
	setFont(BigFont);
	lcdPrint("Projekt Tuner",10,10);
	setFont(SmallFont);
	lcdPrint(" Arthur Wang & Tianrui Hu",10,40);
	// TODO
}

//--------------------------------------
// octave selection screen
//--------------------------------------

void draw_octave(void){
	drawBG(0x215,0x306,0x163,0,110,70,220);
	lcdPrint("Current",1,110);
	lcdPrint("octave",1,120);
	update_octave();
}

void update_octave(void){
	u32 octave = lab3b.octave;
	u8 a = (u8)octave+0x30;
	//xil_printf("%d",a);
	//setFont(BigFont);
	printChar(a,30,150); //print
}

void erase_octave(void){
	drawBG(0x215,0x306,0x163,0,110,70,220);
}


//--------------------------------------
// tuner screen
//--------------------------------------

void draw_tuner(void){
	drawBG(0x115,0x206,0x145,80,110,160,220);
	setColorBg(0x115,0x206,0x145);
	update_tuner();
}

void update_tuner(void){
	drawBG(0x115,0x206,0x145,100,130,160,160);
	u32 note = lab3b.note;
	u32 cent = note % 100;
	u32 octave = note / 1200;
	char* str = note_char(note / 100 % 12);
	setFont(BigFont);
	lcdPrint(str,95,130);
	u8 a = (u8)octave+0x30;
	printChar(a,125,130);
	setFont(SmallFont);


}

void erase_tuner(void){
	drawBG(0x215,0x306,0x163,80,110,160,220);
	setColorBg(0x215,0x306,0x163);
}


//--------------------------------------
// A4 note adjustment screen
//--------------------------------------

void draw_a4(void){
	drawBG(0x215,0x306,0x163,70,250,180,290);
	lcdPrint("Current A4",70,255);
	lcdPrint("Hz",130,270);
	update_a4();
	//TODO
}

void update_a4(void){
	u32 freq = lab3b.a4;
	printChar(0x34,90,270); //4
	u8 a = (((int)freq-400)/10)+0x30;
	u8 b = (((int)freq-400)%10)+0x30;
	printChar(a,100,270); //tenth
	printChar(b,110,270); //decimal
}

void erase_a4(void){
	drawBG(0x215,0x306,0x163,70,250,180,290);
}

void drawBG(u8 r, u8 g, u8 b,int x,int y, int x2, int y2){ //Draw Background with Blue
	setColor(r,g,b);
	fillRect(x,y,x2,y2);
	setColor(0x377,0x377,0x377);
}
