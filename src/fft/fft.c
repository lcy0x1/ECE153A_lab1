#include "trig.h"
#include "header.h"
#include "xintc.h"
#include "../lab3b/hardware.h"

#if USE_FLOAT
#define CAST_SIN(v) v
#define CAST_MUL(v) v
#define CAST_AMP(v) v
typedef float fft_t;
#else
#define OFF_SIN (1<<(SIN_AMP-1))
#define OFF_SQ (1<<(SQ_AMP-1))
#define CAST_SIN(v) (int)(v * (1 << SIN_AMP) + 0.5)
#define CAST_MUL(v) (v + OFF_SIN) >> SIN_AMP
#define CAST_AMP(v) (v + OFF_SQ) >> SQ_AMP
typedef int fft_t;
#endif

typedef struct SLOW_TAG {
	unsigned int arr[SAMPLES];
	volatile int index;
	volatile int count;
} SLOW;

static SLOW buffer[SLOW_SAMPLES];

static unsigned int slow_index = 0;

static fft_t cos[SAMPLES];
static fft_t sin[SAMPLES];

static fft_t new_[SAMPLES];
static fft_t new_im[SAMPLES];
static fft_t q[SAMPLES];
static fft_t w[SAMPLES];

float stream_freq = 100000000 / 2048.0;

void precompute(){
	int n = SAMPLES;
	for(int i=0;i<n;i++){
		cos[i] = CAST_SIN(cosine(-PI*i/n));
		sin[i] = CAST_SIN(sine(-PI*i/n));
	}
}

float fft(int m, float sample_f) {
	int n = 1 << m;
	int a,b,r,d,e,c,i,j;

	// Ordering algorithm
	a=n/2;
	b=1;
	for(i=0; i<m-1; i++){
		d=0;
		for (j=0; j<b; j++){
			for (c=0; c<a; c++){	
				e=c+d;
				r = c*2+d;
				new_[e]=q[r];
				new_im[e]=w[r];
				new_[e+a]=q[r+1];
				new_im[e+a]=w[r+1];
			}
			d+=a<<1;
		}		
		for (r=0; r<n; r++){
			q[r]=new_[r];
			w[r]=new_im[r];
		}
		b<<=1;
		a>>=1;
	}
	//end ordering algorithm

	fft_t re,im,dq,dw;
	int angle;

	for (j=0; j<m; j++){	
	//MATH
		a = ~((n>>j)-1);
		for(i=0; i<n; i+=2){
			angle = (i&a) << (9-m);
			dq = CAST_MUL(q[i+1]);
			dw = CAST_MUL(w[i+1]);
			re = dq * cos[angle] - dw * sin[angle];
			im = dq * sin[angle] + dw * cos[angle];

			new_[i]=q[i]+re;
			new_im[i]=w[i]+im;
			new_[i+1]=q[i]-re;
			new_im[i+1]=w[i]-im;

		}
		for (i=0; i<n; i++){
			q[i]=new_[i];
			w[i]=new_im[i];
		}
	//END MATH

	//REORDER
		for (i=0; i<n/2; i++){
			new_[i]=q[2*i];
			new_[i+(n/2)]=q[2*i+1];
			new_im[i]=w[2*i];
			new_im[i+(n/2)]=w[2*i+1];
		}
		for (i=0; i<n; i++){
			q[i]=new_[i];
			w[i]=new_im[i];
		}
	//END REORDER
	}

	int place;
	fft_t fq, fw, max=0;
	place=1;
	for(i=1; i<(n/2); i++) {
		fq = CAST_AMP(q[i]);
		fw = CAST_AMP(w[i]);
		new_[i] = fq * fq + fw * fw;
		if(max < new_[i]) {
			max = new_[i];
			place = i;
		}
	}
	float s=sample_f/n;
	fft_t y1=new_[place-1],y2=new_[place],y3=new_[place+1];
	float x0=s+(2*s*(y2-y1))/(2*y2-y1-y3);
	x0=x0/s-1;
	
	if(x0 <0 || x0 > 2) { //error
		return 0;
	}
	return place*s-(1-x0)*s;
}

// store, average, and distribute the timer samples into all arrays
void log_slow_mic_value(unsigned int value){
	slow_index++;
	for(int i=0;i<SLOW_SAMPLES;i++){
		SLOW * s = &buffer[i];
		s->arr[s->index] = value;
		// average for the next array
		value = (value >> 1) + (s->arr[(s->index + SAMPLES - 1) & (SAMPLES - 1)] >> 1);
		s->index = (s->index + 1) & (SAMPLES - 1);
		if(s->count < SAMPLES)
			s->count ++;
		if(slow_index & ((2<<i)-1))
			return;
	}
}

// FFT using samples from timer
float slow_fft(int m, int sk){
	int n = 1 << m;
	// start sampler in case frequency changed later
	stream_grabber_start();
	SLOW* s = &buffer[sk];
	if(s->count < SAMPLES)
		return 0;
	// disable interrupt to prevent harzard
	microblaze_disable_interrupts();
	for(int i=0;i<n;i++){
		q[i] = s->arr[(s->index+i)&(SAMPLES-1)];
		w[i] = 0;
	}
	microblaze_enable_interrupts();
	set_width(sk == SLOW_SAMPLES - 1 ? WINDOW_SIZE : SLOW_WINDOW_SIZE);
	return fft(m, stream_freq / (1 << (sk + SLOW_RATE)));

}

// FFT using samples from stream grabber
float one_fft(int m, int sk, int restart){
	int n = 1 << m;
	int skip = 1 << sk;
	int round = sk == 0 ? 0 :(1 << (sk-1));
	stream_grabber_wait_enough_samples( 1 << (m+sk));
	int sum = 0;
	for(int i=0;i<n;i++){
		sum = 0;
		for(int j=0;j<skip;j++)
			sum += (stream_grabber_read_sample((i<<sk) | j) + round) >> sk;
		q[i] = sum;
		w[i] = 0;
	}
	// restart only if it is not the testing FFT
	if(restart)
		stream_grabber_start();
	set_width(FAST_WINDOW_SIZE);
	return fft(m, stream_freq / skip);
}

float auto_range(int octave){

#if !AUTO_RANGE_OVERRIDE
	// octave - configuration map
	if(octave <= 2)
		return slow_fft(9, 3);
	if(octave == 3)
		return slow_fft(9, 2);
	if(octave == 4)
		return slow_fft(9, 1);
	if(octave == 5)
		return slow_fft(9, 0);
	if(octave < 7)
		return one_fft(9, 2, 1);
	if(octave == 8)
		return one_fft(9, 1, 1);
	if(octave == 9)
		return one_fft(9, 0, 1);
#endif

	// 128-point test FFT to get a sense of the frequency range
	float freq = one_fft(7, 0, 0);
	if(freq > CUTOFF << 6)
		return one_fft(9, 0, 1);
	if(freq > CUTOFF << 5)
		return one_fft(9, 1, 1);
	if(freq > CUTOFF << 3)
		return one_fft(9, 2, 1);

	// 128-point test FFT to get a sense of the frequency range
	freq = one_fft(7, 3, 0);
	if(freq > CUTOFF << 2)
		return slow_fft(9, 0);
	if(freq > CUTOFF << 1)
		return slow_fft(9, 1);
	if(freq > CUTOFF)
		return slow_fft(9, 2);
	return slow_fft(9, 3);
}
