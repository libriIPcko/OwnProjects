//Micricontroller		:All 8-bit AVR Micricontrollers with Similar Register Configuration with ATmega16
//System Clock			:1 MHz - 16 MHz
//Header File Version 		:1.1
//Author			:Arun Kumar Garg 
//				:ABLab Solutions
//				:www.ablab.in
//				:info@ablab.in
//Date				:1st August 2011

/*Copyright (c) 2011 ABLab Solutions All rights reserved.
Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the 	following disclaimer in the documentation 
     and/or other materials provided with the distribution.
   * Neither the name of the copyright holders nor the names of contributors may be used to endorse or promote products derived from this software without 
     specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */


/*The functions and macros defined in this header file are for 4X4 Keypad.*/

#ifndef	_KEYPAD_H_
#define	_KEYPAD_H_	1


#include<avr/io.h>
#include<util/delay.h>



/*This macro or preprocessor defines the PORT Register to which 4X4 Keypad is connected.*/

#ifndef KEYPAD_PORT
# warning "KEYPAD_PORT is not defined for keypad.h.Default PORT Register is PORTB."
# define KEYPAD_PORT	PORTB	 
#endif



/*This macro or preprocessor defines the PIN Register to which 4X4 Keypad is connected.*/

#ifndef KEYPAD_PIN
# warning "KEYPAD_PIN is not defined for keypad.h. Default PIN Register is PINB."*/
# define KEYPAD_PIN	PINB	 
#endif



/*This function is declared to read the value of the pressed key of the 4X4 keypad. When no key is pressed, the function returns 0xff.*/

unsigned char read_keypad(void);



/*Function defination*/
unsigned char read_keypad(void)
{
	unsigned char keypad_input=0xff,keypad_output=0xff;
	KEYPAD_PORT=0xfe;
	_delay_us(2);
	keypad_input=KEYPAD_PIN & 0xf0;

	if(keypad_input==0xe0)
		keypad_output=0x01;
	else if(keypad_input==0xd0)
		keypad_output=0x02;
	else if(keypad_input==0xb0)
		keypad_output=0x03;
	else if(keypad_input==0x70)
		keypad_output=0x0c;
	else 
		;

	KEYPAD_PORT=0xfd;
	_delay_us(2);
	keypad_input=KEYPAD_PIN & 0xf0;

	if(keypad_input==0xe0)
		keypad_output=0x04;
	else if(keypad_input==0xd0)
		keypad_output=0x05;
	else if(keypad_input==0xb0)
		keypad_output=0x06;
	else if(keypad_input==0x70)
		keypad_output=0x0d;
	else 
		;

	KEYPAD_PORT=0xfb;
	_delay_us(2);
	keypad_input=KEYPAD_PIN & 0xf0;

	if(keypad_input==0xe0)
		keypad_output=0x07;
	else if(keypad_input==0xd0)
		keypad_output=0x08;
	else if(keypad_input==0xb0)
		keypad_output=0x09;
	else if(keypad_input==0x70)
		keypad_output=0x0e; 
	else 
		;		
		
	KEYPAD_PORT=0xf7;
	_delay_us(2);
	keypad_input=KEYPAD_PIN & 0xf0;

	if(keypad_input==0xe0)
		keypad_output=0x0a; 
	else if(keypad_input==0xd0)
		keypad_output=0x00;
	else if(keypad_input==0xb0)
		keypad_output=0x0b;
	else if(keypad_input==0x70)
		keypad_output=0x0f;
	else 
		;

	return keypad_output;
}
#endif




