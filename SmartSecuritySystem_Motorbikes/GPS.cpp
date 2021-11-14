/*
 * GPS.cpp
 *
 *  Created on: 1. 5. 2020
 *      Author: RPlsicik
 */

#include "GPS.h"
#include "mbed.h"
#include "Shields.h"
#include "uart.h"
#include "UARTSerial.h"
#include <stdio.h>

#include <string.h>

#include <wctype.h>

#include "GPS_commands.h"
#include "TestingFcn.h"
#include "UART_own.h"

//@Pin	 GPIOpin   purpose	addres	status of pin	Interrupt
//@p14 	 GPIO14,	DIO14	J8.5	  Correct			-
NHD_C0216CZ lcd;
DigitalInOut ChipSelect_GPS(GPIO12);

using namespace std;


void GPS::activate(){
	if(TurnOn == false){
		int AntiFreezeCounter = 0;	//Anti bug
		while((uart_IsReadable() == 1)&&(AntiFreezeCounter <= 100)){
			wait_us(10);
			AntiFreezeCounter++;
		}
		ChipSelect_GPS.output();
		ChipSelect_GPS.mode(PullDown);
		ChipSelect_GPS.write(0);
		//wait until to end of noise
		wait_ms(100);
		TurnOn = true;
	}
	else{

	}
}
void GPS::deactivate(){
	ChipSelect_GPS.output();
	ChipSelect_GPS.mode(PullUp);
	ChipSelect_GPS.write(1);

	TurnOn = false;
}
//&	Anti-Sending system for multiple send the same command
void GPS::send(char inputArray[100]){
	if(strcmp(inputArray,OLD_send_array)!=0){
		strcpy(OLD_send_array,inputArray);	//How to using variable of class
		Send_UART(inputArray);
	}
	else{
		//If last time was send same command. It will not send again.
	}

}

void GPS::only_read(){
	if(TurnOn == true){
		//while(1){	// to while add end of read from BUS
			Read_UART();
			Take_dataUART(readed_data);
		//}
	}
	else{
		activate();
		only_read();
	}
}
void GPS::waitTo_read(){
	if(TurnOn == true){
		//while(1){	// to while add end of read from BUS
		//Read_UART();
		Read_UART_scanf();
		Take_dataUART(readed_data);
		//}
	}
	else{
		activate();
		only_read();
	}
}
//@Schvalne poskodena funkcia
void GPS::only_read_waitOnInput(float waitTime_uS){
	//Read_UART_Wait(float waitTime_uS);
	Take_dataUART(readed_data);
}

void GPS::take_position(){
}




//@	Tartget	->	1	LCD output
//@			->	2	UART output
void GPS::ShowInputdData(bool target){
	if(strcmp(GPS::ShowInputdData_OLD,readed_data) != 0){
		strcpy(GPS::ShowInputdData_OLD,readed_data);
		if(target == true){
				//LCD output
				lcd.clearDisplay();
				lcd.displayStringToPos(readed_data,1,1);
			}
			else{
				//UART output
				Send_UART(readed_data);
			}
	}

}
/*
void GPS::ShowStruct(int PosStruct){
	char temp[60];
	//sprintf(temp,"%s->%s\n%s->%s\n",GPS::SavedPositionData[PosStruct]->North_South ,GPS::SavedPositionData[PosStruct]->Latitude ,GPS::SavedPositionData[PosStruct]->East_West ,GPS::SavedPositionData[PosStruct].Longitude);
	//sprintf(temp,"%s->%s\n%s->%s\n",GPS.SavedPositionData[PosStruct].North_South ,GPS.SavedPositionData[PosStruct].Latitude ,GPS.SavedPositionData[PosStruct].East_West ,GPS.SavedPositionData[PosStruct].Longitude);
	sprintf(temp,"%s->%s\n%s->%s\n",GPS::SavedPositionData[PosStruct].North_South, GPS::SavedPositionData[PosStruct].Latitude, GPS::SavedPositionData[PosStruct].East_West, GPS::SavedPositionData[PosStruct].Longitude);

	lcd.displayString(temp);
	Send_UART(temp);
}
*/

