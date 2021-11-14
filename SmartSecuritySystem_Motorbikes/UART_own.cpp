/*
 * UART_own.cpp
 *
 *  Created on: 29. 4. 2020
 *      Author: RPlsicik
 */
#include "mbed.h"
#include "Shields.h"
#include "uart.h"
#include "UARTSerial.h"
#include <stdio.h>

#include <wctype.h>

#include "GPS_commands.h"
#include "GPS_module.h"
#include "GSM_commands.h"
#include "GSM_module.h"
#include "TestingFcn.h"
#include "UART_own.h"

#include <array>

NHD_C0216CZ lcd;
Ticker UART_tic;
Serial uart(GPIO0,GPIO1,9600);

//Library variable
char IDK_uart_command[100];
char readUART[1200];
char readGSMcommand[1000];
char readUART_OLD[1200];
char readUART_OLD_decoding[500];
char Dissassemly_array_OLDinputArray[100];
char command_readUART[100];
bool Readed_ReadUart_wait;

float ReadUartWait_ms = 100;

//Global Variable used in fcn:
//int posArray = 0;	//global variable -> active position in array used in DecodeReadedData


//-------------------------------------------------------------------------------------------------------
//	Interrupt Fcn
//@ With 10ms basic waiting on readable uart
void IRQ_ReadUART(){
		if(uart.readable() == 1){
			//string formatedCommand[30];
			uart.scanf("%s",&readUART);
		}
		//10ms waiting on signal from UART
		wait_ms(10);
		UART_tic.detach();
}
void IRQ_GSM_ReadUART(){
		UART_tic.detach();
		if(uart.readable() == 1){
			uart.scanf("%s",&readUART);
		}
		else{
			//memset(readUART,' ',sizeof(readUART));
			memset(readUART,' ',50);
		}
		//wait_ms(10);
		//10ms waiting on signal from UART
//--------Feedback-------------Feedback-------------Feedback-------------Feedback-------------Feedback-------------

}

//@without feedback
void IRQ_ReadUART_wait(){
	if(uart.readable() == 1){
		char formatedCommand[30];
		uart.scanf("%s",&readUART);
	}
	//10ms waiting on signal from UART
	wait_ms(ReadUartWait_ms);
	UART_tic.detach();
}


/*
void ShowStruct(int PosStruct){
	char temp[60];
	sprintf(temp,"%s->%s\n%s->%s\n",SavedPositionData[PosStruct].North_South,SavedPositionData[PosStruct].Latitude,SavedPositionData[PosStruct].East_West,SavedPositionData[PosStruct].Longitude);

	lcd.displayString(temp);
	Send_UART(temp);
}
*/
//-------------------------------------------------------------------------------------------------------
// Esential Fcn
void Read_UART(){
	UART_tic.attach_us(&IRQ_ReadUART,10);
}
void Read_UART_Wait(float WaitOnSignal_ms){
	ReadUartWait_ms = WaitOnSignal_ms;
	Timer WaitTime;
	WaitTime.start();
	while(WaitTime <= WaitOnSignal_ms){
		UART_tic.attach_us(&IRQ_ReadUART_wait,10);
	}
	WaitTime.stop();
}
//@without output
void Read_GSM_UART_Wait(float WaitOnSignal_ms){
	ReadUartWait_ms = WaitOnSignal_ms;
	Timer WaitTime;
	WaitTime.start();
	while(WaitTime <= WaitOnSignal_ms){
		UART_tic.attach_us(&IRQ_GSM_ReadUART,10);
	}
	WaitTime.stop();
}
void Read_UART_scanf(){
	uart.scanf("%s ",&readUART);
}
//@with output
void Read_GSM_UART_Wait(int WaitOnSignal_ms,char outputData[50]){
	Timer WaitTime;
	int status;
	char tmp[5];
	//WaitTime.reset();
	WaitTime.start();
	//while(WaitTime.read_ms() <= WaitOnSignal_ms){
	while(WaitTime.read_ms() < WaitOnSignal_ms){
		sprintf(tmp,"%d",WaitTime.read_ms());
		lcd.displayStringToPos(tmp,1,1);
		//UART_tic.attach(&IRQ_GSM_ReadUART,1);
		UART_tic.attach_us(&IRQ_GSM_ReadUART,2000);
		//wait_us(20);
		//wait_ms(1);
	}
	WaitTime.stop();
	//WaitTime.reset();
	strcpy(outputData,readUART);

}
void Reading_UART(char output[100]){
	uart.scanf("%s",output);
}

