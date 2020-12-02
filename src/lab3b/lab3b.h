#ifndef lab3b_h
#define lab3b_h

#include "../qpnano/qpn_port.h"
#include "hardware.h"
#include "display.h"
#include "bsp.h"

enum LAB3B_SIG {
	I_UP = Q_USER_SIG,
	I_DOWN,
	P_OCTAVE,
	P_TUNER,
	P_A4
};

typedef struct LAB3B_TAG {
	QActive super;
	u32 octave;
	u32 a4;
	u32 note;
} LAB3B;

extern struct LAB3B_TAG lab3b;

void Lab3B_ctor(void);

void dispatch(enum LAB3B_SIG sig);

void log_debug_info(void);

#endif
