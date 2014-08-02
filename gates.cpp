#include "gates.h"
#include "uart.h"
#include "qepu.h"
Gates::Gates(){
	
}

Serial s;
QEPU::Utils u;

Complex * Gates::ampl2vec(int theta,int phi){
	Complex* vec=(Complex*)malloc(sizeof(Complex)*2);
	vec[0].re=cos(theta*M_PI/360); // ALPHA RE (IM=0)
	vec[1].re=sin((theta*M_PI)/360)*cos((phi*M_PI)/180); // BETA RE
	vec[1].im=sin((theta*M_PI)/360)*sin((phi*M_PI)/180); // BETA IM
	
	s.writestrln("Before: ");
	for(int i=0;i<2;i++){
		s.writestr("R: "); s.writestr(u.int2str(vec[i].re*1000));
		s.writestr(" . I: "); s.writestr(u.int2str(vec[i].im*1000));
		s.writestrln("");
	}
	return vec;
}
int * Gates::vec2ampl(Complex * vec){
	int* newthephi=(int*)malloc(sizeof(int)*2);
	newthephi[0]=(360*acos(vec[0].re))/M_PI;
	newthephi[1]=(180*vec[1].arg())/M_PI;
	
	s.writestrln(""); s.writestrln("After: ");
	for(int i=0;i<2;i++){
		s.writestr("R: "); s.writestr(u.int2str(vec[i].re*1000));
		s.writestr(" . I: "); s.writestr(u.int2str(vec[i].im*1000));
		s.writestrln("");
	}
	s.writestrln("");
	return newthephi;
}

Complex * Gates::multiply2x2(Complex *q,Complex matrix[2][2]){
	int qb_size=2;
	Complex * result=(Complex*)malloc(sizeof(Complex)*qb_size);
	for(int i=0;i<qb_size;i++){
		Complex tmp;
		for(int j=0;j<qb_size;j++) tmp=tmp.add(q[j].mul(matrix[j][i]));
		result[i].re=tmp.re;
		result[i].im=tmp.im;
	}
	return result;
}
Complex * Gates::multiply4x4(Complex *q,Complex matrix[4][4]){
	int qb_size=4;
	Complex * result=(Complex*)malloc(sizeof(Complex)*qb_size);
	for(int i=0;i<qb_size;i++){
		Complex tmp;
		for(int j=0;j<qb_size;j++) tmp=tmp.add(q[j].mul(matrix[j][i]));
		result[i].re=tmp.re;
		result[i].im=tmp.im;
	}
	return result;
}
Complex * Gates::multiply8x8(Complex *q,Complex matrix[8][8]){
	int qb_size=8;
	Complex * result=(Complex*)malloc(sizeof(Complex)*qb_size);
	for(int i=0;i<qb_size;i++){
		Complex tmp;
		for(int j=0;j<qb_size;j++) tmp=tmp.add(q[j].mul(matrix[j][i]));
		result[i].re=tmp.re;
		result[i].im=tmp.im;
	}
	return result;
}

int * Gates::X(int theta,int phi){
	Complex x_matrix[2][2]{{Complex(0,0),Complex(1,0)},
						   {Complex(1,0),Complex(0,0)}};
	return vec2ampl(multiply2x2(ampl2vec(theta,phi),x_matrix));
}
int * Gates::Y(int theta,int phi){
	Complex y_matrix[2][2]{{Complex(0,0),Complex(0,-1)},
						   {Complex(0,1),Complex(0,0)}};
	return vec2ampl(multiply2x2(ampl2vec(theta,phi),y_matrix));
}
int * Gates::Z(int theta,int phi){
	Complex z_matrix[2][2]{{Complex(1,0),Complex(0,0)},
						   {Complex(0,0),Complex(-1,0)}};
	return vec2ampl(multiply2x2(ampl2vec(theta,phi),z_matrix));
}
int * Gates::H(int theta,int phi){
	Complex h_matrix[2][2]{{Complex(1/sqrt(2),0),Complex(1/sqrt(2),0)},
						   {Complex(1/sqrt(2),0),Complex(-1/sqrt(2),0)}};
	return vec2ampl(multiply2x2(ampl2vec(theta,phi),h_matrix));
}
int * Gates::S(int theta,int phi){
	Complex s_matrix[2][2]{{Complex(1,0),Complex(0,0)},
						   {Complex(0,0),Complex(0,1)}};
	return vec2ampl(multiply2x2(ampl2vec(theta,phi),s_matrix));
}
int * Gates::T(int theta,int phi){
	Complex t_matrix[2][2]{{Complex(1,0),Complex(0,0)},
						   {Complex(0,0),Complex(1/sqrt(2),1/sqrt(2))}};
	return vec2ampl(multiply2x2(ampl2vec(theta,phi),t_matrix));
}

//MAY NOT IMPLEMENT
/* 
int * Gates::Rx(int theta,int phi,int delta){
	
}
int * Gates::Ry(int theta,int phi,int delta){
	
}
int * Gates::Rz(int theta,int phi,int delta){
	
}*/