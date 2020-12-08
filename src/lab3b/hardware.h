#ifndef hardware_h
#define hardware_h

#include "xtmrctr.h"
#include "xintc.h"
#include "xparameters.h"
#include "xgpio.h"
#include "xspi.h"
#include <xbasic_types.h>

#define RESET_VALUE 32768 // the timer operates at 3kHz
#define DEBOUNCE_TIME 100 // the debounce time is 0.1s

#define BTN_UP 1
#define BTN_LEFT 2
#define BTN_RIGHT 4
#define BTN_DOWN 8
#define BTN_CENTER 16

#define ENC_CW 1
#define ENC_CCW 2
#define ENC_PUSH 4

void intr_timer(u32 time); // implemented in bsp.c, slow sampler
void intr_button(u32 flag); // implemented in bsp.c, parse button flag intp HSM signal
void intr_encoder(u32 flag); // implemented in bsp.c, parse encoder flag intp HSM signal

/* setup interrupts and lcd, return XST_SUCCESS if initialization is successful */
int setup(void);
void enable_all(void); // enable all interrupts
u32 getGlobalTime(void); // for debug only
u32 getMicValue(void); // used by timer to act as a slow sampler
void show_led(u32 val); // for debug only

#endif