//@Description
//@	NATIVE, basic FCN -> Send_UART data
//@FCN is sending multiple the same data
int Send_UART(char inputArray[]){
	bool StatusOfSend;
	StatusOfSend = uart.printf("%s\n",inputArray);
	StatusOfSend=0;
	return 0;
}

bool uart_IsReadable(){
	if(uart.readable()==1){
		return true;
	}
	else{
		return false;
	}

}

//@Description
//@
//@char inputArray[100] 	  <- DATA FOR SEND
//@char OLD_inputArray[100]   <- Old Sent data
//@ function compare new data with old sent data, do not send multiple the same data
//@ Return status of printf
//@							-> -1    	= ERROR
//@							-> Number	= Number of printed char
void Send_UART(char inputArray[],char OLD_inputArray[]){
	if(strcmp(OLD_inputArray,inputArray) != 0){
		uart.printf("%s\r\n",inputArray);
		strcpy(OLD_inputArray,inputArray);
		//Testing Error and blank array of printf
		/*if((StatusOfSend != -1)||(StatusOfSend != 0)){}*/
	}
}

int Send_UART_nonPrintableChar(int inputArray[],int SizeArray){
	bool StatusOfSend;
	int pos = 0;
	int CR = 0x0D;
	//int PRINTF_status;
	//while((pos <= SizeArray-1)&&(pos < 50)){ <- overload control pos<50
	while((pos <= SizeArray)){
		//PRINTF_status = uart.printf("%c",inputArray[pos]);
		uart.printf("%c",inputArray[pos]);
		//uart.printf("-> %d|",PRINTF_status);
		//uart.printf("\n");
		pos++;
	}
	uart.printf("%d",CR);
	pos = 0;
	StatusOfSend = 0;
	return StatusOfSend;
}
void Take_dataUART(char GlobalVarOutput[]){
	strcpy(GlobalVarOutput,readUART);
}

void Take_commandUART(char GlobalVarOutput[]){
	strcpy(GlobalVarOutput,command_readUART);
}
//-------------------------------------------------------------------------------------------------------
//Intermediate Fcn
/*
void Decode_NMEA_code(char inputArray[100],string outputData[30]){
	//detecting
	char *p_ch;
	char tempArray[150];
	char outputArray[20][100];
	int pos = 0;
	//Whole code with purpose decode received NMEA code
	p_ch = strtok(inputArray,",");
	while(p_ch != NULL){
		sprintf(outputArray[pos],"%s",p_ch);
		//outputData[pos] = tempArray;
		outputData[pos] = tempArray;
		pos++;
		if(pos > 20){
			pos = 0;
		}
	//is first char dollar?
		//if(inputArray[0] == 36){}
	}
}
*/

//@Testing Fcn
void Send_HexUART(){
	char HexArray[] = {0xff,0x56, 0xF0,0x01, 0x55, 0xAA,0x56, 0x0D};
	int Int_HexArray[] = {0xff,0x56, 0xF0,0x01, 0x57, 0x55, 0xAA,0x56, 0x0D};
	int lenght_Int_HexArray = sizeof(Int_HexArray)/sizeof(Int_HexArray[0]);
	int Commnd[]={0xB5,0x62,0x06,0x01,0x03,0x00,0xF0,0x01,0x01,0xFC,0x12,0x0D};
	int lenght_command = sizeof(Commnd)/sizeof(Commnd[0]);


	Send_UART(HexArray);
	Send_UART_nonPrintableChar(Int_HexArray,lenght_Int_HexArray);
	Send_UART_nonPrintableChar(Commnd,lenght_command);
}
//@Testing Fcn
int Save_UART_command(char outputArray[]){
	if((readUART[0] == '#')||(readUART[1] == '#')){/*first char is #*/
		strcpy(outputArray,readUART);
		return 1;
	}
	else{
		return 0;
	}
}



