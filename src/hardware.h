#ifndef hardware_h
#define hardware_h

#include "xtmrctr.h"
#include "xintc.h"
#include "xparameters.h"
#include "xgpio.h"
#include "xspi.h"
#include <xbasic_types.h>

#define RESET_VALUE 100000 // the timer operates at 1kHz

void analysis();

int init(void);


#endif
