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
	// TODO
}

//--------------------------------------
// octave selection screen
//--------------------------------------

void draw_octave(void){
	u32 octave = lab3b.octave;
}

void update_octave(void){

}

void erase_octave(void){

}


//--------------------------------------
// tuner screen
//--------------------------------------

void draw_tuner(void){

}

void update_tuner(void){
	u32 note = lab3b.note;
	u32 cent = note % 100;
	u32 octave = note / 1200;
	char* str = note_char(note / 100 % 12);
}

void erase_tuner(void){

}


//--------------------------------------
// A4 note adjustment screen
//--------------------------------------

void draw_a4(void){
	u32 freq = lab3b.a4;
	//TODO
}

void update_a4(void){
}

void erase_a4(void){

}

