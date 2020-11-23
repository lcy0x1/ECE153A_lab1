#include "hardware.h"

#define ASSERT(X) if(X != XST_SUCCESS) return XST_FAILURE;

#define INTR_TIMER_0 XPAR_MICROBLAZE_0_AXI_INTC_AXI_TIMER_0_INTERRUPT_INTR
#define INTR_TIMER_1 XPAR_MICROBLAZE_0_AXI_INTC_AXI_TIMER_1_INTERRUPT_INTR
#define INTR_ENC XPAR_MICROBLAZE_0_AXI_INTC_AXI_GPIO_ENCODER_IP2INTC_IRPT_INTR
#define INTR_BTN XPAR_MICROBLAZE_0_AXI_INTC_AXI_GPIO_BTN_IP2INTC_IRPT_INTR
#define ID_INTC XPAR_MICROBLAZE_0_AXI_INTC_DEVICE_ID
#define ID_TMR_0 XPAR_AXI_TIMER_0_DEVICE_ID
#define ID_TMR_1 XPAR_AXI_TIMER_1_DEVICE_ID
#define ID_ENC XPAR_AXI_GPIO_ENCODER_DEVICE_ID
#define ID_BTN XPAR_AXI_GPIO_BTN_DEVICE_ID
#define ID_LED XPAR_AXI_GPIO_LED_DEVICE_ID
#define ID_RGB XPAR_AXI_GPIO_RGBLEDS_DEVICE_ID
#define GPIO_MASK XGPIO_IR_CH1_MASK

XIntc sys_intc;
XTmrCtr sys_tmralt;

void tmr_handler();

// ---------- setup functions ----------

int init(void) {
	ASSERT(XIntc_Initialize(&sys_intc, ID_INTC))
	ASSERT(XIntc_Connect(&sys_intc, INTR_TIMER_0, tmr_handler, &sys_tmralt))
	ASSERT(XIntc_Start(&sys_intc, XIN_REAL_MODE))
	XIntc_Enable(&sys_intc, INTR_TIMER_0);
	ASSERT(XTmrCtr_Initialize(&sys_tmralt, ID_TMR_0))
	XTmrCtr_SetOptions(&sys_tmralt, ID_TMR_0,XTC_INT_MODE_OPTION | XTC_AUTO_RELOAD_OPTION);
	XTmrCtr_SetResetValue(&sys_tmralt, ID_TMR_0, 0xFFFFFFFF - RESET_VALUE);
	microblaze_register_handler(XIntc_DeviceInterruptHandler, ID_INTC);
	XTmrCtr_Start(&sys_tmralt, ID_TMR_0);

	return XST_SUCCESS;
}
void tmr_handler() {
	Xuint32 ctrl = XTimerCtr_ReadReg(sys_tmralt.BaseAddress, 0, XTC_TCSR_OFFSET);
	analysis();
	XTmrCtr_WriteReg(sys_tmralt.BaseAddress, 0, XTC_TCSR_OFFSET,
			ctrl |XTC_CSR_INT_OCCURED_MASK);
}
