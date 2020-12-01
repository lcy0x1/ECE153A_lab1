#include "note.h"
#include <math.h>
//#include "lcd.h"

//array to store note names for findNote
static char notes[12][3]={"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};

//finds and prints note of frequency and deviation from note
int findNote(float f) {

	int ans = round(log2(f/440)*1200+5700);

	return ans;
}
