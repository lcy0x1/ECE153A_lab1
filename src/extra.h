
#include "xtmrctr.h"
#include "xintc.h"
#include "xparameters.h"
#include "xtmrctr_l.h"
#include "xintc_l.h"
#include "xgpio.h"
#include "xgpio_l.h"
#include "mb_interface.h"
#include <xbasic_types.h>
#include <xio.h>

#define ASSERT(X) if(X != XST_SUCCESS) return XST_FAILURE;

// this macro tells the granularity of the timer. currently it works on 125us granularity
// smaller granularity will overburden the processor and make the stop watch unusable
#define GRANULARITY 10

// 80kHz / granularity
#define RESET_VALUE 1250*GRANULARITY

#define FSM_STATE_BITS 7
#define FSM_ERR_SAME 8
#define FSM_ERR_TWO_JUMP 16
#define FSM_ERR_AMBIGIOUS 32
#define FSM_ERR_UNKNOWN 64
#define FSM_OUT_CW 128
#define FSM_OUT_CCW 256
#define RGB_GREEN 2
#define RGB_OFF 0

enum ENC_STATE {
	INIT, CW1, CW2, CW3, CCW1, CCW2, CCW3
};
void rgbLED();
void rgboff();
//LED RGB status check
// the interrupt handler for the timer
void timer_handler();
// the interrupt handler for the encoder
void encoder_handler();

// timer disable
void extra_disable();
// timer enable
void extra_enable();
// initialize everything
// @param intr represents the interrupt to be called during timer update
// all interrupt and register related code are prehandled in timer_handler
int extra_method(void (*intr)(void));
// get and clear the button flags.
// Button cooldown for debouncing is handled here
// it should be called every update
u32 getEncPos();
