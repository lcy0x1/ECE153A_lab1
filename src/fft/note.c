#include "header.h"
#include <math.h>

static char notes[12][3]={"C ","C#","D ","D#","E ","F ","F#","G ","G#","A ","A#","B "};

static float window[WINDOW_SIZE];
static int win_size = 0, win_end = -1;
static float win_sum = 0, win_sqsum = 0;
static int win_level = 8;

void set_width(int width){
	if(win_level == width)
		return;
	win_size = 0;
	win_end = -1;
	win_sum = 0;
	win_sqsum = 0;
	win_level = width;
}

void add_window(float freq){
	win_end = (win_end + 1) & (win_level - 1);
	if(win_size == win_level){
		win_sum -= window[win_end];
		win_sqsum -= window[win_end] * window[win_end];
	}
	else win_size ++;
	window[win_end] = freq;
	win_sum += freq;
	win_sqsum += freq*freq;
}

float get_mean(){
	if(win_size == 0)
		return 0;
	return win_sum / win_size;
}

float get_stdev(){
	if(win_size < 2)
		return 0;
	float mean = win_sum / win_size;
	return sqrt(win_sqsum / win_size - mean * mean);
}

int find_note(int a4, float freq){
	return (int)(log2(freq/a4)*1200+5700.5);
}

char* note_char(int note){
	return notes[note];
}
