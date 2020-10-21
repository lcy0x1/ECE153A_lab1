#include "extra.h"
#include <stdlib.h>

#define INTR_TIMER_0 XPAR_MICROBLAZE_0_AXI_INTC_AXI_TIMER_0_INTERRUPT_INTR
#define INTR_BTN XPAR_MICROBLAZE_0_AXI_INTC_AXI_GPIO_BTN_IP2INTC_IRPT_INTR
#define ID_INTC XPAR_MICROBLAZE_0_AXI_INTC_DEVICE_ID
#define ID_BTN XPAR_AXI_GPIO_BTN_DEVICE_ID
#define ID_LED XPAR_AXI_GPIO_LED_DEVICE_ID
#define BTN_MASK XGPIO_IR_CH1_MASK

XIntc sys_intc;
XTmrCtr sys_tmrctr;
Xuint32 data;
XGpio sys_btn;
XGpio sys_led;
volatile u32 btn_flag = 0;

void (*intr_tmr)(void);

u32 getAndClearBtnFlag() {
	u32 ans = btn_flag;
	btn_flag = 0;
	return ans;
}

void timer_handler() {
	Xuint32 ControlStatusReg;
	ControlStatusReg =
	XTimerCtr_ReadReg(sys_tmrctr.BaseAddress, 0, XTC_TCSR_OFFSET);

	intr_tmr();

	XTmrCtr_WriteReg(sys_tmrctr.BaseAddress, 0, XTC_TCSR_OFFSET,
			ControlStatusReg |XTC_CSR_INT_OCCURED_MASK);

}

/* Since Bounce might happen, the button interrupt handler should be as short as possible */
void button_handler() {
	// use OR operation to read only press, not release
	btn_flag |= XGpio_DiscreteRead(&sys_btn, 1);
	XGpio_DiscreteWrite(&sys_led, 1, btn_flag); // for testing only
	XGpio_InterruptClear(&sys_btn, BTN_MASK);
}

void extra_disable() {
	XIntc_Disable(&sys_intc, INTR_TIMER_0);
}

void extra_enable() {
	XIntc_Enable(&sys_intc, INTR_TIMER_0);
}

int extra_method(void (*intr_timer)(void)) {
	intr_tmr = intr_timer;

	ASSERT(XIntc_Initialize(&sys_intc, ID_INTC))
	ASSERT(XIntc_Connect(&sys_intc, INTR_TIMER_0, timer_handler, &sys_tmrctr))
	ASSERT(XIntc_Connect(&sys_intc, INTR_BTN, button_handler, &sys_btn))
	ASSERT(XIntc_Start(&sys_intc, XIN_REAL_MODE))
	XIntc_Enable(&sys_intc, INTR_TIMER_0);
	XIntc_Enable(&sys_intc, INTR_BTN);
	ASSERT(XGpio_Initialize(&sys_led, ID_LED))
	ASSERT(XGpio_Initialize(&sys_btn, ID_BTN))
	ASSERT(XTmrCtr_Initialize(&sys_tmrctr, ID_INTC))
	XTmrCtr_SetOptions(&sys_tmrctr, 0,
	XTC_INT_MODE_OPTION | XTC_AUTO_RELOAD_OPTION);
	XTmrCtr_SetResetValue(&sys_tmrctr, 0, 0xFFFFFFFF - RESET_VALUE);
	XTmrCtr_Start(&sys_tmrctr, 0);
	XGpio_InterruptEnable(&sys_btn, BTN_MASK);
	XGpio_InterruptGlobalEnable(&sys_btn);
	microblaze_register_handler(XIntc_DeviceInterruptHandler, ID_INTC);
	microblaze_enable_interrupts();

	return XST_SUCCESS;
}
