/*
	(c) 2021 Dmitry Grinberg   https://dmitry.gr
	Non-commercial use only OR licensing@dmitry.gr
*/

#include <stdbool.h>
#include "printf.h"


static void StrPrvPrintfEx_number(uint32_t number, bool zeroExtend, uint32_t padToLength, uint32_t base, bool signedNum)
{
	char buf[64];
	uint32_t idx = sizeof(buf) - 1;
	uint32_t chr, i;
	bool neg = false;
	uint32_t numPrinted = 0;
	
	if (signedNum && number >> 31) {
		neg = true;
		number = -number;
	}
	
	if(padToLength > 31)
		padToLength = 31;
	
	buf[idx--] = 0;	//terminate
	
	do{
		chr = number % base;
		number = number / base;
		
		buf[idx--] = (chr >= 10)?(chr + 'A' - 10):(chr + '0');
		
		numPrinted++;
		
	}while(number);
	
	if (neg) {
	
		buf[idx--] = '-';
		numPrinted++;
	}
	
	if (padToLength > numPrinted)
		padToLength -= numPrinted;
	else
		padToLength = 0;
	
	while(padToLength--) {
		
		buf[idx--] = zeroExtend?'0':' ';
		numPrinted++;
	}
	
	idx++;
	
	for (i = 0; i < numPrinted; i++)
		_putchar((buf + idx)[i]);
}

void printf(const char* fmtStr, ...){
	
	char c, t;
	uint32_t val32;
	va_list vl;
	
	va_start(vl, fmtStr);

	
	while((c = *fmtStr++) != 0){
		
		if(c == '\n'){
			_putchar(c);
		}
		else if(c == '%'){
			
			bool zeroExtend = false;
			uint32_t padToLength = 0;
			const char* str;
			
more_fmt:
			
			c = *fmtStr++;
			
			switch(c){
				
				case '%':
					
					_putchar(c);
					break;
				
				case 'c':
					
					t = va_arg(vl,unsigned int);
					_putchar(t);
					break;
				
				case 's':
					
					str = va_arg(vl,char*);
					if(!str) str = "(null)";
					while ((c = *str++))
						_putchar(c);
					break;
				
				case '0':
					
					if(!zeroExtend && !padToLength){
						
						zeroExtend = true;
						goto more_fmt;
					}
				
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
					
					padToLength = (padToLength * 10) + c - '0';
					goto more_fmt;
				
				case 'd':
					val32 = va_arg(vl,int32_t);
					StrPrvPrintfEx_number(val32, zeroExtend, padToLength, 10, true);
					break;
					
				case 'u':
					val32 = va_arg(vl,uint32_t);
					StrPrvPrintfEx_number(val32, zeroExtend, padToLength, 10, false);
					break;
					
				case 'x':
				case 'X':
					val32 = va_arg(vl,uint32_t);
					StrPrvPrintfEx_number(val32, zeroExtend, padToLength, 16, false);
					break;
					
				case 'l':
					//long == int
					goto more_fmt;
				
				default:
					_putchar(c);
					break;
				
			}
		}
		else
			_putchar(c);
	}

	va_end(vl);
}
