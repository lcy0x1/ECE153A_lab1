#ifndef lab2b_h
#define lab2b_h

#include "qpnano/qpn_port.h"

#define MAX_VOLUME 63
#define TIMEOUT 2000

enum VOL_SIG {
	ENCODER_CW = Q_USER_SIG,
	ENCODER_CCW,
	ENCODER_CLICK
};

enum DISP_SIG {
	VOLUME_CHANGE = Q_USER_SIG,
	BUTTON_CLICK,
	TICK
};

extern struct Volume_Tag volume;
extern struct Display_Tag display;

void Lab2B_ctor(void);

void dispatch_volume(enum VOL_SIG sig);

void dispatch_display(enum DISP_SIG, u32 param);

#endif
