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

#define GRANULARITY 10
#define RESET_VALUE 1250*GRANULARITY // 80kHz / granularity

void timer_handler();
void button_handler();
void extra_disable();
void extra_enable();
int extra_method(void (*intr_tmr)(void));
u32 getAndClearBtnFlag();