void GPS::ShowStruct(int PosStruct,char Latitude[9],char Longitude[9]){
	//char temp[60];
	//sprintf(temp,"%s->%s\n%s->%s\n",GPS::SavedPositionData[PosStruct].North_South, GPS::SavedPositionData[PosStruct].Latitude, GPS::SavedPositionData[PosStruct].East_West, GPS::SavedPositionData[PosStruct].Longitude);
	sprintf(Latitude,"%s",GPS::SavedPositionData[PosStruct].Latitude);
	sprintf(Longitude,"%s",GPS::SavedPositionData[PosStruct].Longitude);
}

//@ Target -> 	1	-	lcd
//@ 			2	-	RawOutput to variable
//@				21	-	RawOutput to variables -> GPS::SavedPositionData[PosStruct].Latitude
//@											   -> GPS::SavedPositionData[PosStruct].Longitude
//@ 			3	-	uart
void GPS::ShowDecodedData(int target,int PosStruct){
	char tempOutput_1[50];
	char tempOutput_2[50];
	//sprintf(tempOutput_1,"%s:%s %s:%s\n",GPS::SavedPositionData[PosStruct].North_South,GPS::SavedPositionData[PosStruct].Latitude);
	sprintf(tempOutput_2,"%s:%s\n",GPS::SavedPositionData[PosStruct].East_West,GPS::SavedPositionData[PosStruct].Longitude);
	if(target == 1){
		//LCD output
		lcd.clearDisplay();
		//lcd.displayStringToPos(tempOutput_1,1,1);
		lcd.displayStringToPos(tempOutput_2,1,1);
	}
	if(target == 2){
		//ShowDecodedData_outputVar[sizeof(ShowDecodedData_outputVar];
		char temporaryOutput[30];
		sprintf(temporaryOutput,"%s %s",GPS::SavedPositionData[PosStruct].Latitude,GPS::SavedPositionData[PosStruct].Longitude);
		//strcpy(temporaryOutput,ShowDecodedData_outputVar);
		strcpy(ShowDecodedData_outputVar,temporaryOutput);
		//lcd.displayStringToPos(temporaryOutput,1,1);
	}
	if(target == 21){
		//ShowDecodedData_outputVar[sizeof(ShowDecodedData_outputVar];
		char temporaryOutput[30];
		sprintf(temporaryOutput,"%s",GPS::SavedPositionData[PosStruct].Latitude);
		strcpy(ShowDecodedData_latitude,temporaryOutput);
		sprintf(temporaryOutput,"%s",GPS::SavedPositionData[PosStruct].Longitude);
		strcpy(ShowDecodedData_longitude,temporaryOutput);
	}
	else{
		//UART output
		Send_UART(tempOutput_1);
		Send_UART(tempOutput_2);
	}
}

