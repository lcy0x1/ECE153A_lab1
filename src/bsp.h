#ifndef bsp_h
#define bsp_h

#include "xtmrctr.h"
#include "xintc.h"
#include "xparameters.h"
#include "xtmrctr_l.h"
#include "xintc_l.h"
#include "xgpio.h"
#include "xgpio_l.h"
#include "xspi.h"
#include "xspi_l.h"
#include "mb_interface.h"
#include <xbasic_types.h>
#include <xio.h>

#define RESET_VALUE 100000 // the timer operates at 1kHz
#define DEBOUNCE_TIME 100 // the debounce time is 0.1s

#define TIMEOUT_SUCCESS 0
#define TIMEOUT_REPLACE 1
#define TIMEOUT_ERROR 2

#define BTN_UP 1
#define BTN_LEFT 2
#define BTN_RIGHT 4
#define BTN_DOWN 8
#define BTN_CENTER 16

#define ENC_CW 1
#define ENC_CCW 2
#define ENC_PUSH 4

void setTimerInterrupt(void (*intr)(u32)); // parameter: time
void setButtonInterrupt(void (*intr)(u32)); // parameter: button flag
void setEncoderInterrupt(void (*intr)(u32)); // parameter: encoder flag

/* setup interrupts and lcd, return XST_SUCCESS if initialization is successful */
int setup(void);

// ---------- Code below is only used for debug purpose ----------

/* NOT USED: only used for debug purpose
 * retrieve the global time value */
u32 getTimeGlobal(void);

/* NOT USED: only used for debug purpose
 * set LED */
void setLEDs(u32 flag);

/* NOT USED: only used for debug purpose
 * set timeout and wake-up function, return one of the TIMEOUT op-code */
u32 timeout(u32 rst, void (*callback)(void));

#endif
