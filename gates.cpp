#include "gates.h"
#include "uart.h"
#include "qepu.h"
#include <stdarg.h>
#include <stdlib.h>
#include <avr/eeprom.h>
Gates::Gates(){setup_seed();}

Serial s;
QEPU::Utils utils;
uint16_t EEMEM randinit;

void print_states(int qb_count,Complex * vec,char* message){
	s.writestrln(message);
	for(int i=0;i<qb_count;i++){
		s.writestr("R: "); s.writestr(utils.int2str(vec[i].re*1000));
		s.writestr(" . I: "); s.writestr(utils.int2str(vec[i].im*1000));
		s.writestrln("");
	}
}

void Gates::setup_seed(){
	srand(eeprom_read_word(&randinit));
	eeprom_write_word(&randinit,rand()%10000);
}

int Gates::touch(double probability){
	return RAND_MAX*probability>=rand();
}

int Gates::custom_pow(int base,int exp){
	int result=1; for(int i=0;i<exp;i++) result*=base;
	return result;
}

Complex * Gates::reverse_kronecker(Complex * kron,int kron_size){
	int reversed_kron_size=(log(kron_size)/log(2))*2;
	Complex * reversed_kronecker=(Complex*)malloc(sizeof(Complex)*reversed_kron_size);
		
	//TEMPORARY
	reversed_kronecker=kron;
	for(int i=0;i<kron_size;i++)
		if(kron[i].re==1){
			char *toBin=utils.int2binstr(i,reversed_kron_size);
			int index_rev_kro=0; //USE ITS OWN INDEX BECAUSE J IS THE INDEX OF TOBIN AN TOBIN HAS TO GO REVERSED
			for(int j=reversed_kron_size-1;j>=0;j--){
				if(toBin[j]=='1'){
					reversed_kronecker[index_rev_kro]=Complex(0,0); reversed_kronecker[index_rev_kro+1]=Complex(1,0);
				}else{
					reversed_kronecker[index_rev_kro]=Complex(1,0); reversed_kronecker[index_rev_kro+1]=Complex(0,0);
				}
				index_rev_kro+=2;
			}
			break;
		}
	return reversed_kronecker;
}

Complex * Gates::kronecker(Complex * vec,int qb_count,int touch_enable){
	if(qb_count==1) return vec;
	int kron_size=custom_pow(2,qb_count);
	Complex * kronvec=(Complex*)malloc(sizeof(Complex)*kron_size);
	print_states(2*2,vec,"Before: ");
	
	//KRONECKER ALGORITHM: 
	/*
	// Vec[0]=Alpha1 Vec[1]=Beta1
	// Vec[2]=Alpha2 Vec[3]=Beta2
	// kronvec[0]=vec[0]*vec[2] kronvec[1]=vec[0]*vec[3]
	// kronvec[2]=vec[1]*vec[2] kronvec[3]=vec[1]*vec[3]
	*/
	
	int vec1i=0;
	int vec2i_default=qb_count;
	int vec2i=vec2i_default;
	for(int i=0;i<kron_size;i++){
    	if(vec2i==qb_count*2){vec2i=vec2i_default;vec1i++;}
		kronvec[i]=vec[vec1i].mul(vec[vec2i++]);
	}
	
	if(touch_enable){
		//TOUCH THE ENTANGLED/SUPERPOSITIONED QUBIT BEFORE GIVING IT TO A MATRIX:
		int binary_touch=1;
		while(binary_touch)	for(int i=0;i<kron_size;i++)
			if(touch(kronvec[i].re)==binary_touch){
				for(int j=0;j<kron_size;j++){kronvec[j].re=0; kronvec[j].im=0;}
				kronvec[i].re=binary_touch;
				binary_touch=0;
				break;
			}
	}
	return kronvec;
}

