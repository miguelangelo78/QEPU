#ifndef GATES_H_
#define GATES_H_
#define QB_SIZE1 1
#define QB_SIZE2 2
#define QB_SIZE3 3
#define	QB1_MATRIX_SIZE 2
#define	QB2_MATRIX_SIZE 4
#define QB3_MATRIX_SIZE 8
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
		int * CNO(int theta1, int phi1, int theta2,int phi2);
		int * CSI(int theta1, int phi1, int theta2,int phi2);
		int * SWA(int theta1, int phi1, int theta2,int phi2);
		int * INC(int theta1, int phi1, int theta2,int phi2);
		int * DEC(int theta1, int phi1, int theta2,int phi2);
		int * SWQ(int theta1, int phi1, int theta2,int phi2);
		int * SWI(int theta1, int phi1, int theta2,int phi2);
		int * ROX(int theta,int phi,int delta);
		int * ROY(int theta,int phi,int delta);
		int * ROZ(int theta,int phi,int delta);
		int * CSW(int theta1, int phi1,int theta2,int phi2,int theta3,int phi3);
		int * TOF(int theta1, int phi1,int theta2,int phi2,int theta3,int phi3);
		int * DEU(int theta1, int phi1,int theta2,int phi2,int theta3,int phi3,int delta);
	private:
		Complex* multiply2x2(Complex *q,Complex matrix[2][2]);
		Complex* multiply4x4(Complex *q,Complex matrix[4][4]);
		Complex* multiply8x8(Complex *q,Complex matrix[8][8]);
		Complex * ampl2vec(int qb_count,int theta_list[6],int phi_list[6]);
		int * vec2ampl(Complex * vec,int qb_count);
		Complex * kronecker(Complex * vec,int qb_count,int touch_enable);
		Complex * reverse_kronecker(Complex * kron,int kron_size);
		int touch(double probability);
		void setup_seed();
		int custom_pow(int base,int exp);
};
#endif