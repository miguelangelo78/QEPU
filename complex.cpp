#include "complex.h"
Complex::Complex(float Re,float Im):re(Re),im(Im){}
Complex::Complex(void):re(0.0f),im(0.0f){}
	
Complex Complex::add(Complex c2){
	return Complex(this->re+c2.re,this->im+c2.im);
}
Complex Complex::mul(Complex c2){
	return Complex((this->re*c2.re)-(this->im*c2.im),(this->re*c2.im)+(this->im*c2.re));
}
float Complex::arg(){
	return atan(this->im/this->re);
}