//@----------------------GPS control commands (WITHOUT CHIP SELECT)------------------|
//@										|											 |
//@- Function	->	99	AllCommands		|		- On_Off	--> true	= ENABLE 	 |
//@	     		->	1	Time			|					--> false	= DISABLE    |
//@				->	2	Location		|											 |
//@----------------------------------------------------------------------------------|
void GPS::control(int function,bool On_Off){
	//input is special constant
	char tempArray1[100];
	if(function == 2){
			if(On_Off == true){
				//sprintf(tempArray1,"Turn - ON POSITION -> \n");
				//Send_UART(tempArray1);
				//Send_UART(GPS_POSITION_ENABLE);
				Send_UART_nonPrintableChar(GPS_POSITION_ENABLE,lenght_GPS_POSITION_ENABLE);
				wait_ms(200);
			}
			else if(On_Off == false){
				//sprintf(tempArray1,"Turn - OFF POSITION -> \n");
				//Send_UART(tempArray1);
				//Send_UART(GPS_POSITION_DISABLE);
				Send_UART_nonPrintableChar(GPS_POSITION_DISABLE,1112);
				wait_ms(200);
			}
		}
		else if(function == 1){
			if(On_Off == true){
				//sprintf(tempArray1,"Turn - ON TIME -> \n");
				//Send_UART(tempArray1);
				//Send_UART(GPS_TIME_ENABLE);
				Send_UART_nonPrintableChar(GPS_TIME_ENABLE,lenght_GPS_TIME_ENABLE);
				wait_ms(200);
			}
			else if(On_Off == false){
				//sprintf(tempArray1,"Turn - OFF TIME -> \n");
				//Send_UART(tempArray1);
				//Send_UART(GPS_TIME_DISABLE);
				Send_UART_nonPrintableChar(GPS_TIME_ENABLE,lenght_GPS_TIME_ENABLE);
				wait_ms(200);
			}
		}
		else if(function == 99){
			if(On_Off == true){
				//sprintf(tempArray1,"Turn - ON ALL COMMANDS -> \n");
				//Send_UART(tempArray1);
				//Send_UART(GPS_AllCommands_ENABLE);
				Send_UART_nonPrintableChar(GPS_AllCommands_ENABLE,lenght_GPS_AllCommands_ENABLE);
				wait_ms(200);
			}
			else if(On_Off == false){
				//sprintf(tempArray1,"Turn - OFF ALL COMMANDS -> \n");
				//Send_UART(tempArray1);
				//Send_UART(GPS_AllCommands_DISABLE);
				Send_UART_nonPrintableChar(GPS_AllCommands_DISABLE,lenght_GPS_AllCommands_DISABLE);
				//00
				wait_ms(200);
			}
		}
}

int LastPosOfStruct(){
}

