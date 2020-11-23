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

XIntc sys_intc;
XGpio sys_btn;
XGpio sys_enc;
XGpio sys_led;
XGpio sys_rgb;
XTmrCtr sys_tmrctr;
XTmrCtr sys_tmralt;

static volatile u32 time_global = 0;
static enum ENC_STATE enc_state = INIT;

int setup_lcd(void);
void timer_handler();
void timer_alt_handler();
void button_handler();
void encoder_handler();
u32 encoder_FSM(u32 enc_flag);
u32 encoder_FSM_switch(u32 enc_flag);

// ---------- setup functions ----------

int setup(void) {
	ASSERT(XIntc_Initialize(&sys_intc, ID_INTC))
	//ASSERT(XIntc_Connect(&sys_intc, INTR_TIMER_0, timer_handler, &sys_tmrctr))
	ASSERT(XIntc_Connect(&sys_intc, INTR_TIMER_1, timer_alt_handler, &sys_tmralt))
	//ASSERT(XIntc_Connect(&sys_intc, INTR_ENC, encoder_handler, &sys_enc))
	//ASSERT(XIntc_Connect(&sys_intc, INTR_BTN, button_handler, &sys_btn))
	ASSERT(XIntc_Start(&sys_intc, XIN_REAL_MODE))
	//XIntc_Enable(&sys_intc, INTR_TIMER_0);
	XIntc_Enable(&sys_intc, INTR_TIMER_1);
	//XIntc_Enable(&sys_intc, INTR_BTN);
	//XIntc_Enable(&sys_intc, INTR_ENC);
	//ASSERT(XGpio_Initialize(&sys_led, ID_LED))
	//ASSERT(XGpio_Initialize(&sys_rgb, ID_RGB))
	//ASSERT(XGpio_Initialize(&sys_btn, ID_BTN))
	//ASSERT(XGpio_Initialize(&sys_enc, ID_ENC))
	//ASSERT(XTmrCtr_Initialize(&sys_tmrctr, ID_TMR_0))
	ASSERT(XTmrCtr_Initialize(&sys_tmralt, ID_TMR_1))
	//XTmrCtr_SetOptions(&sys_tmrctr, ID_TMR_0, XTC_INT_MODE_OPTION | XTC_AUTO_RELOAD_OPTION);
	XTmrCtr_SetOptions(&sys_tmralt, ID_TMR_1,XTC_INT_MODE_OPTION | XTC_AUTO_RELOAD_OPTION);
	//XTmrCtr_SetResetValue(&sys_tmrctr, ID_TMR_0, 0xFFFFFFFF - RESET_VALUE);
	XTmrCtr_SetResetValue(&sys_tmralt, ID_TMR_1, 0xFFFFFFFF - RESET_VALUE);
	microblaze_register_handler(XIntc_DeviceInterruptHandler, ID_INTC);

	//ASSERT(setup_lcd())

	return XST_SUCCESS;
}

void enable_all(void){
	//XTmrCtr_Start(&sys_tmrctr, ID_TMR_0);
	XTmrCtr_Start(&sys_tmralt, ID_TMR_1);
	//XGpio_InterruptEnable(&sys_btn, GPIO_MASK);
	//XGpio_InterruptEnable(&sys_enc, GPIO_MASK);
	//XGpio_InterruptGlobalEnable(&sys_btn);
	//XGpio_InterruptGlobalEnable(&sys_enc);
	microblaze_enable_interrupts();
}

int setup_lcd(void) {
	static XGpio dc;
	static XSpi spi;
	XSpi_Config *spiConfig; /* Pointer to Configuration data */
	u32 controlReg;

	ASSERT(XGpio_Initialize(&dc, XPAR_SPI_DC_DEVICE_ID))
	XGpio_SetDataDirection(&dc, GPIO_MASK, 0x0);
	ASSERT(!(spiConfig = XSpi_LookupConfig(XPAR_SPI_DEVICE_ID)))
	ASSERT(XSpi_CfgInitialize(&spi, spiConfig, spiConfig->BaseAddress))
	XSpi_Reset(&spi);
	controlReg = XSpi_GetControlReg(&spi);
	XSpi_SetControlReg(&spi, (controlReg | XSP_CR_ENABLE_MASK | XSP_CR_MASTER_MODE_MASK) & (~XSP_CR_TRANS_INHIBIT_MASK));
	XSpi_SetSlaveSelectReg(&spi, ~0x01);

	return XST_SUCCESS;
}

// ---------- private: interrupt handlers ----------

void timer_handler() {
	Xuint32 ctrl = XTimerCtr_ReadReg(sys_tmrctr.BaseAddress, 0, XTC_TCSR_OFFSET);
	XTmrCtr_WriteReg(sys_tmrctr.BaseAddress, 0, XTC_TCSR_OFFSET,
			ctrl |XTC_CSR_INT_OCCURED_MASK);
	time_global++;

	intr_timer(time_global);
}

void timer_alt_handler() {
	Xuint32 ctrl = XTimerCtr_ReadReg(sys_tmralt.BaseAddress, 0, XTC_TCSR_OFFSET);
	XTmrCtr_WriteReg(sys_tmralt.BaseAddress, 0, XTC_TCSR_OFFSET,
			ctrl |XTC_CSR_INT_OCCURED_MASK);
}

void encoder_handler() {
	static u32 enc_pushed = 0;
	static u32 enc_prev = 0;

	u32 action = 0;

	u32 enc_flag = XGpio_DiscreteRead(&sys_enc, GPIO_MASK);
	XGpio_InterruptClear(&sys_enc, GPIO_MASK);

	u32 result = encoder_FSM(enc_flag & 3);
	if (enc_pushed == 0 && (enc_flag & 4) > 0) {
		u32 time = time_global;
		if (time - enc_prev > DEBOUNCE_TIME)
			action |= ENC_PUSH;
		enc_prev = time;
	}
	enc_pushed = enc_flag & 4;
	if (result & FSM_OUT_CW)
		action |= ENC_CW;
	if (result & FSM_OUT_CCW)
		action |= ENC_CCW;
	if (action)
		intr_encoder(action);

}

void button_handler() {
	static u32 btn_prev = 0;
	u32 btn_flag = XGpio_DiscreteRead(&sys_btn, GPIO_MASK);
	XGpio_InterruptClear(&sys_btn, GPIO_MASK);
	if (!btn_flag)
		return;
	u32 time = time_global;
	if (time - btn_prev > DEBOUNCE_TIME)
		intr_button(btn_flag);
	btn_prev = time;
}

// ---------- private: internal functions ---------

u32 encoder_FSM(u32 enc_flag) {
	u32 result = encoder_FSM_switch(enc_flag);
	if (!(result & FSM_ERR_SAME))
		enc_state = result & FSM_STATE_BITS;
	return result;
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
// end double bouncing correction code ---------------------------------------

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
// end double bouncing correction code ---------------------------------------

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
// end double bouncing correction code ---------------------------------------

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
// end double bouncing correction code ---------------------------------------
	}
	return FSM_ERR_UNKNOWN;
}


u32 getGlobalTime(void) {
	return time_global;
}
