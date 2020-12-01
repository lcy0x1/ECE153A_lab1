#include "header.h"
#include <math.h>

static char notes[12][3]={"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};

int find_note(float freq){
	return (int)(log2(freq/440)*1200+5700.5);
}

char* note_char(int note){
	return notes[note];
}