Complex * Gates::ampl2vec(int qb_count,int theta_list[6],int phi_list[6]){
	Complex* vec=(Complex*)malloc(sizeof(Complex)*(qb_count*2));
	int thephi_index=0;
	for(int i=0;i<qb_count*2;i+=2){
		vec[i].re=cos(theta_list[thephi_index]*M_PI/360);												// ALPHA RE (IM=0)
		vec[i+1].re=sin((theta_list[thephi_index]*M_PI)/360)*cos((phi_list[thephi_index]*M_PI)/180);	// BETA RE
		vec[i+1].im=sin((theta_list[thephi_index]*M_PI)/360)*sin((phi_list[thephi_index++]*M_PI)/180);  // BETA IM
	}
	return kronecker(vec,qb_count,true); // PUT VEC INTO KRONECKER AND RETURN THE RESULT
}
int * Gates::vec2ampl(Complex * vec,int qb_count){
	int kron_size=custom_pow(2,qb_count);
	if(qb_count>1) vec=reverse_kronecker(vec,kron_size);
	
	int* newthephi=(int*)malloc(sizeof(int)*2);
	newthephi[0]=(360*acos(vec[0].re))/M_PI;
	newthephi[1]=(180*vec[1].arg())/M_PI;
	
	print_states(kron_size,vec,"After: ");
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
	return vec2ampl(multiply2x2(ampl2vec(1,theta_list,phi_list),x_matrix),1);
}
int * Gates::Y(int theta,int phi){
	Complex y_matrix[2][2]{{Complex(0,0),Complex(0,-1)},
						   {Complex(0,1),Complex(0,0)}};
	int theta_list[1]={theta}; int phi_list[1]={phi};
	return vec2ampl(multiply2x2(ampl2vec(1,theta_list,phi_list),y_matrix),1);
}
int * Gates::Z(int theta,int phi){
	Complex z_matrix[2][2]{{Complex(1,0),Complex(0,0)},
						   {Complex(0,0),Complex(-1,0)}};
	int theta_list[1]={theta}; int phi_list[1]={phi};
	return vec2ampl(multiply2x2(ampl2vec(1,theta_list,phi_list),z_matrix),1);
}
int * Gates::H(int theta,int phi){
	Complex h_matrix[2][2]{{Complex(1/sqrt(2),0),Complex(1/sqrt(2),0)},
						   {Complex(1/sqrt(2),0),Complex(-1/sqrt(2),0)}};
	int theta_list[1]={theta}; int phi_list[1]={phi};
	return vec2ampl(multiply2x2(ampl2vec(1,theta_list,phi_list),h_matrix),1);
}
int * Gates::S(int theta,int phi){
	Complex s_matrix[2][2]{{Complex(1,0),Complex(0,0)},
						   {Complex(0,0),Complex(0,1)}};
	int theta_list[1]={theta}; int phi_list[1]={phi};
	return vec2ampl(multiply2x2(ampl2vec(1,theta_list,phi_list),s_matrix),1);
}
int * Gates::T(int theta,int phi){
	Complex t_matrix[2][2]{{Complex(1,0),Complex(0,0)},
						   {Complex(0,0),Complex(1/sqrt(2),1/sqrt(2))}};
	int theta_list[1]={theta}; int phi_list[1]={phi};
	return vec2ampl(multiply2x2(ampl2vec(1,theta_list,phi_list),t_matrix),1);
}

int * Gates::CNO(int theta1, int phi1, int theta2,int phi2){
	Complex cno_matrix[4][4]{{Complex(1,0),Complex(0,0),Complex(0,0),Complex(0,0)},
						     {Complex(0,0),Complex(1,0),Complex(0,0),Complex(0,0)},
							 {Complex(0,0),Complex(0,0),Complex(0,0),Complex(1,0)},
							 {Complex(0,0),Complex(0,0),Complex(1,0),Complex(0,0)}};
	int theta_list[2]={theta1,theta2}; int phi_list[2]={phi1,phi2};						
	return vec2ampl(multiply4x4(ampl2vec(2,theta_list,phi_list),cno_matrix),2);
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
