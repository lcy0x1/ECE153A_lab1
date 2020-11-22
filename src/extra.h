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

//1600 cycles, or 0.2s cool down for all buttons
//the reason we use 0.2s is that many bouncing persists over 0.1s
//though we admit that human input can also be faster than 0.2s
//so we are filtering out some human input as well
#define DEBOUNCE_COOLDOWN 1600

// this macro tells the granularity of the stop watch. currently it works on 1ms granularity
// smaller granularity will overburden the processor and make the stop watch unusable
#define GRANULARITY 10

// 80kHz / granularity
#define RESET_VALUE 1250*GRANULARITY

// Below is the configuration macros

// enable multi-function start to detect debouncing
#define MULTI_FUNCTION_START 1

// enable software debounce
#define SOFTWARE_DEBOUNCE 1

// enable debounce stats collection. It prints in timer interrupt, clear it when not testing
// DO NOT ENABLE IT IN REAL RUN
#define DEBOUNCE_STATS_COLLECT 0


// the interrupt handler for the timer
void timer_handler();
// the interrupt handler for the button
void button_handler();

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
u32 getAndClearBtnFlag();
