#include "lab2b.h"

typedef struct Volume_Tag {
	QActive super;
	u32 volume;
	u32 cache_volume;
} Volume;

typedef struct Display_Tag {
	QActive super;
	u32 button;
	u32 volume;
	u32 volume_time;
	u32 button_time;

	u32 update_flag;
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
	QActive_postISR((QActive *)&volume, sig, getGlobalTime());
}

void dispatch_display(enum DISP_SIG sig, u32 param) {
	QActive_postISR((QActive *)&display, sig, param);
}

void Lab2B_ctor(void) {
	QActive_ctor(&display.super, (QStateHandler ) &Disp_init);
	QActive_ctor(&volume.super, (QStateHandler ) &Vol_init);
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
		if (me->volume < MAX_VOLUME) {
			me->volume++;
		}
		dispatch_display(VOLUME_CHANGE, me->volume);
		return Q_TRAN(&Vol_enable);
	}
	case ENCODER_CCW: {
		if (me->volume > 0) {
			me->volume--;
		}
		dispatch_display(VOLUME_CHANGE, me->volume);
		return Q_TRAN(&Vol_enable);
	}
	}
	return Q_SUPER(&QHsm_top);
}

QState Vol_enable(Volume *me) {
	switch (Q_SIG(me)) {
	case ENCODER_CLICK: {
		me->cache_volume = me->volume;
		me->volume = 0;
		dispatch_display(VOLUME_CHANGE, me->volume);
		return Q_TRAN(&Vol_disable);
	}
	}
	return Q_SUPER(&Vol_base);
}

QState Vol_disable(Volume *me) {
	switch (Q_SIG(me)) {
	case ENCODER_CLICK: {
		me->volume = me->cache_volume;
		dispatch_display(VOLUME_CHANGE, me->volume);
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
		return Q_HANDLED();
	}
	case Q_EXIT_SIG: {
		return Q_HANDLED();
	}
	case VOLUME_CHANGE: {
		me->volume = Q_PAR(me);
		me->volume_time = TIMEOUT;
		me->update_flag |= UPDATE_VOL_ON;
		return Q_HANDLED();
	}
	case BUTTON_CLICK: {
		me->button = Q_PAR(me);
		me->button_time = TIMEOUT;
		me->update_flag |= UPDATE_TXT_ON;
		return Q_HANDLED();
	}
	case TICK: {
		if (me->volume_time > 0) {
			me->volume_time--;
			if(me->volume_time == 0)
				me->update_flag |= UPDATE_VOL_OFF;
		}
		if (me->button_time > 0) {
			me->button_time--;
			if(me->button_time == 0)
				me->update_flag |= UPDATE_TXT_OFF;
		}
		if (me->update_flag)
			flush(me->update_flag, me->volume, me->button);
		me->update_flag = 0;
		return Q_HANDLED();
	}
	}
	return Q_SUPER(&QHsm_top);
}

#include <stdio.h>

void log_debug_info(void){
	print("--- DEBUG INFO ---\n\r");
	printf("queue size - volume: %d\n\r",volume.super.nUsed);
	printf("queue size - display: %d\n\r",display.super.nUsed);
	printf("volume queue: \n\r");
	QActiveCB cb = QF_active[volume.super.prio];
	char* strs[8] = {"(null)","entry","exit","init","timeout","cw","ccw","click"};
	for(int i=0;i<volume.super.nUsed;i++){
		QEvent ev = cb.queue[(volume.super.head + i) % cb.end];
		printf("event %d: %s - %d\n\r",i,strs[ev.sig],ev.par);
	}
	cb = QF_active[display.super.prio];
	strs[5] = "vol";
	strs[6] = "btn";
	strs[7] = "tick";
	printf("display queue: \n\r");
	for(int i=0;i<display.super.nUsed;i++){
		QEvent ev = cb.queue[(display.super.head - i + cb.end) % cb.end];
		printf("event %d: %s - %d\n\r",i,strs[ev.sig],ev.par);
	}


}
