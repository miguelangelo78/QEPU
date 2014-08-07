#include "uart.h"
Serial::Serial(){
	 //Set Baud rate
	 int ubrr=51;
	 UBRRL = ubrr;
	 UBRRH = (ubrr>>8);
	 
	 //Set Frame Format
	 UCSRC=(1<<URSEL)|(3<<UCSZ0);
	 
	 //Enable The receiver and transmitter
	 UCSRB=(1<<RXEN)|(1<<TXEN);
}
char Serial::read(){
	 //Wait until data is available
	 while(!(UCSRA & (1<<RXC)));
	 
	 //Now when USART receives data from host
	 //and it is available in the buffer
	 return UDR;
}
void Serial::write(char data){
	 //Wait until the transmitter is ready
	 while(!(UCSRA & (1<<UDRE)));
	 //Now write the data to USART buffer
	 UDR=data;
}
void Serial::writestr(char * data){
	for(int i=0;true;i++) if(data[i]=='\0') break; else write(data[i]);
}
void Serial::writeln(char data){
	write(data);
	write(13); //NEW LINE
}
void Serial::writestrln(char * data){
	writestr(data);
	write(13); //NEW LINE
}