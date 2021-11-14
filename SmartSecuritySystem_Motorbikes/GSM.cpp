/*
 * GSM.cpp
 *
 *  Created on: 2. 5. 2020
 *      Author: RPlsicik
 */
#include "mbed.h"
#include "Shields.h"
#include "uart.h"
#include "UARTSerial.h"
#include <stdio.h>
#include <string.h>

#include <wctype.h>

#include "TestingFcn.h"
#include "UART_own.h"

#include "GPS_commands.h"
#include "GPS.h"

#include "GSM.h"


NHD_C0216CZ lcd;
Serial uart(GPIO0,GPIO1,9600);
//@Pin	 GPIOpin   purpose	addres	status of pin	Interrupt
//@p14 	 GPIO14,	DIO14	J8.5	  Correct			-
DigitalInOut ChipSelect_GSM(GPIO16);
void GSM::Communication(char input[100],char output[100], float WaitTime_ms){
	if(GSM::TurnOn != 1){
		GSM::activate();
	}
	GSM::send(input);
	GSM::read(WaitTime_ms);
	strcpy(output,GSM::readed_data);
}

void GSM::activate(){
	//deactivate other peripherials on same UART BUS
	//activate GPS module
	if(TurnOn == false){
		int AntiFreezeCounter = 0;	//Anti bug
		while((uart_IsReadable() == 1)&&(AntiFreezeCounter <= 100)){
			wait_us(10);
			AntiFreezeCounter++;
		}
		ChipSelect_GSM.output();
		ChipSelect_GSM.mode(PullDown);
		ChipSelect_GSM.write(0);
		//wait until to end of noise
		wait_ms(100);
		TurnOn = true;
	}
	else{
	}
}

void GSM::deactivate(){
	ChipSelect_GSM.output();
	ChipSelect_GSM.mode(PullUp);
	ChipSelect_GSM.write(1);
	TurnOn = false;
}

//&	Anti-Sending system for multiple send the same command
void GSM::send(char inputArray[100]){
	if(strcmp(inputArray,OLD_send_array)!=0){
		strcpy(OLD_send_array,inputArray);	//How to using variable of class
		Send_UART(inputArray);
	}
	else{
		//If last time was send same command. It will not send again.
	}
}


void GSM::read(float WaitTime_ms){	//WaitTime_ms
	//float WaitTime_us = WaitTime_ms*1000;
	char newReadData[100];
	strcpy(newReadData,GSM::readed_data);
	//Read_UART();
	int Wait = WaitTime_ms;									//Read_UART_Wait(Wait);
	Read_GSM_UART_Wait(Wait);
	Take_dataUART(readed_data);

	//readed_data is output of function;
}
void GSM::scaning(char output[100]){
	uart.scanf("%s",&output);
}

void GSM::only_read(){
	if(TurnOn == true){
//		while(1){
			Read_UART();
			Take_dataUART(readed_data);
//}
	}
	else{
		activate();
		only_read();
	}
}


void GSM::TestAT(char Output[13]){
	//Timer SecureCoundown;
	//SecureCoundown.start();
	uart.printf("AT\n");
	//uart.scanf("%s",&Output);
	Reading_UART(Output);
	/*
	while(SecureCoundown.read_ms()<800){
		if(uart.readable()==1){
			uart.scanf("%s",&Output);
		}
		else{
			//Do nothing
		}
	}
	*/
}
//FCN of operation with GSM module
void GSM::Call(char mobileNumber[13]){
	char tmpTxT[40];
	sprintf(tmpTxT,"ATD%s;\n",mobileNumber);
	uart.printf("%s",tmpTxT);
	//memset(tmpTxT,' ',sizeof(tmpTxT));
	//uart.scanf("%s",tmpTxT);
	//Reading_UART(tmpTxT);
	//strcpy(TypeOfOutput,tmpTxT);
	//dopisat pripad NO DIALTONE, NO CARRIAGE atd...
}
//@ mobile number write only number (ATD+421)| 944 542 714
void GSM::CallwithEnd(char mobileNumber[13], int TimeOfCalling_s, char TypeOfOutput[20]){
	Timer TimeRinging;
	char tmpTxT[40];
	int TimeOfCalling_ms = TimeOfCalling_s*1000;
	memset(tmpTxT,' ',sizeof(tmpTxT));
	memset(TypeOfOutput,' ',sizeof(TypeOfOutput));
	uart.printf("ATD+421%s;\n",mobileNumber);
	TimeRinging.start();
	//uart.scanf("%s",&tmpTxT);		//Try secure scanf in to the while put test uart.readable
	Reading_UART(tmpTxT);
	if(strcmp(tmpTxT,"OK")==0){
		TypeOfOutput = "OK\n";
		//Zamrzne
		char lcdOutput[15];
		//while(TimeRinging.read_ms()<TimeOfCalling_ms){
		lcd.clearDisplay();
		while(TimeRinging.read_ms()<= TimeOfCalling_ms){
			sprintf(lcdOutput,"%d ms",TimeRinging.read_ms());
			lcd.displayStringToPos(lcdOutput,1,1);
		}

		uart.printf("ATH\n");
	}
	else{
		TypeOfOutput = "ERROR\n";
	}
	lcd.clearDisplay();
	TimeRinging.stop();
}

//algoritmus GSM modulu
void GSM::AnswerTheCall(char TypeOfOutput[50]){
	Timer cas;
	char Data_1[12];
	char Data_2[12];
	char tmp[25];
	int time;
	//Mutex command;
	send("ATA\n");
	//cas.start();
	uart.scanf("%s %s",&Data_1);
	//time = cas.read();

	//cas.reset();
	//uart.scanf("%s %s",&Data_1,&Data_2);
	wait_ms(200);
	sprintf(TypeOfOutput,"-> %s",&Data_1);
	lcd.clearDisplay();
	lcd.displayStringToPos(TypeOfOutput,2,1);
	//cas.stop();
	//wait(2);
}

void GSM::IsRinging(int status_output){
	activate();
	char tmp[10];
	int status;
	status_output = 0;
	//status = CompareArray("RING",RINGconstant);
	Read_GSM_UART_Wait(1000,tmp);
	//status = strcmp(tmp,"RING");
	status_output = strcmp(tmp,"RING");
	/*
	if(status == 0){ //strstr
		status_output = 1;
	}
	else{
		status_output = 0;
	}
	*/
}

void GSM::WriteSMS(char mobileNumber[20],char text[160]){
	int SubstituteChar = 0x1A;
	uart.printf("AT+CMGF=1\n");	//Activate text mode
	wait_us(500);
	uart.printf("AT+CMGS=\"%s\"\n",mobileNumber);	//activate start of write sms
	wait_us(500);
	//->place for write sms to send substring char
	uart.printf("%s\n",text);	//recommendet lenght of text is 64
	wait_us(500);
	uart.printf("%c\n",SubstituteChar);
	wait(3);
}

/*
void GSM::WriteSMS(char mobileNumber,char text[160]);
*/




GSM::GSM() {
	// TODO Auto-generated constructor stub

}

GSM::~GSM() {
	// TODO Auto-generated destructor stub
}

bool GSM::CompareArray(char array_1[],char array_2[]){
	int lenght;
	int lenght_1 = sizeof(array_1);
	int lenght_2 = sizeof(array_2);
	bool output;
	if(lenght_1 > lenght_2){
		lenght = lenght_1;
	}
	else{
		lenght = lenght_2;
	}

	for(int n=0;n <= lenght; n++){
		if(array_1[n] != array_2[n]){
			output = 0;
			return output;
		}
		else{
			output = 1;
		}
	}
	return output;
}
