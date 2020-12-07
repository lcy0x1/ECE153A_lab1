#ifndef display_h
#define display_h

#include "../lcd/lcd.h"

enum PAGE {
	OCTAVE,
	TUNER,
	A4
};

enum ACTION {
	DRAW,
	ERASE,
	UPDATE
};

// initialize LCD and draw background
void init_background(void);

// post a command to the pending command list
// specific data should be retrieved from the QActive struct
void post_command(enum ACTION act, enum PAGE page);

// execute all pending commands
void execute_command(void);

#endif
