#include "trig.h"
#include "header.h"

static int cos[SAMPLES];
static int sin[SAMPLES];

void precompute(){
	int n = SAMPLES;
	int amp = 1 << SIN_AMP;
	for(int i=0;i<n;i++){
		cos[i] = (int)(cosine(-PI*i/n) * amp + 0.5);
		sin[i] = (int)(sine(-PI*i/n) * amp + 0.5);
	}
}

static int new_[SAMPLES];
static int new_im[SAMPLES];
static int q[SAMPLES];
static int w[SAMPLES];

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

	int re,im,dq,dw,angle;

	for (j=0; j<m; j++){	
	//MATH
		a = ~((n>>j)-1);
		for(i=0; i<n; i+=2){
			angle = (i&a) << (9-m);
			dq = (q[i+1]+OFF_SIN) >> SIN_AMP;
			dw = (w[i+1]+OFF_SIN) >> SIN_AMP;
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

	int place,max, fq, fw;
	max=0;
	place=1;
	for(i=1; i<(n/2); i++) {
		fq = (q[i] + OFF_SQ) >> SQ_AMP;
		fw = (w[i] + OFF_SQ) >> SQ_AMP;
		new_[i] = fq * fq + fw * fw;
		if(max < new_[i]) {
			max = new_[i];
			place = i;
		}
	}
	float s=sample_f/n;
	int y1=new_[place-1],y2=new_[place],y3=new_[place+1];
	float x0=s+(2*s*(y2-y1))/(2*y2-y1-y3);
	x0=x0/s-1;
	
	if(x0 <0 || x0 > 2) { //error
		return 0;
	}
	return place*s-(1-x0)*s;
}

float stream_freq = 100000000 / 2048.0;

float one_fft(int m, int sk, int restart){
	int n = 1 << m;
	int skip = 1 << sk;
	int round = 1 << (sk-1);
	stream_grabber_wait_enough_samples( 1 << (m+sk));
	int sum = 0;
	for(int i=0;i<n;i++){
		sum = 0;
		for(int j=0;j<skip;j++)
			sum += (stream_grabber_read_sample((i<<sk) | j) + round) >> sk;
		q[i] = sum;
		w[i] = 0;
	}
	if(restart)
		stream_grabber_start();
	return fft(m, stream_freq / skip);
}

float auto_range(void){
	float freq = one_fft(7, 0, 0);
	if(freq > CUTOFF << 3)
		return one_fft(9, 0, 1);
	if(freq > CUTOFF << 2)
		return one_fft(9, 1, 1);
	freq = one_fft(7, 2, 0);
	if(freq > CUTOFF << 1)
		return one_fft(8, 2, 1);
	if(freq > CUTOFF)
		return one_fft(7, 3, 1);
	return one_fft(7, 4, 1);
}
