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

#define FSM_STATE_BITS 7
#define FSM_ERR_SAME 8
#define FSM_ERR_TWO_JUMP 16
#define FSM_ERR_AMBIGIOUS 32
#define FSM_ERR_UNKNOWN 64
#define FSM_OUT_CW 128
#define FSM_OUT_CCW 256

enum ENC_STATE {
	INIT, CW1, CW2, CW3, CCW1, CCW2, CCW3, AMB0
};

// the interrupt handler for the encoder
void encoder_handler();

void extra_disable();

void extra_enable();

int extra_method(void);

void updateBaseLoop();
