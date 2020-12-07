#ifndef HEADER_H
#define HEADER_H

#define WINDOW_SIZE 8

#define CUTOFF 2500
#define SAMPLES 512
#define SIN_AMP 15
#define SQ_AMP 16

#define USE_FLOAT 1

// waiting for input, called repeatedly by stream grabber wait
// delay: estimated wait duration, in number of samples left to be captured
void stream_wait(int delay);
void precompute();

float auto_range(int octave);
void add_window(float freq);
float get_mean();
float get_stdev();

int find_note(int a4, float freq);
char* note_char(int note);

void stream_grabber_start();
unsigned stream_grabber_samples_sampled_captures();
void stream_grabber_wait_enough_samples(unsigned required_samples);
int stream_grabber_read_sample(unsigned which_sample);

#endif