//@----------------------GPS control commands (WITHOUT CHIP SELECT)------------------|
//@										|											 |
//@- Function	->	99	AllCommands		|		- On_Off	--> true	= ENABLE 	 |
//@	     		->	1	Time			|					--> false	= DISABLE    |
//@				->	2	Location		|											 |
//@----------------------------------------------------------------------------------|
/*
void GPS_control(int function, bool On_Off){
	char tempArray1[30];
	if(function == 2){
		if(On_Off == true){
			sprintf(tempArray1,"Turn - ON POSITION -> \n");
			Send_UART(tempArray1);
			//Send_UART(GPS_POSITION_ENABLE);
			Send_UART_nonPrintableChar(GPS_POSITION_ENABLE,lenght_GPS_POSITION_ENABLE);
			//wait_ms(200);
		}
		else if(On_Off == false){
			sprintf(tempArray1,"Turn - OFF POSITION -> \n");
			Send_UART(tempArray1);
			//Send_UART(GPS_POSITION_DISABLE);
			Send_UART_nonPrintableChar(GPS_POSITION_DISABLE,lenght_GPS_POSITION_DISABLE);
			//wait_ms(200);
		}
	}
	else if(function == 1){
		if(On_Off == true){
			sprintf(tempArray1,"Turn - ON POSITION -> \n");
			Send_UART(tempArray1);
			//Send_UART(GPS_TIME_ENABLE);
			Send_UART_nonPrintableChar(GPS_TIME_ENABLE,lenght_GPS_TIME_ENABLE);
			//wait_ms(200);
		}
		else if(On_Off == false){
			sprintf(tempArray1,"Turn - OFF TIME -> \n");
			Send_UART(tempArray1);
			//Send_UART(GPS_TIME_DISABLE);
			Send_UART_nonPrintableChar(GPS_TIME_ENABLE,lenght_GPS_TIME_ENABLE);
			//wait_ms(200);
		}
	}
	else if(function == 99){
		if(On_Off == true){
			sprintf(tempArray1,"Turn - ON ALL COMMANDS -> \n");
			Send_UART(tempArray1);
			//Send_UART(GPS_AllCommands_ENABLE);
			Send_UART_nonPrintableChar(GPS_AllCommands_ENABLE,lenght_GPS_AllCommands_ENABLE);
			//wait_ms(200);
		}
		else if(On_Off == false){
			sprintf(tempArray1,"Turn - OFF ALL COMMANDS -> \n");
			Send_UART(tempArray1);
			//Send_UART(GPS_AllCommands_DISABLE);
			Send_UART_nonPrintableChar(GPS_AllCommands_DISABLE,lenght_GPS_AllCommands_DISABLE);
			//00
			//wait_ms(200);
		}
	}
}
*/
/*
void SavedPosition(){
if(strcmp(readUART_OLD_decoding,readUART) != 0){
	strcpy(readUART_OLD_decoding,readUART);

	char Array[100];
	//DetectCommand with hastag


	//Detect data from GPS
	if (readUART[0] == '$') {
		//
		strcpy(Array,readUART);
		//Separate char array by comma
		//Dolna Marikova Lat/Long ->
		//(49.211789, 18.351985)
		//49° 12' 42.4404'' N 18° 21' 7.146'' E

		char *p_CH;
		char tmp[20];
		p_CH = strtok(Array,",");
		//Find last filled struct in array //output is posArray which is not filled //If is full array point on the first mark all array like nonfilled
			bool stopWhile = 0;
			while((posArray <= (sizeof(SavedPositionData)/sizeof(SavedPositionData[0])))&&(stopWhile == 0)){
				if(SavedPositionData[posArray].Filled == 0){
					stopWhile = 1;
				}
				else if(posArray > (sizeof(SavedPositionData)/sizeof(SavedPositionData[0]))){
					posArray = 0;
					stopWhile = 1;
				}
				else{
					posArray++;
				}
			}

		//Filling struct on position //posArray/ with Geolocation data
			short position = 0;
			while(p_CH != NULL){
				position++;
				sprintf(tmp,"%s",p_CH);
				p_CH = strtok(NULL,",");
				uart.printf("%d - %s\n",position,tmp);
				if(position == 2){
					//Current latitude
					strcpy(SavedPositionData[posArray].Latitude,tmp);
					SavedPositionData[posArray].Filled = 1;

				}
				else if (position == 3) {
					//North/South orientation
					//Latitude
					strcpy(SavedPositionData[posArray].North_South,tmp);

				}
				else if (position == 4) {
					//Current longitude
					strcpy(SavedPositionData[posArray].Longitude,tmp);

				}
				else if(position == 5){
					//East/West orientation
					//Longitude
					strncpy(SavedPositionData[posArray].East_West,tmp,1);

				}
				else{
				}
				//Last row of while;
				wait_us(10);
			}

	}
	else{

	}
}
*/
/*
void DecodeReadedData(){
if(strcmp(readUART_OLD_decoding,readUART) != 0){
	strcpy(readUART_OLD_decoding,readUART);

	char Array[100];
	//DetectCommand with hastag
	if((readUART[0] == '#')||(readUART[1] == '#')){
		char temporaryArray_pos1[100];
		char temporaryArray_pos2[100];
		Dissassemly_array(readUART,temporaryArray_pos1,1);
		Dissassemly_array(readUART,temporaryArray_pos2,2);
		strncpy(Array,readUART,100);
		if(strcmp(Array,"#Show")== 0){
			if(strcmp(temporaryArray_pos1,"at")== 0){
				//replace by atoi();
				//int position = temporaryArray_pos2 - '0';
				ShowStruct(atoi(temporaryArray_pos2));
			}
			else{
				for(int n=0;n<=(sizeof(SavedPositionData)/sizeof(SavedPositionData[0]));n++){
					ShowStruct(n);
				}
			}
		}
		else if (strcmp(Array,"#GPS_OFF")==0) {
			GPS_control(99,False);
		}
		else if (strcmp(Array,"#GPS_ON")==0) {
			GPS_control(99,True);
		}
		else if (strcmp(Array,"#GPS_pos_ON")==0) {
			GPS_control(2,True);
		}
		else if (strcmp(Array,"#GPS_pos_OFF")==0) {
			GPS_control(2,True);
		}

	}

	//Detect data from GPS
	else if (readUART[0] == '$') {
		//
		strcpy(Array,readUART);
		//Separate char array by comma
		//Dolna Marikova Lat/Long ->
		//(49.211789, 18.351985)
		//49° 12' 42.4404'' N 18° 21' 7.146'' E

		char *p_CH;
		char tmp[20];
		p_CH = strtok(Array,",");

		//Find last filled struct in array //output is posArray which is not filled //If is full array point on the first mark all array like nonfilled

			bool stopWhile = 0;
			while((posArray <= (sizeof(SavedPositionData)/sizeof(SavedPositionData[0])))&&(stopWhile == 0)){
				if(SavedPositionData[posArray].Filled == 0){
					stopWhile = 1;
				}
				else if(posArray > (sizeof(SavedPositionData)/sizeof(SavedPositionData[0]))){
					posArray = 0;
					stopWhile = 1;
				}
				else{
					posArray++;
				}
			}

		//Filling struct on position //posArray/ with Geolocation data
			short position = 0;
			while(p_CH != NULL){
				position++;
				sprintf(tmp,"%s",p_CH);
				p_CH = strtok(NULL,",");
				uart.printf("%d - %s\n",position,tmp);
				if(position == 2){
					//Current latitude
					strcpy(SavedPositionData[posArray].Latitude,tmp);
					SavedPositionData[posArray].Filled = 1;

				}
				else if (position == 3) {
					//North/South orientation
					//Latitude
					strcpy(SavedPositionData[posArray].North_South,tmp);

				}
				else if (position == 4) {
					//Current longitude
					strcpy(SavedPositionData[posArray].Longitude,tmp);

				}
				else if(position == 5){
					//East/West orientation
					//Longitude
					strncpy(SavedPositionData[posArray].East_West,tmp,1);

				}
				else{
				}
				//Last row of while;
				wait_us(10);
			}
	}
}
else{

}
}
*/

