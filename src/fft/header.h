#ifndef HEADER_H
#define HEADER_H

#define SAMPLES 512
#define SIN_AMP 15
#define OFF 16384

#define AMPLITUDE (3.3/67108864.0)

void precompute();

// waiting for input, called repeatedly by stream grabber wait
// delay: estimated wait duration, in number of samples left to be captured
void stream_wait(int delay);

float auto_range(void);

#endif
