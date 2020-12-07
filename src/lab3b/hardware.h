#ifndef hardware_h
#define hardware_h

#include "xtmrctr.h"
#include "xintc.h"
#include "xparameters.h"
#include "xgpio.h"
#include "xspi.h"
#include <xbasic_types.h>

#define RESET_VALUE 65536 // the timer operates at 1.5kHz
#define DEBOUNCE_TIME 100 // the debounce time is 0.1s

#define BTN_UP 1
#define BTN_LEFT 2
#define BTN_RIGHT 4
#define BTN_DOWN 8
#define BTN_CENTER 16

#define ENC_CW 1
#define ENC_CCW 2
#define ENC_PUSH 4

void intr_timer(u32 time);
void intr_button(u32 flag);
void intr_encoder(u32 flag);

/* setup interrupts and lcd, return XST_SUCCESS if initialization is successful */
int setup(void);
void enable_all(void);
u32 getGlobalTime(void);
u32 getMicValue(void);

#endif
