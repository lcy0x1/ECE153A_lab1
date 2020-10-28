#include "extra.h"
#include <stdlib.h>

#define INTR_TIMER_0 XPAR_MICROBLAZE_0_AXI_INTC_AXI_TIMER_0_INTERRUPT_INTR
#define INTR_ENC XPAR_MICROBLAZE_0_AXI_INTC_AXI_GPIO_ENCODER_IP2INTC_IRPT_INTR
#define ID_INTC XPAR_MICROBLAZE_0_AXI_INTC_DEVICE_ID
#define ID_ENC XPAR_AXI_GPIO_ENCODER_DEVICE_ID
#define ID_LED XPAR_AXI_GPIO_LED_DEVICE_ID
#define ENC_MASK XGPIO_IR_CH1_MASK

XIntc sys_intc;
XTmrCtr sys_tmrctr;
Xuint32 data;
XGpio sys_enc;
XGpio sys_led;

void (*intr_tmr)(void);

volatile int tmr_timer = 0, last_time = 0;
volatile int enc_pos = 0;
int enc_pushed = 0, led_off = 0;
enum ENC_STATE enc_state = INIT;

// software debounce and debounce stat collection can be enabled or disabled in extra.h
u32 getEncPos() {
	return enc_pos;
}

// pre-handle interrupt related code and call intr_tmr. If intr_tmr is null, it is dead
void timer_handler() {
	Xuint32 ControlStatusReg;
	ControlStatusReg =
	XTimerCtr_ReadReg(sys_tmrctr.BaseAddress, 0, XTC_TCSR_OFFSET);

	tmr_timer++;
	//intr_tmr();

	XTmrCtr_WriteReg(sys_tmrctr.BaseAddress, 0, XTC_TCSR_OFFSET,
			ControlStatusReg |XTC_CSR_INT_OCCURED_MASK);

}

// update enc_state and potentially enc_pos.
// return 0 for valid transition
// return 1 for invalid transition
// return 2 for same state
// invalid transitions are usually the result of missed interrupt
u32 encoder_FSM(u32 enc_flag) {
	if (enc_flag == 3)
		if (enc_state == CCW3) { // pin 1 0->1
			enc_state = INIT;
			return FSM_OUT_CCW;
		} else if (enc_state == CW3) { // pin 2 0->1
			enc_state = INIT;
			return FSM_OUT_CW;
		} else if (enc_state == CW1 || enc_state == CCW1) // pin 1 0->1 or pin 2 0->1
			enc_state = INIT;
		else if (enc_state == INIT)
			return FSM_ERR_SAME;
		else if (enc_state == CCW2) {
			enc_state = INIT;
			return FSM_OUT_CCW | FSM_ERR_TWO_JUMP;
		} else if (enc_state == CW2) {
			enc_state = INIT;
			enc_pos--;
			return FSM_OUT_CW | FSM_ERR_TWO_JUMP;
		} else
			return FSM_ERR_UNKNOWN;
	else if (enc_flag == 2)
		if (enc_state == CW2) // pin 1 0->1
			enc_state = CW3;
		else if (enc_state == INIT || enc_state == CCW2) // pin 2 1->0 or pin 1 0->1
			enc_state = CCW1;
		else if (enc_state == CW3 || enc_state == CCW1)
			return FSM_ERR_SAME;
		else if (enc_state == CW1) {
			enc_state = CW3;
			return FSM_ERR_TWO_JUMP;
		} else if (enc_state == CCW3) {
			enc_state = CCW1;
			return FSM_ERR_TWO_JUMP;
		} else
			return FSM_ERR_UNKNOWN;
	else if (enc_flag == 1)
		if (enc_state == CCW2) // pin 2 0->1
			enc_state = CCW3;
		else if (enc_state == INIT || enc_state == CW2) // pin 1 1->0 or pin 2 0->1
			enc_state = CW1;
		else if (enc_state == CCW3 || enc_state == CW1)
			return FSM_ERR_SAME;
		else if (enc_state == CCW1) {
			enc_state = CCW3;
			return FSM_ERR_TWO_JUMP;
		} else if (enc_state == CW3) {
			enc_state = CW1;
			return FSM_ERR_TWO_JUMP;
		} else
			return FSM_ERR_UNKNOWN;
	else if (enc_flag == 0)
		if (enc_state == CCW1 || enc_state == CCW3) // pin 1 1->0 or pin 2 1->0
			enc_state = CCW2;
		else if (enc_state == CW1 || enc_state == CW3) // pin 2 1->0 or pin 1 1->0
			enc_state = CW2;
		else if (enc_state == CCW2 || enc_state == CW2)
			return FSM_ERR_SAME;
		else
			return FSM_ERR_AMBIGIOUS;
	else
		return FSM_ERR_UNKNOWN;
	return FSM_ERR_NONE;
}

void encoder_handler() {
	u32 enc_flag = XGpio_DiscreteRead(&sys_enc, 1);
	u32 enc_prev = enc_state;
	u32 result = encoder_FSM(enc_flag & 3);
	if (enc_pushed == 0 && (enc_flag & 4) > 0) {
		led_off = 1 - led_off;
	}
	enc_pushed = enc_flag & 4;
	if (!led_off) {
		if (result & FSM_OUT_CW)
			enc_pos--;
		if (result & FSM_OUT_CCW)
			enc_pos++;
	}
	u32 led = led_off ? 0 : 1 << (enc_pos & 0xF);
	XGpio_DiscreteWrite(&sys_led, 1, led);

	//u32 led_debug = enc_flag | enc_prev << 4 | enc_state << 8 | result << 12;
	//if (result != 0 && result != 2)
	//	XGpio_DiscreteWrite(&sys_led, 1, led_debug);

	XGpio_InterruptClear(&sys_enc, ENC_MASK); // clear the interrupt flag
}

void extra_disable() {
	XIntc_Disable(&sys_intc, INTR_TIMER_0);
}

void extra_enable() {
	XIntc_Enable(&sys_intc, INTR_TIMER_0);
}

int extra_method(void (*intr)(void)) {
	intr_tmr = intr;

	ASSERT(XIntc_Initialize(&sys_intc, ID_INTC))
	ASSERT(XIntc_Connect(&sys_intc, INTR_TIMER_0, timer_handler, &sys_tmrctr))
	ASSERT(XIntc_Connect(&sys_intc, INTR_ENC, encoder_handler, &sys_enc))
	ASSERT(XIntc_Start(&sys_intc, XIN_REAL_MODE))
	XIntc_Enable(&sys_intc, INTR_TIMER_0);
	XIntc_Enable(&sys_intc, INTR_ENC);
	ASSERT(XGpio_Initialize(&sys_led, ID_LED))
	ASSERT(XGpio_Initialize(&sys_enc, ID_ENC))
	ASSERT(XTmrCtr_Initialize(&sys_tmrctr, ID_INTC))
	XTmrCtr_SetOptions(&sys_tmrctr, 0,
	XTC_INT_MODE_OPTION | XTC_AUTO_RELOAD_OPTION);
	XTmrCtr_SetResetValue(&sys_tmrctr, 0, 0xFFFFFFFF - RESET_VALUE);
	XTmrCtr_Start(&sys_tmrctr, 0);
	XGpio_InterruptEnable(&sys_enc, ENC_MASK);
	XGpio_InterruptGlobalEnable(&sys_enc);
	microblaze_register_handler(XIntc_DeviceInterruptHandler, ID_INTC);
	microblaze_enable_interrupts();

	return XST_SUCCESS;
}
