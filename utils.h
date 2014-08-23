#ifndef UTILS_H_
#define UTILS_H_
#include "qepu_constants.h"
#include <util/delay.h>
#include <math.h>
#include <avr/io.h>
#include <limits.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
class Utils{
	public:
		char* char2str(char c);
		int* str2intarr(char* c);
		int dec2hex(int dec);
		char* int2str(int dec);
		int countdigits(int dec);
		int concint(int n1,int n2);
		int delay(int ms);
		char *int2binstr(int num,int strlength);
};
#endif