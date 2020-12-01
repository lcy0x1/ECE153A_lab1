#ifndef HEADER_H
#define HEADER_H

#define WINDOW_SIZE 32

#define CUTOFF 1250
#define SAMPLES 512
#define SIN_AMP 15
#define SQ_AMP 17
#define OFF_SIN (1<<(SIN_AMP-1))
#define OFF_SQ (1<<(SQ_AMP-1))

// waiting for input, called repeatedly by stream grabber wait
// delay: estimated wait duration, in number of samples left to be captured
void stream_wait(int delay);
void precompute();

float auto_range(void);
int find_note(float freq);
void add_window(float freq);
float get_mean();
float get_stdev();

char* note_char(int note);

void stream_grabber_start();
unsigned stream_grabber_samples_sampled_captures();
void stream_grabber_wait_enough_samples(unsigned required_samples);
int stream_grabber_read_sample(unsigned which_sample);

#endif
