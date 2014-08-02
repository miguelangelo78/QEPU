#ifndef GATES_H_
#define GATES_H_
#define M_PI 3.14159265358979323846
#include <math.h>
#include <stdio.h>
#include "complex.h"
class Gates{
	public:
		Gates();
		int * X(int theta,int phi);
		int * Y(int theta,int phi);
		int * Z(int theta,int phi);
		int * H(int theta,int phi);
		int * S(int theta,int phi);
		int * T(int theta,int phi);
		int * Rx(int theta,int phi,int delta);
		int * Ry(int theta,int phi,int delta);
		int * Rz(int theta,int phi,int delta);
	private:
		Complex* multiply2x2(Complex *q,Complex matrix[2][2]);
		Complex* multiply4x4(Complex *q,Complex matrix[4][4]);
		Complex* multiply8x8(Complex *q,Complex matrix[8][8]);
		Complex * ampl2vec(int theta,int phi);
		int * vec2ampl(Complex * vec);
};
#endif