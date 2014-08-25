#include "utils.h"

char * Utils::int2binstr(int num,int strlength){
	char * str = (char*)malloc(strlength + 1);
	if(!str) return NULL;
	str[strlength] = 0;

	// type punning because signed shift is implementation-defined
	unsigned u = *(unsigned *)&num;
	for(; strlength--; u >>= 1)
	str[strlength] = u & 1 ? '1' : '0';

	return str;
}

char* Utils::char2str(char c){
	char*str=(char*)malloc(sizeof(char));
	sprintf(str,"%d",c);
	return str;
}
int*  Utils::str2intarr(char* c){
	int c_length=0;
	for(int i=0;true;i++) if(c[i]==0xFF && c[i-1]==0xFF && c[i-2]==0xFF && c[i-3]==0xFF && c[i-4]==0xFF) break; else c_length++; c_length-=4;
	
	int *intarr=(int*)malloc(sizeof(int)*(c_length+5));
	for(int i=0;i<c_length;i++) intarr[i]=(int)c[i];
	for(int i=c_length;i<c_length+5;i++) intarr[i]=0xFF;
	return intarr;
}
int Utils::countdigits(int dec){
	int counter=0;
	while(dec!=0){
		dec/=10;
		++counter;
	}
	return counter;
}
char* Utils::int2str(int dec){
	char str[10];
	sprintf(str,"%d",dec);
	return str;
}
int Utils::dec2hex(int dec){
	char str[100];
	sprintf(str,"%x",dec);
	return (int)strtol(str,NULL,16);
}
int Utils::concint(int n1,int n2){
	char str1[10],str2[10];
	sprintf(str1,"%d",n1);
	sprintf(str2,"%d",n2);
	strcat(str1,str2);
	return atoi(str1);
}
int Utils::delay(int ms){
	while(ms--) _delay_ms(1);
}
float Utils::custom_pow(float base,int exp){
	float result=1; for(int i=0;i<exp;i++) result*=base;
	return result;
}