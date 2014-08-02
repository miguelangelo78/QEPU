#ifndef COMPLEX_H_
#define COMPLEX_H_
#include <math.h>
class Complex{
	public:
		Complex(float Re,float Im);
		Complex(void);
		Complex add(Complex c2);
		Complex mul(Complex c2);
		float arg();
		float re,im;
};
#endif