//@ Target -> 	1	-	lcd
//@ 			2	-	uart
/*
void ShowDecodedData(bool target,int PosStruct){
	char tempOutput_1[50];
	char tempOutput_2[50];
	sprintf(tempOutput_1,"%s:%s %s:%s\n",SavedPositionData[PosStruct].North_South,SavedPositionData[PosStruct].Latitude);
	sprintf(tempOutput_2,"%s:%s\n",SavedPositionData[PosStruct].East_West,SavedPositionData[PosStruct].Longitude);
	if(target == true){
		//LCD output
		lcd.clearDisplay();
		lcd.displayStringToPos(tempOutput_1,1,1);
		lcd.displayStringToPos(tempOutput_2,2,1);
	}
	else{
		//UART output
		Send_UART(tempOutput_1);
		Send_UART(tempOutput_2);
	}
}
*/
/*
int Dissassemly_array(char inputArray[100],char outputArray[100],int position){
	if(strcmp(inputArray,Dissassemly_array_OLDinputArray) != 0){
		strcpy(Dissassemly_array_OLDinputArray,inputArray);
		char *p_CH;
		int iteration = 0;
		p_CH = strtok(inputArray,",");

		while(p_CH != NULL){
			if(iteration == position){
				strcpy(outputArray,p_CH);
				return iteration;
			}
			iteration++;
			p_CH = strtok(NULL,",");

		}
	}
	else{
		return 999;
	}
}
*/
