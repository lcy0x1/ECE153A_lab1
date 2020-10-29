#include "extra.h"
#include <stdlib.h>

#define INTR_TIMER_0 XPAR_MICROBLAZE_0_AXI_INTC_AXI_TIMER_0_INTERRUPT_INTR
#define INTR_ENC XPAR_MICROBLAZE_0_AXI_INTC_AXI_GPIO_ENCODER_IP2INTC_IRPT_INTR
#define ID_INTC XPAR_MICROBLAZE_0_AXI_INTC_DEVICE_ID
#define ID_ENC XPAR_AXI_GPIO_ENCODER_DEVICE_ID
#define ID_LED XPAR_AXI_GPIO_LED_DEVICE_ID
#define ID_RGB XPAR_AXI_GPIO_RGBLEDS_DEVICE_ID
#define ENC_MASK XGPIO_IR_CH1_MASK

XIntc sys_intc;
XGpio sys_enc;
XGpio sys_led;
XGpio sys_rgb;

static u32 sub_timer_count = 0;
static u32 last_push = 0;
static volatile u32 timer_count = 0;
static int enc_pos = 0;
static int enc_pushed = 0, led_off = 0;
static enum ENC_STATE enc_state = INIT;

void updateBaseLoop() {
	if (sub_timer_count == 0 && timer_count == 0)
		XGpio_DiscreteWrite(&sys_led, 1, 1);
	sub_timer_count++;
	if ((sub_timer_count & 0xFF) == 0) {
		timer_count++;
	}
	if ((sub_timer_count & 0x1FFFF) == 0) {
		XGpio_DiscreteWrite(&sys_rgb, 1, (sub_timer_count >> 16) & 2);
	}
}

u32 encoder_FSM_switch(u32 enc_flag) {
	switch (enc_flag) {
	case 3:
		switch (enc_state) {
		case CCW3:
			return INIT | FSM_OUT_CCW;
		case CW3:
			return INIT | FSM_OUT_CW;
		case CCW1:
		case CW1:
			return INIT;

// below is extra correction code that solves double bouncing problem -------
		case INIT:								// double bouncing correction
			return FSM_ERR_SAME;				// double bouncing correction
		case CCW2:								// double bouncing correction
			return INIT | FSM_ERR_TWO_JUMP | FSM_OUT_CCW;// double bouncing correction
		case CW2:								// double bouncing correction
			return INIT | FSM_ERR_TWO_JUMP | FSM_OUT_CW;// double bouncing correction
		case AMB0:								// double bouncing correction
			return INIT | FSM_ERR_AMBIGIOUS;	// double bouncing correction
		}										// double bouncing correction
// end double boncing correction code ---------------------------------------

	case 2:
		switch (enc_state) {
		case CW2: // pin B 0->1
		case AMB0:
			return CW3;
		case INIT: // pin A 1->0
		case CCW2: //  pin B 0->1
			return CCW1;

// below is extra correction code that solves double bouncing problem -------
		case CW3:								// double bouncing correction
		case CCW1:								// double bouncing correction
			return FSM_ERR_SAME;				// double bouncing correction
		case CW1:								// double bouncing correction
			return CW3 | FSM_ERR_TWO_JUMP;		// double bouncing correction
		case CCW3:								// double bouncing correction
			return CCW1 | FSM_ERR_TWO_JUMP;		// double bouncing correction
		}										// double bouncing correction
// end double boncing correction code ---------------------------------------

	case 1:
		switch (enc_state) {
		case CCW2: // pin A 0->1
		case AMB0:
			return CCW3;
		case INIT: // pin B 1->0
		case CW2: //  pin A 0->1
			return CW1;

// below is extra correction code that solves double bouncing problem -------
		case CCW3:								// double bouncing correction
		case CW1:								// double bouncing correction
			return FSM_ERR_SAME;				// double bouncing correction
		case CCW1:								// double bouncing correction
			return CCW3 | FSM_ERR_TWO_JUMP;		// double bouncing correction
		case CW3:								// double bouncing correction
			return CW1 | FSM_ERR_TWO_JUMP;		// double bouncing correction
		}										// double bouncing correction
// end double boncing correction code ---------------------------------------

	case 0:
		switch (enc_state) {
		case CCW1: // pin 1 1->0
		case CCW3: // pin 2 1->0
			return CCW2;
		case CW1: // pin 2 1->0
		case CW3: // pin 1 1->0
			return CW2;

// below is extra correction code that solves double bouncing problem -------
		case CCW2:								// double bouncing correction
		case CW2:								// double bouncing correction
		case AMB0:								// double bouncing correction
			return FSM_ERR_SAME;				// double bouncing correction
		case INIT:								// double bouncing correction
			return AMB0 | FSM_ERR_AMBIGIOUS;	// double bouncing correction
		}										// double bouncing correction
// end double boncing correction code ---------------------------------------
	}
	return FSM_ERR_UNKNOWN;
}

u32 encoder_FSM(u32 enc_flag) {
	u32 result = encoder_FSM_switch(enc_flag);
	if (!(result & FSM_ERR_SAME))
		enc_state = result & FSM_STATE_BITS;
	return result;
}

void encoder_handler() {
	u32 enc_flag = XGpio_DiscreteRead(&sys_enc, 1);
	u32 result = encoder_FSM(enc_flag & 3);
	if (enc_pushed == 0 && (enc_flag & 4) > 0) {
		u32 time = timer_count;
		if (time - last_push > 100)
			led_off = 1 - led_off;
		last_push = time;
	}
	enc_pushed = enc_flag & 4;
	if (!led_off) {
		if (result & FSM_OUT_CW)
			enc_pos--;
		if (result & FSM_OUT_CCW)
			enc_pos++;
	}
	enc_pos = enc_pos & 0xF;
	u32 led = led_off ? 0 : 1 << enc_pos;
	XGpio_DiscreteWrite(&sys_led, 1, led);

	XGpio_InterruptClear(&sys_enc, ENC_MASK); // clear the interrupt flag
}

int extra_method(void) {

	ASSERT(XIntc_Initialize(&sys_intc, ID_INTC))
	ASSERT(XIntc_Connect(&sys_intc, INTR_ENC, encoder_handler, &sys_enc))
	ASSERT(XIntc_Start(&sys_intc, XIN_REAL_MODE))
	XIntc_Enable(&sys_intc, INTR_ENC);
	ASSERT(XGpio_Initialize(&sys_led, ID_LED))
	ASSERT(XGpio_Initialize(&sys_rgb, ID_RGB))
	ASSERT(XGpio_Initialize(&sys_enc, ID_ENC))
	XGpio_InterruptEnable(&sys_enc, ENC_MASK);
	XGpio_InterruptGlobalEnable(&sys_enc);
	microblaze_register_handler(XIntc_DeviceInterruptHandler, ID_INTC);
	microblaze_enable_interrupts();

	return XST_SUCCESS;
}
