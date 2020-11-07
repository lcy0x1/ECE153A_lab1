#ifndef setup_h
#define setup_h

#define RESET_VALUE 100000 // the timer operates at 1kHz
#define DEBOUNCE_TIME 100 // the debounce time is 0.1s
#define BTN_SIGNAL(FLAG) ; // FLAG is the pressed button
#define ENC_SIGNAL(VOL) ; // VOL is the current volume

#define TIMEOUT_SUCCESS 0
#define TIMEOUT_REPLACE 1
#define TIMEOUT_ERROR 2

#define BTN_UP 1
#define BTN_LEFT 2
#define BTN_RIGHT 4
#define BTN_DOWN 8
#define BTN_CENTER 16


/* setup interrupts and lcd, return XST_SUCCESS if initialization is successful */
int setup(void);

/* retrieve the current time */
u32 getTimeGlobal(void);

/* set timeout and wake-up function, return one of the TIMEOUT op-code*/
u32 timeout(u32 rst, void (*callback)(void));

#endif
