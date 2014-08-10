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
		
	for(int i=0;i<kron_size;i++)
		if(kron[i].re==1){
			char *toBin=utils.int2binstr(i,log(kron_size)/log(2));
			int index_rev_kro=0; //USE ITS OWN INDEX BECAUSE J IS THE INDEX OF TOBIN AN TOBIN HAS TO GO REVERSED
			for(int j=0;j<reversed_kron_size;j++){
				if(toBin[j]=='1'){
					reversed_kronecker[index_rev_kro]=Complex(0,0); reversed_kronecker[index_rev_kro+1]=Complex(1,0);
				}else{
					reversed_kronecker[index_rev_kro]=Complex(1,0); reversed_kronecker[index_rev_kro+1]=Complex(0,0);
				}
				index_rev_kro+=2;
			}
			break;
		}
	//print_states(reversed_kron_size,reversed_kronecker,"Reversed kronecker AFTER mul: ");
	return reversed_kronecker;
}

Complex * Gates::kronecker(Complex * vec,int qb_count,int touch_enable){
	if(qb_count==1) return vec;
	int kron_size=custom_pow(2,qb_count);
	Complex * kronvec=(Complex*)malloc(sizeof(Complex)*kron_size);
	
	int vec1i=0;
	int vec2i_default=VECTOR_QUBIT_SIZE;
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
	//print_states(kron_size,kronvec,"Kronecker: ");
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
	print_states(qb_count*2,vec,"Before: ");
	return kronecker(vec,qb_count,true); // PUT VEC INTO KRONECKER AND RETURN THE RESULT
}
int * Gates::vec2ampl(Complex * vec,int qb_count){
	int kron_size=custom_pow(2,qb_count);
	if(qb_count>1) vec=reverse_kronecker(vec,kron_size);
	
	int* newthephi=(int*)malloc(sizeof(int)*(qb_count*2));
	for(int i=0;i<qb_count*2;i++) if(i%2==0) newthephi[i]=(360*acos(vec[i].re))/M_PI; else newthephi[i]=(180*vec[i].arg())/M_PI;
	
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
	Complex x_matrix[QB1_MATRIX_SIZE][QB1_MATRIX_SIZE]{{Complex(0,0),Complex(1,0)},
													   {Complex(1,0),Complex(0,0)}};
	int theta_list[QB_SIZE1]={theta}; int phi_list[1]={phi};
	return vec2ampl(multiply2x2(ampl2vec(QB_SIZE1,theta_list,phi_list),x_matrix),QB_SIZE1);
}
int * Gates::Y(int theta,int phi){
	Complex y_matrix[QB1_MATRIX_SIZE][QB1_MATRIX_SIZE]{{Complex(0,0),Complex(0,-1)},
													   {Complex(0,1),Complex(0,0)}};
	int theta_list[QB_SIZE1]={theta}; int phi_list[QB_SIZE1]={phi};
	return vec2ampl(multiply2x2(ampl2vec(QB_SIZE1,theta_list,phi_list),y_matrix),QB_SIZE1);
}
int * Gates::Z(int theta,int phi){
	Complex z_matrix[QB1_MATRIX_SIZE][QB1_MATRIX_SIZE]{{Complex(1,0),Complex(0,0)},
													   {Complex(0,0),Complex(-1,0)}};
	int theta_list[QB_SIZE1]={theta}; int phi_list[QB_SIZE1]={phi};
	return vec2ampl(multiply2x2(ampl2vec(QB_SIZE1,theta_list,phi_list),z_matrix),QB_SIZE1);
}
int * Gates::H(int theta,int phi){
	Complex h_matrix[QB1_MATRIX_SIZE][QB1_MATRIX_SIZE]{{Complex(1/sqrt(2),0),Complex(1/sqrt(2),0)},
													   {Complex(1/sqrt(2),0),Complex(-1/sqrt(2),0)}};
	int theta_list[QB_SIZE1]={theta}; int phi_list[QB_SIZE1]={phi};
	return vec2ampl(multiply2x2(ampl2vec(QB_SIZE1,theta_list,phi_list),h_matrix),QB_SIZE1);
}
int * Gates::S(int theta,int phi){
	Complex s_matrix[QB1_MATRIX_SIZE][QB1_MATRIX_SIZE]{{Complex(1,0),Complex(0,0)},
													   {Complex(0,0),Complex(0,1)}};
	int theta_list[QB_SIZE1]={theta}; int phi_list[QB_SIZE1]={phi};
	return vec2ampl(multiply2x2(ampl2vec(QB_SIZE1,theta_list,phi_list),s_matrix),QB_SIZE1);
}
int * Gates::T(int theta,int phi){
	Complex t_matrix[QB1_MATRIX_SIZE][QB1_MATRIX_SIZE]{{Complex(1,0),Complex(0,0)},
													   {Complex(0,0),Complex(1/sqrt(2),1/sqrt(2))}};
	int theta_list[QB_SIZE1]={theta}; int phi_list[QB_SIZE1]={phi};
	return vec2ampl(multiply2x2(ampl2vec(QB_SIZE1,theta_list,phi_list),t_matrix),QB_SIZE1);
}

