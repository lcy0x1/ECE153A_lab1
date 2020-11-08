#include "xil_cache.h"
#include "lab2b.h"
#include "bsp.h"

#define QUEUE_SIZE 100

static QEvent volumeQueue[QUEUE_SIZE];
static QEvent displayQueue[QUEUE_SIZE];

QActiveCB const Q_ROM Q_ROM_VAR QF_active[] = {
		{ (QActive *) 0, (QEvent *) 0, 0 },
		{ (QActive *) &display, displayQueue, QUEUE_SIZE },
		{ (QActive *) &volume, volumeQueue, QUEUE_SIZE }
};

Q_ASSERT_COMPILE(QF_MAX_ACTIVE == Q_DIM(QF_active) - 1);

int main() {
	Xil_ICacheInvalidate()
	Xil_ICacheEnable();
	Xil_DCacheInvalidate()
	Xil_DCacheEnable();
	Lab2B_ctor();
	BSP_init();
	QF_run();
	return 0;
}
