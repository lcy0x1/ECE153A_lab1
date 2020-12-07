#include "lab3b.h"

LAB3B lab3b;

static QState Q_init(LAB3B *me);
static QState Q_base(LAB3B *me);
static QState Q_tuner(LAB3B *me);
static QState Q_octave(LAB3B *me);
static QState Q_a4(LAB3B *me);

void Lab3B_ctor(void) {
	QActive_ctor(&lab3b.super, (QStateHandler ) &Q_init);
	lab3b.a4 = 440;
	lab3b.octave = 4;
	lab3b.freq = 0;
}

void dispatch(enum LAB3B_SIG sig) {
	QActive_postISR((QActive *) &lab3b, sig);
}

QState Q_init(LAB3B *me) {
	return Q_TRAN(&Q_tuner);
}

QState Q_base(LAB3B *me) {
	switch (Q_SIG(me)) {
	case Q_INIT_SIG:
		return Q_TRAN(&Q_tuner);
	case Q_ENTRY_SIG:
	case Q_EXIT_SIG:
	case I_UP:
	case I_DOWN:
		return Q_HANDLED();
	case P_OCTAVE:
		return Q_TRAN(&Q_octave);
	case P_TUNER:
		return Q_TRAN(&Q_tuner);
	case P_A4:
		return Q_TRAN(&Q_a4);
	}
	return Q_SUPER(&QHsm_top);
}

QState Q_octave(LAB3B *me) {
	switch (Q_SIG(me)) {
	case Q_ENTRY_SIG: {
		post_command(DRAW, OCTAVE);
		return Q_HANDLED();
	}
	case Q_EXIT_SIG: {
		post_command(ERASE, OCTAVE);
		return Q_HANDLED();
	}
	case I_UP: {
		me->octave++;
		if(me->octave > 10)
			me->octave = 0;
		post_command(UPDATE, OCTAVE);
		return Q_HANDLED();
	}
	case I_DOWN: {
		me->octave--;
		if(me->octave > 10)
			me->octave = 10;
		post_command(UPDATE, OCTAVE);
		return Q_HANDLED();
	}
	}
	return Q_SUPER(&Q_base);
}

QState Q_tuner(LAB3B *me) {
	switch (Q_SIG(me)) {
	case Q_ENTRY_SIG: {
		post_command(DRAW, TUNER);
		set_FFT_enable(1);
		return Q_HANDLED();
	}
	case Q_EXIT_SIG: {
		post_command(ERASE, TUNER);
		set_FFT_enable(0);
		return Q_HANDLED();
	}
	}
	return Q_SUPER(&Q_base);
}

QState Q_a4(LAB3B *me) {
	switch (Q_SIG(me)) {
	case Q_ENTRY_SIG: {
		post_command(DRAW, A4);
		return Q_HANDLED();
	}
	case Q_EXIT_SIG: {
		post_command(ERASE, A4);
		return Q_HANDLED();
	}
	case I_UP: {
		if (me->a4 < 460) {
			me->a4++;
			post_command(UPDATE, A4);
		}
		return Q_HANDLED();
	}
	case I_DOWN: {
		if (me->a4 > 420) {
			me->a4--;
			post_command(UPDATE, A4);
		}
		return Q_HANDLED();
	}
	}
	return Q_SUPER(&Q_base);
}

#include <stdio.h>

void log_debug_info(void){
	print("--- DEBUG INFO ---\n\r");
	printf("queue size: %d\n\r",lab3b.super.nUsed);
	printf("queue: \n\r");
	QActiveCB cb = QF_active[lab3b.super.prio];
	char* strs[10] = {"(null)","entry","exit","init","timeout","up","down","octave","tuner","a4"};
	for(int i=0;i<lab3b.super.nUsed;i++){
		QEvent ev = cb.queue[(lab3b.super.head + i) % cb.end];
		printf("event %d: %s\n\r",i,strs[ev.sig]);
	}
}



