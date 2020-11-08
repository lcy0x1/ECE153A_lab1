#include "lab2b.h"
#include "display.h"

typedef struct Volume_Tag {
	QHsm super;
	u32 volume;
} Volume;

typedef struct Display_Tag {
	QHsm super;
	u32 param;
	u32 button;
	u32 volume;
	u32 volume_time;
	u32 button_time;
} Display;

Volume volume;
Display display;

/* Setup state machines */
/**********************************************************************/
static QState Vol_init(Volume *me);
static QState Vol_base(Volume *me);
static QState Vol_enable(Volume *me);
static QState Vol_disable(Volume *me);
static QState Disp_init(Display *me);
static QState Disp_on(Display *me);

void update_display();

/**********************************************************************/

void dispatch_volume(enum VOL_SIG sig) {
	Q_SIG(&volume) = sig;
	QHsm_dispatch((QHsm *) &volume);
}

void dispatch_display(enum DISP_SIG sig, u32 param) {
	Q_SIG(&display) = sig;
	display.param = param;
	QHsm_dispatch((QHsm *) &display);
}

void Lab2B_ctor(void) {
	QHsm_ctor(&display.super, (QStateHandler ) &Disp_init);
	QHsm_init((QHsm *) &display);
	QHsm_ctor(&volume.super, (QStateHandler ) &Vol_init);
	QHsm_init((QHsm *) &volume);
}

QState Vol_init(Volume *me) {
	return Q_TRAN(&Vol_enable);
}

QState Vol_base(Volume *me) {
	switch (Q_SIG(me)) {
	case Q_INIT_SIG: {
		return Q_TRAN(&Vol_enable);
	}
	case Q_ENTRY_SIG: {
		return Q_HANDLED();
	}
	case Q_EXIT_SIG: {
		return Q_HANDLED();
	}
	case ENCODER_CW: {
		return Q_HANDLED();
	}
	case ENCODER_CCW: {
		return Q_HANDLED();
	}
	}
	return Q_SUPER(&QHsm_top);
}

QState Vol_enable(Volume *me) {
	switch (Q_SIG(me)) {
	case ENCODER_CW: {
		if (me->volume < MAX_VOLUME) {
			me->volume++;
		}
		dispatch_display(VOLUME_CHANGE, me->volume);
		return Q_HANDLED();
	}
	case ENCODER_CCW: {
		if (me->volume > 0) {
			me->volume--;
		}
		dispatch_display(VOLUME_CHANGE, me->volume);
		return Q_HANDLED();
	}
	case ENCODER_CLICK: {
		return Q_TRAN(&Vol_disable);
	}
	}
	return Q_SUPER(&Vol_base);
}

QState Vol_disable(Volume *me) {
	switch (Q_SIG(me)) {
	case Q_ENTRY_SIG: {
		dispatch_display(VOLUME_CHANGE, 0);
		return Q_HANDLED();
	}
	case Q_EXIT_SIG: {
		dispatch_display(VOLUME_CHANGE, me->volume);
		return Q_HANDLED();
	}
	case ENCODER_CLICK: {
		return Q_TRAN(&Vol_enable);
	}
	}
	return Q_SUPER(&Vol_base);
}

QState Disp_init(Display *me) {
	return Q_TRAN(&Disp_on);
}

QState Disp_on(Display *me) {
	switch (Q_SIG(me)) {
	case Q_INIT_SIG: {
		return Q_HANDLED();
	}
	case Q_ENTRY_SIG: {
		update_display();
		return Q_HANDLED();
	}
	case Q_EXIT_SIG: {
		return Q_HANDLED();
	}
	case VOLUME_CHANGE: {
		me->volume = me->param;
		me->param = 0;
		me->volume_time = TIMEOUT;
		update_display();
		return Q_HANDLED();
	}
	case BUTTON_CLICK: {
		me->button = me->param;
		me->param = 0;
		me->button_time = TIMEOUT;
		update_display();
		return Q_HANDLED();
	}
	case TICK: {
		u32 should_update = 0;
		if (me->volume_time > 0) {
			me->volume_time--;
			should_update |= me->volume_time == 0;
		}
		if (me->button_time > 0) {
			me->button_time--;
			should_update |= me->button_time == 0;
		}
		if (should_update)
			update_display();
		return Q_HANDLED();
	}
	}
	return Q_SUPER(&QHsm_top);
}

void update_display(void) {
	drawBG();
	if (display.volume_time > 0)
		drawVolume(display.volume);
	if (display.button_time > 0)
		drawText(display.button);
	flush();
}