void GPS::SavePosition(){
	if(strcmp(SavePosOldData,GPS::readed_data) != 0){
		strcpy(SavePosOldData,GPS::readed_data);
		char Array[100];
		//DetectCommand with hastag
		//Detect data from GPS
		if (GPS::readed_data[0] == '$') {
			GPS::posArray_AbsoluteLenght = (sizeof(GPS::SavedPositionData)/sizeof(GPS::SavedPositionData[0]));
			strcpy(Array,GPS::readed_data);
			//Separate char array by comma
			//Dolna Marikova Lat/Long ->
			//(49.211789, 18.351985)
			//49° 12' 42.4404'' N 18° 21' 7.146'' E
			char *p_CH;
			char tmp[20];
			p_CH = strtok(Array,",");
			//Find last filled struct in array //output is posArray which is not filled //If is full array point on the first mark all array like nonfilled
				bool stopWhile = 0;
				while((posArray <= (sizeof(GPS::SavedPositionData)/sizeof(GPS::SavedPositionData[0])))&&(stopWhile == 0)){
					if(GPS::SavedPositionData[posArray].Filled == 0){
						stopWhile = 1;
					}
					else if(posArray >= (sizeof(GPS::SavedPositionData)/sizeof(GPS::SavedPositionData[0]))){
						posArray = 0;
						for(int n=0; n<(sizeof(GPS::SavedPositionData)/sizeof(GPS::SavedPositionData[0])); n++){
							GPS::SavedPositionData[n].Filled = 0;
							GPS::posArray = 0;
						}
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
					//uart.printf("%d - %s\n",position,tmp);
					if(position == 2){
						//Current latitude
						string TempFormated;
						TempFormated = tmp;
						TempFormated.insert(2,".");
						TempFormated.erase(TempFormated.begin()+5);	//delete char on position
						//Delete last char BLE limitation
						TempFormated.erase(TempFormated.begin()+10);	//delete char on position
						//Find non-correct position data
						if((TempFormated.length()==9)||(TempFormated.length()==10)){
							strcpy(GPS::SavedPositionData[posArray].Latitude,TempFormated.c_str());
							GPS::SavedPositionData[posArray].Filled = 1;
						}
						else{
							GPS::SavedPositionData[posArray].Filled = 0;
						}
					}
					else if (position == 3) {
						//North/South orientation
						//Latitude
						strcpy(GPS::SavedPositionData[posArray].North_South,tmp);
					}
					else if (position == 4) {
						//Current longitude
						string TempFormated;
						TempFormated = tmp;
						TempFormated.erase(TempFormated.begin());	//delete char on position
						TempFormated.erase(TempFormated.begin()+4);	//delete char on position
						TempFormated.insert(2,".");
						//Delete last char BLE limitation
								TempFormated.erase(TempFormated.begin()+10);	//delete char on position
						if((TempFormated.length()==9)||(TempFormated.length()==10)){
							strcpy(GPS::SavedPositionData[posArray].Longitude,TempFormated.c_str());
							GPS::SavedPositionData[posArray].Filled = 1;
						}
						else{
							GPS::SavedPositionData[posArray].Filled = 0;
						}
					}
					else if(position == 5){
						//East/West orientation
						//Longitude
						strncpy(GPS::SavedPositionData[posArray].East_West,tmp,1);
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
}
void GPS::FakeDataGenerator(){
	//fill struct with fake data
	for(int n=0;n<=10;n++){
		int GeneratorLatitude;
		int GeneratorLongitude;
		char tmp[9];
		srand(time(NULL));

		GeneratorLatitude = rand() % 100 + 1;	//generate number between 1 to 100
		sprintf(tmp,"49.211%d",GeneratorLatitude);
		strcpy(SavedPositionData[n].Latitude, tmp);
		//


		GeneratorLongitude = rand() % 100 + 1;	//generate number between 1 to 100
		sprintf(tmp,"18.37227%d",GeneratorLongitude);
		strcpy(SavedPositionData[n].Longitude, tmp);
		//

	}
}
//@ When SavePosition is correc return 0
//@ When SavePosition is INcorrect return 1

int GPS::SavePosition_modif(){
	if(strcmp(SavePosOldData,GPS::readed_data) != 0){
			strcpy(SavePosOldData,GPS::readed_data);
			char Array[100];
			//DetectCommand with hastag
			//Detect data from GPS
			if (GPS::readed_data[0] == '$GPGLL') {
				//
				strcpy(Array,GPS::readed_data);
				//Separate char array by comma
				//Dolna Marikova Lat/Long ->
				//(49.211789, 18.351985)
				//49° 12' 42.4404'' N 18° 21' 7.146'' E

				char *p_CH;
				char tmp[20];
				p_CH = strtok(Array,",");
				//Find last filled struct in array //output is posArray which is not filled //If is full array point on the first mark all array like nonfilled
					bool stopWhile = 0;
					while((posArray <= (sizeof(GPS::SavedPositionData)/sizeof(GPS::SavedPositionData[0])))&&(stopWhile == 0)){
						if(GPS::SavedPositionData[posArray].Filled == 0){
							stopWhile = 1;
						}
						else if(posArray > (sizeof(GPS::SavedPositionData)/sizeof(GPS::SavedPositionData[0]))){
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
						//uart.printf("%d - %s\n",position,tmp);
						if(position == 2){
							//Current latitude
							strcpy(GPS::SavedPositionData[posArray].Latitude,tmp);
							GPS::SavedPositionData[posArray].Filled = 1;

						}
						else if (position == 3) {
							//North/South orientation
							//Latitude
							strcpy(GPS::SavedPositionData[posArray].North_South,tmp);

						}
						else if (position == 4) {
							//Current longitude
							strcpy(GPS::SavedPositionData[posArray].Longitude,tmp);

						}
						else if(position == 5){
							//East/West orientation
							//Longitude
							strncpy(GPS::SavedPositionData[posArray].East_West,tmp,1);

						}
						else{
						}
						//Last row of while;
						wait_us(10);
					}
					return 0;
			}
			else{
				return 1;
			}

		}
}

GPS::GPS() {
	// TODO Auto-generated constructor stub
	//GPS_data SavedPositionData[10];
}

GPS::~GPS() {
	// TODO Auto-generated destructor stub
}

;
