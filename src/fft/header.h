#ifndef HEADER_H
#define HEADER_H

// if USE FLOAT is set, FFT will be in float mode, otherwise it will be in int mode.
// float mode is more sensitive to low volume and resistant to large volume.
#define USE_FLOAT 1
// if AUTO_RANGE_OVERRIDE is set, FFT will automatically choose the correct configuration
// and override the provided octave. This comes with a small 10ms overhead in Debug mode
#define AUTO_RANGE_OVERRIDE 1

// Window size for window averaging. smooth the data to be more displayable.
#define WINDOW_SIZE 8
// Max FFT size
#define SAMPLES 512
// The sampling rate of the timer
#define SLOW_RATE 4
// The array holding the timer samples
#define SLOW_SAMPLES 1024

// These 2 values are used by auto-ranging to find the correct configuration
#define CUTOFF 2500
#define SLOW_CUTOFF 600
// These 2 values are used by integer FFT to avoid overflow while maintaining sensitivity
#define SIN_AMP 15
#define SQ_AMP 16


// waiting for input, called repeatedly by stream grabber wait
// delay: estimated wait duration, in number of samples left to be captured
void stream_wait(int delay);
// precompute the sine and cosine array for FFT
void precompute();

// do FFT with provided octave selection.
// Choose appropriate configuration based on octave.
// if AUTO_RANGE_OVERRIDE is set, auto range will override octave selection
float auto_range(int octave);
// add the frequency into the averaging window
void add_window(float freq);
// get the mean of the current window
float get_mean();
// get the standard deviation of the current window
float get_stdev();
// convert frequency to note in cents
int find_note(int a4, float freq);
// get the notation for a note
char* note_char(int note);

// start the stram grabber from the beginning
void stream_grabber_start();
// get the number of samples already captured
unsigned stream_grabber_samples_sampled_captures();
// wait for enough samples. If needs to wait a long time, LCD display is updated
void stream_grabber_wait_enough_samples(unsigned required_samples);
// read a sample from stream grabber
int stream_grabber_read_sample(unsigned which_sample);
// log a sample from timer
void log_slow_mic_value(unsigned value);

#endif
