#include "fft.h"
#include "trig.h"

static float cos[SAMPLES];
static float sin[SAMPLES];

void precompute(){
	int n = SAMPLES;
	for(int i=0;i<n;i++){
		cos[i] = cosine(-PI*i/n);
		sin[i] = sine(-PI*i/n);
	}
}

static float new_[SAMPLES];
static float new_im[SAMPLES];

float fft(float* q, float* w, float sample_f) {
	int n = SAMPLES, m = M, angle;
	int a,b,r,d,e,c;
	int k,place;
	a=n/2;
	b=1;
	int i,j;
	float real=0,imagine=0;
	float max,frequency;

	// ORdering algorithm
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

	b=1;
	a=n;
	k=0;
	for (j=0; j<m; j++){	
	//MATH
		for(i=0; i<n; i+=2){
			if ((i&(a-1))==0 && i!=0)
				k++;
			angle = k << (m-j);
			real = q[i+1] * cos[angle] - w[i+1] * sin[angle];
			imagine = q[i+1] * sin[angle] + w[i+1] * cos[angle];
			new_[i]=q[i]+real;
			new_im[i]=w[i]+imagine;
			new_[i+1]=q[i]-real;
			new_im[i+1]=w[i]-imagine;

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
		b<<=1;
		a>>=1;
		k=0;		
	}

	//find magnitudes
	max=0;
	place=1;
	for(i=1;i<(n/2);i++) { 
		new_[i]=q[i]*q[i]+w[i]*w[i];
		if(max < new_[i]) {
			max=new_[i];
			place=i;
		}
	}
	
	float s=sample_f/n; //spacing of bins
	
	frequency = (sample_f/n)*place;

	//curve fitting for more accuarcy
	//assumes parabolic shape and uses three point to find the shift in the parabola
	//using the equation y=A(x-x0)^2+C
	float y1=new_[place-1],y2=new_[place],y3=new_[place+1];
	float x0=s+(2*s*(y2-y1))/(2*y2-y1-y3);
	x0=x0/s-1;
	
	if(x0 <0 || x0 > 2) { //error
		return 0;
	}
	if(x0 <= 1)  {
		frequency=frequency-(1-x0)*s;
	}
	else {
		frequency=frequency+(x0-1)*s;
	}
	
	return frequency;
}
