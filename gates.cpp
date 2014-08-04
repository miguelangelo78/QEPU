#include "gates.h"
#include "uart.h"
#include "qepu.h"
#include <stdarg.h>
#include <stdlib.h>
Gates::Gates(){setup_seed();}

Serial s;
QEPU::Utils u;

void Gates::setup_seed(){
	unsigned char oldADMUX = ADMUX;
	ADMUX |=  _BV(MUX0); //choose ADC1 on PB2
	ADCSRA |= _BV(ADPS2) |_BV(ADPS1) |_BV(ADPS0); //set prescaler to max value, 128

	ADCSRA |= _BV(ADEN); //enable the ADC
	ADCSRA |= _BV(ADSC);//start conversion

	while (ADCSRA & _BV(ADSC)); //wait until the hardware clears the flag. Note semicolon!

	unsigned char byte1 = ADCL;

	ADCSRA |= _BV(ADSC);//start conversion

	while (ADCSRA & _BV(ADSC)); //wait again note semicolon!

	unsigned char byte2 = ADCL;

	unsigned int seed = byte1 << 8 | byte2;

	srand(seed);

	ADCSRA &= ~_BV(ADEN); //disable ADC

	ADMUX = oldADMUX;
}

int Gates::touch(double probability){
	return RAND_MAX*probability>=rand();
}

Complex * Gates::ampl2vec(int qb_count,int theta_list[6],int phi_list[6]){
	Complex* vec=(Complex*)malloc(sizeof(Complex)*(qb_count*2));
	int thephi_index=0;
	for(int i=0;i<qb_count*2;i+=2){
		vec[i].re=cos(theta_list[thephi_index]*M_PI/360);												// ALPHA RE (IM=0)
		vec[i+1].re=sin((theta_list[thephi_index]*M_PI)/360)*cos((phi_list[thephi_index]*M_PI)/180);	// BETA RE
		vec[i+1].im=sin((theta_list[thephi_index]*M_PI)/360)*sin((phi_list[thephi_index++]*M_PI)/180);  // BETA IM
	}
	/*vec[0].re=cos(theta*M_PI/360);						 // ALPHA RE (IM=0)
	vec[1].re=sin((theta*M_PI)/360)*cos((phi*M_PI)/180); // BETA RE
	vec[1].im=sin((theta*M_PI)/360)*sin((phi*M_PI)/180); // BETA IM*/
	
	s.writestrln("Before: ");
	for(int i=0;i<qb_count*2;i++){
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
	for(int i=0;i<4;i++){
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
	int theta_list[1]={theta}; int phi_list[1]={phi};
	return vec2ampl(multiply2x2(ampl2vec(1,theta_list,phi_list),x_matrix));
}
int * Gates::Y(int theta,int phi){
	Complex y_matrix[2][2]{{Complex(0,0),Complex(0,-1)},
						   {Complex(0,1),Complex(0,0)}};
	//return vec2ampl(multiply2x2(ampl2vec(2,theta,phi),y_matrix));
}
int * Gates::Z(int theta,int phi){
	Complex z_matrix[2][2]{{Complex(1,0),Complex(0,0)},
						   {Complex(0,0),Complex(-1,0)}};
	//return vec2ampl(multiply2x2(ampl2vec(2,theta,phi),z_matrix));
}
int * Gates::H(int theta,int phi){
	Complex h_matrix[2][2]{{Complex(1/sqrt(2),0),Complex(1/sqrt(2),0)},
						   {Complex(1/sqrt(2),0),Complex(-1/sqrt(2),0)}};
	//return vec2ampl(multiply2x2(ampl2vec(2,theta,phi),h_matrix));
}
int * Gates::S(int theta,int phi){
	Complex s_matrix[2][2]{{Complex(1,0),Complex(0,0)},
						   {Complex(0,0),Complex(0,1)}};
	//return vec2ampl(multiply2x2(ampl2vec(2,theta,phi),s_matrix));
}
int * Gates::T(int theta,int phi){
	Complex t_matrix[2][2]{{Complex(1,0),Complex(0,0)},
						   {Complex(0,0),Complex(1/sqrt(2),1/sqrt(2))}};
	//return vec2ampl(multiply2x2(ampl2vec(2,theta,phi),t_matrix));
}

int * Gates::CNO(int theta1, int phi1, int theta2,int phi2){
	Complex cno_matrix[4][4]{{Complex(1,0),Complex(0,0),Complex(0,0),Complex(0,0)},
						     {Complex(0,0),Complex(1,0),Complex(0,0),Complex(0,0)},
							 {Complex(0,0),Complex(0,0),Complex(0,0),Complex(1,0)},
							 {Complex(0,0),Complex(0,0),Complex(1,0),Complex(0,0)}};
	int theta_list[2]={theta1,theta2}; int phi_list[2]={phi1,phi2};						
	return vec2ampl(multiply4x4(ampl2vec(2,theta_list,phi_list),cno_matrix));
}
int * Gates::CSI(int theta1, int phi1, int theta2,int phi2){
	
}
int * Gates::SWA(int theta1, int phi1, int theta2,int phi2){
	
}
int * Gates::INC(int theta1, int phi1, int theta2,int phi2){
	
}
int * Gates::DEC(int theta1, int phi1, int theta2,int phi2){
	
}
int * Gates::SWQ(int theta1, int phi1, int theta2,int phi2){
	
}
int * Gates::SWI(int theta1, int phi1, int theta2,int phi2){
	
}
int * Gates::ROX(int theta,int phi,int delta){
	
}
int * Gates::ROY(int theta,int phi,int delta){
	
}
int * Gates::ROZ(int theta,int phi,int delta){
	
}