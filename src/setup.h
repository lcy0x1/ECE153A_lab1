#ifndef setup_h
#define setup_h

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

u32 getTimeGlobal(void); // retrieve the current time
void setLEDs(u32 flag);

/* NOT USED set timeout and wake-up function, return one of the TIMEOUT op-code*/
u32 timeout(u32 rst, void (*callback)(void));

#endif