int * Gates::CNO(int theta1, int phi1, int theta2,int phi2){
	Complex cno_matrix[QB2_MATRIX_SIZE][QB2_MATRIX_SIZE]{{Complex(1,0),Complex(0,0),Complex(0,0),Complex(0,0)},
													     {Complex(0,0),Complex(1,0),Complex(0,0),Complex(0,0)},
													     {Complex(0,0),Complex(0,0),Complex(0,0),Complex(1,0)},
													     {Complex(0,0),Complex(0,0),Complex(1,0),Complex(0,0)}};
	int theta_list[QB_SIZE2]={theta1,theta2}; int phi_list[QB_SIZE2]={phi1,phi2};						
	return vec2ampl(multiply4x4(ampl2vec(QB_SIZE2,theta_list,phi_list),cno_matrix),QB_SIZE2);
}
int * Gates::CSI(int theta1, int phi1, int theta2,int phi2){
	Complex csi_matrix[QB2_MATRIX_SIZE][QB2_MATRIX_SIZE]{{Complex(1,0),Complex(0,0),Complex(0,0),Complex(0,0)},
													     {Complex(0,0),Complex(1,0),Complex(0,0),Complex(0,0)},
													     {Complex(0,0),Complex(0,0),Complex(1,0),Complex(0,0)},
													     {Complex(0,0),Complex(0,0),Complex(0,0),Complex(-1,0)}};
	int theta_list[QB_SIZE2]={theta1,theta2}; int phi_list[QB_SIZE2]={phi1,phi2};
	return vec2ampl(multiply4x4(ampl2vec(QB_SIZE2,theta_list,phi_list),csi_matrix),QB_SIZE2);
}
int * Gates::SWA(int theta1, int phi1, int theta2,int phi2){
	Complex swa_matrix[QB2_MATRIX_SIZE][QB2_MATRIX_SIZE]{{Complex(1,0),Complex(0,0),Complex(0,0),Complex(0,0)},
													     {Complex(0,0),Complex(0,0),Complex(1,0),Complex(0,0)},
													     {Complex(0,0),Complex(1,0),Complex(0,0),Complex(0,0)},
													     {Complex(0,0),Complex(0,0),Complex(0,0),Complex(1,0)}};
	int theta_list[QB_SIZE2]={theta1,theta2}; int phi_list[QB_SIZE2]={phi1,phi2};
	return vec2ampl(multiply4x4(ampl2vec(QB_SIZE2,theta_list,phi_list),swa_matrix),QB_SIZE2);	
}
int * Gates::INC(int theta1, int phi1, int theta2,int phi2){
	Complex inc_matrix[QB2_MATRIX_SIZE][QB2_MATRIX_SIZE]{{Complex(0,0),Complex(1,0),Complex(0,0),Complex(0,0)},
													     {Complex(0,0),Complex(0,0),Complex(1,0),Complex(0,0)},
													     {Complex(0,0),Complex(0,0),Complex(0,0),Complex(1,0)},
													     {Complex(1,0),Complex(0,0),Complex(0,0),Complex(0,0)}};
	int theta_list[QB_SIZE2]={theta1,theta2}; int phi_list[QB_SIZE2]={phi1,phi2};
	return vec2ampl(multiply4x4(ampl2vec(QB_SIZE2,theta_list,phi_list),inc_matrix),QB_SIZE2);
}
int * Gates::DEC(int theta1, int phi1, int theta2,int phi2){
	Complex dec_matrix[QB2_MATRIX_SIZE][QB2_MATRIX_SIZE]{{Complex(0,0),Complex(0,0),Complex(0,0),Complex(1,0)},
													     {Complex(1,0),Complex(0,0),Complex(0,0),Complex(0,0)},
													     {Complex(0,0),Complex(1,0),Complex(0,0),Complex(0,0)},
													     {Complex(0,0),Complex(0,0),Complex(1,0),Complex(0,0)}};
	int theta_list[QB_SIZE2]={theta1,theta2}; int phi_list[QB_SIZE2]={phi1,phi2};
	return vec2ampl(multiply4x4(ampl2vec(QB_SIZE2,theta_list,phi_list),dec_matrix),QB_SIZE2);
}
int * Gates::SWQ(int theta1, int phi1, int theta2,int phi2){
	Complex swq_matrix[QB2_MATRIX_SIZE][QB2_MATRIX_SIZE]{{Complex(1,0),Complex(0,0),   Complex(0,0),   Complex(0,0)},
	 												     {Complex(0,0),Complex(.5,.5), Complex(.5,-.5),Complex(0,0)},
													     {Complex(0,0),Complex(.5,-.5),Complex(.5,.5), Complex(0,0)},
													     {Complex(0,0),Complex(0,0),   Complex(0,0),   Complex(1,0)}};
	int theta_list[QB_SIZE2]={theta1,theta2}; int phi_list[QB_SIZE2]={phi1,phi2};
	return vec2ampl(multiply4x4(ampl2vec(QB_SIZE2,theta_list,phi_list),swq_matrix),QB_SIZE2);
}
int * Gates::SWI(int theta1, int phi1, int theta2,int phi2){
	Complex dec_matrix[QB2_MATRIX_SIZE][QB2_MATRIX_SIZE]{{Complex(1,0),Complex(0,0),Complex(0,0),Complex(0,0)},
													     {Complex(0,0),Complex(0,0),Complex(0,1),Complex(0,0)},
 													     {Complex(0,0),Complex(0,1),Complex(0,0),Complex(0,0)},
													     {Complex(0,0),Complex(0,0),Complex(0,0),Complex(1,0)}};
	int theta_list[QB_SIZE2]={theta1,theta2}; int phi_list[QB_SIZE2]={phi1,phi2};
	return vec2ampl(multiply4x4(ampl2vec(QB_SIZE2,theta_list,phi_list),dec_matrix),QB_SIZE2);
}
int * Gates::ROX(int theta,int phi,int delta){
	float delta_float=(delta*M_PI)/180;
	Complex rox_matrix[QB1_MATRIX_SIZE][QB1_MATRIX_SIZE]{{Complex(cos(delta_float/2),0), Complex(0,-sin(delta_float/2))},
														 {Complex(0,-sin(delta_float/2)),Complex(cos(delta_float/2),0)}};
	int theta_list[QB_SIZE1]={theta}; int phi_list[QB_SIZE1]={phi};
	return vec2ampl(multiply2x2(ampl2vec(QB_SIZE1,theta_list,phi_list),rox_matrix),QB_SIZE1);		
}
int * Gates::ROY(int theta,int phi,int delta){
	float delta_float=(delta*M_PI)/180;
	Complex roy_matrix[QB1_MATRIX_SIZE][QB1_MATRIX_SIZE]{{Complex(cos(delta_float/2),0),Complex(-sin(delta_float/2),0)},
														 {Complex(sin(delta_float/2),0),Complex(cos(delta_float/2),0)}};
	int theta_list[QB_SIZE1]={theta}; int phi_list[QB_SIZE1]={phi};
	return vec2ampl(multiply2x2(ampl2vec(QB_SIZE1,theta_list,phi_list),roy_matrix),QB_SIZE1);
}
int * Gates::ROZ(int theta,int phi,int delta){
	float delta_float=(delta*M_PI)/180;
	Complex roy_matrix[QB1_MATRIX_SIZE][QB1_MATRIX_SIZE]{{Complex(0,exp(-delta_float/2)),Complex(0,0)},
														 {Complex(0,0),		  		     Complex(0,exp(delta_float/2))}};
	int theta_list[QB_SIZE1]={theta}; int phi_list[QB_SIZE1]={phi};
	return vec2ampl(multiply2x2(ampl2vec(QB_SIZE1,theta_list,phi_list),roy_matrix),QB_SIZE1);
}
int * Gates::CSW(int theta1, int phi1,int theta2,int phi2,int theta3,int phi3){
	Complex csw_matrix[QB3_MATRIX_SIZE][QB3_MATRIX_SIZE]={{Complex(1,0),Complex(0,0),Complex(0,0),Complex(0,0),Complex(0,0),Complex(0,0),Complex(0,0),Complex(0,0)},
														  {Complex(0,0),Complex(1,0),Complex(0,0),Complex(0,0),Complex(0,0),Complex(0,0),Complex(0,0),Complex(0,0)},
														  {Complex(0,0),Complex(0,0),Complex(1,0),Complex(0,0),Complex(0,0),Complex(0,0),Complex(0,0),Complex(0,0)},
														  {Complex(0,0),Complex(0,0),Complex(0,0),Complex(1,0),Complex(0,0),Complex(0,0),Complex(0,0),Complex(0,0)},
														  {Complex(0,0),Complex(0,0),Complex(0,0),Complex(0,0),Complex(1,0),Complex(0,0),Complex(0,0),Complex(0,0)},
														  {Complex(0,0),Complex(0,0),Complex(0,0),Complex(0,0),Complex(0,0),Complex(0,0),Complex(1,0),Complex(0,0)},
														  {Complex(0,0),Complex(0,0),Complex(0,0),Complex(0,0),Complex(0,0),Complex(1,0),Complex(0,0),Complex(0,0)},
														  {Complex(0,0),Complex(0,0),Complex(0,0),Complex(0,0),Complex(0,0),Complex(0,0),Complex(0,0),Complex(1,0)}};
	int theta_list[QB_SIZE3]={theta1,theta2,theta3}; int phi_list[QB_SIZE3]={phi1,phi2,phi3};
	return vec2ampl(multiply8x8(ampl2vec(QB_SIZE3,theta_list,phi_list),csw_matrix),QB_SIZE3);
}
int * Gates::TOF(int theta1, int phi1,int theta2,int phi2,int theta3,int phi3){
	Complex tof_matrix[QB3_MATRIX_SIZE][QB3_MATRIX_SIZE]={{Complex(1,0),Complex(0,0),Complex(0,0),Complex(0,0),Complex(0,0),Complex(0,0),Complex(0,0),Complex(0,0)},
														  {Complex(0,0),Complex(1,0),Complex(0,0),Complex(0,0),Complex(0,0),Complex(0,0),Complex(0,0),Complex(0,0)},
														  {Complex(0,0),Complex(0,0),Complex(1,0),Complex(0,0),Complex(0,0),Complex(0,0),Complex(0,0),Complex(0,0)},
														  {Complex(0,0),Complex(0,0),Complex(0,0),Complex(1,0),Complex(0,0),Complex(0,0),Complex(0,0),Complex(0,0)},
														  {Complex(0,0),Complex(0,0),Complex(0,0),Complex(0,0),Complex(1,0),Complex(0,0),Complex(0,0),Complex(0,0)},
														  {Complex(0,0),Complex(0,0),Complex(0,0),Complex(0,0),Complex(0,0),Complex(1,0),Complex(0,0),Complex(0,0)},
														  {Complex(0,0),Complex(0,0),Complex(0,0),Complex(0,0),Complex(0,0),Complex(0,0),Complex(0,0),Complex(1,0)},
														  {Complex(0,0),Complex(0,0),Complex(0,0),Complex(0,0),Complex(0,0),Complex(0,0),Complex(1,0),Complex(0,0)}};
	int theta_list[QB_SIZE3]={theta1,theta2,theta3}; int phi_list[QB_SIZE3]={phi1,phi2,phi3};
	return vec2ampl(multiply8x8(ampl2vec(QB_SIZE3,theta_list,phi_list),tof_matrix),QB_SIZE3);
}
int * Gates::DEU(int theta1, int phi1,int theta2,int phi2,int theta3,int phi3,int delta){
	float delta_float=(delta*M_PI)/180;
	Complex deu_matrix[QB3_MATRIX_SIZE][QB3_MATRIX_SIZE]={{Complex(1,0),Complex(0,0),Complex(0,0),Complex(0,0),Complex(0,0),Complex(0,0),Complex(0,0),               Complex(0,0)},
														  {Complex(0,0),Complex(1,0),Complex(0,0),Complex(0,0),Complex(0,0),Complex(0,0),Complex(0,0),               Complex(0,0)},
														  {Complex(0,0),Complex(0,0),Complex(1,0),Complex(0,0),Complex(0,0),Complex(0,0),Complex(0,0),				 Complex(0,0)},
														  {Complex(0,0),Complex(0,0),Complex(0,0),Complex(1,0),Complex(0,0),Complex(0,0),Complex(0,0),				 Complex(0,0)},
														  {Complex(0,0),Complex(0,0),Complex(0,0),Complex(0,0),Complex(1,0),Complex(0,0),Complex(0,0),				 Complex(0,0)},
														  {Complex(0,0),Complex(0,0),Complex(0,0),Complex(0,0),Complex(0,0),Complex(1,0),Complex(0,0),				 Complex(0,0)},
														  {Complex(0,0),Complex(0,0),Complex(0,0),Complex(0,0),Complex(0,0),Complex(0,0),Complex(0,cos(delta_float)),Complex(sin(delta_float),0)},
														  {Complex(0,0),Complex(0,0),Complex(0,0),Complex(0,0),Complex(0,0),Complex(0,0),Complex(sin(delta_float),0),Complex(0,cos(delta_float))}};
	int theta_list[QB_SIZE3]={theta1,theta2,theta3}; int phi_list[QB_SIZE3]={phi1,phi2,phi3};
	return vec2ampl(multiply8x8(ampl2vec(QB_SIZE3,theta_list,phi_list),deu_matrix),QB_SIZE3);	
}