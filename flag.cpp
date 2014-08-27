#include "qepu.h"

int8_t QEPU::Flag::flaglist[FLAG_COUNT];

QEPU::Flag::Flag(){
	flaglist[ALLOW_BRANCH_STACK]=HIGH;	
}

void QEPU::Flag::compare(int data1,int data2){
	flaglist[CND_LWER]=(data1<data2)?HIGH:LOW;
	flaglist[CND_LWER_EQUAL]=(data1<=data2)?HIGH:LOW;
	flaglist[CND_EQUAL]=(data1==data2)?HIGH:LOW;
	flaglist[CND_GRTER_EQUAL]=(data1>=data2)?HIGH:LOW;
	flaglist[CND_GRTER]=(data1>data2)?HIGH:LOW;
	flaglist[CND_DIFF]=(data1!=data2)?HIGH:LOW;
	flaglist[CND_ZERO]=(data1==LOW)?HIGH:LOW;
	flaglist[CND_NOT_ZERO]=(data1!=LOW)?HIGH:LOW;
}