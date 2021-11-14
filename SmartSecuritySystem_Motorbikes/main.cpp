//@ For downgrade general code to 5_SmartSystem_v008 !!!!
//@ For downgrade GPS code to 5_SmartSystem_v0071_GPSmodif !!!!

#include "mbed.h"
#include "Shields.h"
#include "uart.h"
#include "UARTSerial.h"
#include <stdio.h>
#include "PwmOut.h"
#include <wctype.h>

#include "Bluetooth.h"
#include "GPS_commands.h"
//not tested
#include "GPS_module.h"
#include "GSM_commands.h"
#include "GSM_module.h"
#include "TestingFcn.h"

#include "UART_own.h"

#include "GPS.h"
#include "GSM.h"
#include "Accelerometer.h"

#include "array"


#define TITLE   "BLE"

bool RepeatAlgorithm = false;
bool MainAlgorithm = false;
bool Algorithm_Call = false;
bool Algorithm_Alarm = false;
bool Algorithm_SendPosition = false;

//Objects
NHD_C0216CZ lcd;
GPS GPS_module;
GSM GSM_module;

InterruptIn	StatusSecurity(GPIO17);

Accelerometer accel;
Ticker cycleBLEsendData;
void IRQ_BLEmenu_sendAccel();

void UartBUS_init(){
	GSM_module.deactivate();
	GPS_module.deactivate();
	//Trun off ECHO mode of GSM
	GSM_module.send("ATE0\n");
}

bool DevelopMode = true;
char GLOABL_dataUART[1200];
char GLOBAL_commandUART[1200];
char GLOBAL_oldUART[100];

string GLOBAL_BLE_RECEIVED_DATA_s;
char GLOBAL_BLE_RECEIVED_DATA[100];
char GLOBAL_BLE_RECEIVED_DATA_OLD[100];

int Ble_Menu(char Command[50]);
struct SepCommand{
	char wordArray[30];
}SeparatedCommand[20];

Ticker watchdog;
bool toogleVar_IRQ1 = 0;
int watchdog_counter;
void IRQ_Watchdoq_ContinuoslyRun();

DigitalInOut StatusOfLock(GPIO17);
bool LockStatus_Toogle = false;
bool SecurityStatus;
bool Robbery;
//DigitalInOut Alarm(GPIO15);
DigitalInOut Alarm(GPIO7);

Ticker motionDetector;
int OldAxisValues[4];
bool StatusOfMotion;
int counter_IRQ;
int CountOfMotion;
int CountOfNonMotion;
bool FirstRun = true;
void MotionDetection(bool On);
void IRQ_MotionDetector();

Ticker Bluetooth;
void IRQ_Bluetooth_Communication();

Ticker RingDetector;
bool RingStatus = false;
void IRQ_RING_detection();

Ticker ReadFromUart;
char txt_data[100];
void IRQ_ReadFromBus();

Ticker ReadFromGPS;
void IRQ_TakePosition();
/*
Ticker WaitOnCall;
bool EndAlgorithm = false;
void IRQ_WaitOnCall();
*/
Ticker LockStatus;

/*
Ticker SecCounter;
bool toogleVar_SecCounter = false;
int  Counter_SecCounter = 0;
void IRQ_SecCounter_();
*/

//Serial pc(USBTX,USBRX);
/*---------------------------------------------------MAIN---------------------------------------------------------*/
int main(){
	//Initialization part
	lcd.init();
	lcd.displayString(TITLE);
	wait(2);
	lcd.clearDisplay();
  //Initialize for Startup
	UartBUS_init();
	accel.accel_initialize();
	FirstRun = true;			//Variable immediately after initialize
	//Core of testing CODE
		watchdog.attach(&IRQ_Watchdoq_ContinuoslyRun,0.4);	//Služi len ako sledovanie aktivity uP
		Bluetooth.attach(&IRQ_Bluetooth_Communication,0.7);
    while(1){
    if((Algorithm_Alarm == true)||(Algorithm_Call == true)||(Algorithm_SendPosition==true)){
    	wait_ms(500);
    	ReadFromGPS.attach(IRQ_TakePosition,5);
		if(Algorithm_Alarm == true){
			if(Robbery == true){
				//char tmp[30];
				Alarm.output();
				Alarm.write(1);
			}
			else{
				Alarm.output();
				Alarm.write(0);
			}
		}
		else if((Algorithm_Call == true)&&(RepeatAlgorithm == false)){
			if(Robbery == true){
				//char tmp[30];
				RepeatAlgorithm = true;
				GSM_module.activate();
				wait_ms(500);
				GSM_module.Call("+421944542714");
				GSM_module.deactivate();
			}
		}
		else if((Algorithm_SendPosition == true)&&(RepeatAlgorithm == false)){
			if(Robbery == true){
				char tmp[30];
				RepeatAlgorithm = true;
				GSM_module.activate();
				wait_ms(500);
				//GPS_module.ShowDecodedData(21,GPS_module.posArray);
				//sprintf(tmp,"N:%s E:%s",GPS_module.ShowDecodedData_latitude,GPS_module.ShowDecodedData_longitude);
				sprintf(tmp,"N:49.211789 E:18.351985");
				GSM_module.WriteSMS("+421944542714",tmp);
				GSM_module.deactivate();
			}
		}
    }
    }
}
/*--------------------------------------------------------------------------------------------------------------*/
																				//Declarations fcn

														//Interupt functions declaration
void IRQ_Watchdoq_ContinuoslyRun(){
	LED testLED;
	watchdog_counter++;
	if(toogleVar_IRQ1 == true){
		toogleVar_IRQ1 = false;
		//lcd.clearDisplay();
		//lcd.displayStringToPos("!",2,15);
		testLED.ledOn(IDKLED1);
		testLED.ledOff(IDKLED2);
	}
	else{
		toogleVar_IRQ1 = true;
		//lcd.displayStringToPos(" ",2,15);
		testLED.ledOff(IDKLED1);
		testLED.ledOn(IDKLED2);
	}
	//char tmp[20];
//Test  status of lock
	//LockStatus_Toogle = false;
	IRQ_MotionDetector();
	if((Algorithm_Alarm != false)||(Algorithm_Call != false)||(Algorithm_SendPosition != false)){
		StatusOfLock.input();
		if((StatusOfLock.read()==0)){	//
			//LOCK
			char temp[20];
			//motionDetector.attach(&IRQ_MotionDetector,0.5);
			sprintf(temp,"           ");
			lcd.displayStringToPos(temp,2,1);
			sprintf(temp,"Locked    <%d>",StatusOfMotion);
			SecurityStatus = true;
			lcd.displayStringToPos(temp,2,1);
			if((StatusOfMotion == 1)){
				if(FirstRun == false){
					Robbery = true;
				}
				FirstRun = false;
			}
			else{
			}
		}
		else{
			//UNLOCK
			FirstRun = true;
			char temp[20];
			sprintf(temp,"           ");
			lcd.displayStringToPos(temp,2,1);
			sprintf(temp,"Unlocked    <%d>",StatusOfMotion);
			motionDetector.detach();
			SecurityStatus = false;
			lcd.displayStringToPos(temp,2,1);
			Robbery = false;
			//Turn off alarm
			//Alarm.output();
			//Alarm.write(0);
			RepeatAlgorithm = false;
		}
	}
}

//@	ADDED BLE extension
void IRQ_TakePosition(){
	ReadFromGPS.detach();
	//GPS_module.only_read();
	GPS_module.waitTo_read();
	GPS_module.SavePosition();
	GPS_module.ShowDecodedData(2,0);
	GPS_module.ShowDecodedData(21,GPS_module.posArray);
	char tmp[10];
	char tmpText[21];
	//pc.printf("\t\t\t EPOCH \n");
	//pc.printf("%s\n",GPS_module.readed_data);
	//pc.printf("\t<%s><%s>\tActivePos->%d/%d",GPS_module.ShowDecodedData_latitude,GPS_module.ShowDecodedData_longitude,GPS_module.posArray,GPS_module.posArray_AbsoluteLenght);
	sprintf(tmp,"%s",GPS_module.ShowDecodedData_latitude);
	lcd.displayStringToPos(tmp,1,1);
	sprintf(tmp,"%s",GPS_module.ShowDecodedData_longitude);
	lcd.displayStringToPos(tmp,2,1);
	//pc.printf("\n");
	wait_ms(100);
	//last added rows START
	string Latitude_string = GPS_module.ShowDecodedData_latitude;
	string Longitude_string = GPS_module.ShowDecodedData_longitude;
	if((Latitude_string.empty()==0)&&(Longitude_string.empty()==0)){
		//sprintf(tmpText,"%s %s",GPS_module.ShowDecodedData_latitude,GPS_module.ShowDecodedData_longitude);
		sprintf(tmpText,"%s %s",Latitude_string,Longitude_string);
		//BLE_SendChar(tmpText);
		BLE_SendData(tmpText);
	}
	//Three row in branch are cut from behind rows
	//last added rows END
	ReadFromGPS.attach(IRQ_TakePosition,1);
}

void IRQ_Bluetooth_Communication(){
	if(DevelopMode == true){
		int newData = 0;
		newData = BLE_ReadReceivedData();
		if(newData == 1){
			//char tmp[50];
			//GLOBAL_BLE_RECEIVED_DATA_s = GLOBAL_BLE_RECEIVED_DATA;
			//lcd.displayString(GLOBAL_BLE_RECEIVED_DATA);
			//BLE_SendChar(GLOBAL_BLE_RECEIVED_DATA);
			BLE_SendData(GLOBAL_BLE_RECEIVED_DATA);
			Ble_Menu(GLOBAL_BLE_RECEIVED_DATA);
			memset(GLOBAL_BLE_RECEIVED_DATA,' ',sizeof(GLOBAL_BLE_RECEIVED_DATA));
		}
		else{
		}
	}
	else{
	}
}

void IRQ_MotionDetector(){
	//motionDetector.detach();
	counter_IRQ++;
	if(counter_IRQ > 10){
		counter_IRQ=0;
		CountOfMotion = 0;
		CountOfNonMotion = 0;
	}
	if(accel.accel_initStatus != 1){
		char command[7];
		sprintf(command,"m_init");
		accel.devSetup(command);
	}
	bool equalAllAxis[3] = {false,false,false};
	int FreshData[3];
	accel.read_axis();
	FreshData[0] = accel.X_axis;
	FreshData[1] = accel.Y_axis;
	FreshData[2] = accel.Z_axis;
	//char temporary[60];
	//Comparating old data vs Fresh data
	for(int n=0;n<=2;n++){
		//if((((FreshData[n])-10) < OldAxisValues[n] ) < ((FreshData[n])+10)){
		//if((FreshData[n]-10 < OldAxisValues[n])||(FreshData[n]+10 > OldAxisValues[n] )){
		if((FreshData[n]-10 <= OldAxisValues[n])&&(OldAxisValues[n] <= FreshData[n]+10) ){
			equalAllAxis[n] = true;
		}
		else{
			equalAllAxis[n] = false;
		}
	}

	//Ak sa nerovnaju tak vpiš Fresh data do starych dat
	if((equalAllAxis[0] == false)||(equalAllAxis[1] == false)||(equalAllAxis[2] == false)){
		for(int n=0;n<=2;n++){
			OldAxisValues[n] = FreshData[n];
			//sprintf(temporary,"Copy: <%d>-<%d>\n",OldAxisValues[n],FreshData[n]);
			//GSM_module.send(temporary);
		}
		//StatusOfMotion = true;
		CountOfMotion++;
	}
	else{
	 //StatusOfMotion = false;
	 CountOfNonMotion++;
	}

	if((CountOfMotion > ((CountOfNonMotion)/2))){
		StatusOfMotion = true;
	}
	else{
		StatusOfMotion = false;

	}


	//Feedback
	//char tmpoutput[30];

	/*
	sprintf(tmpoutput,"......NEW EPOCH.........\n");
	pc.printf("%s\n",tmpoutput);
	sprintf(tmpoutput,"-<%d> -<%d> -<%d>",FreshData[0]-10,FreshData[1]-10,FreshData[2]-10);
	pc.printf("%s\n",tmpoutput);
	sprintf(tmpoutput,"<%d><%d><%d>",OldAxisValues[0],OldAxisValues[1],OldAxisValues[2]);
	pc.printf("OLD DATA:\t|%s|\n",tmpoutput);
	sprintf(tmpoutput,"<%d><%d><%d>",FreshData[0],FreshData[1],FreshData[2]);
	pc.printf("FRESH DATA: |%s|\n",tmpoutput);
	sprintf(tmpoutput,"M<%d>nM<%d>%d %d",CountOfMotion,CountOfNonMotion,counter_IRQ,StatusOfMotion);
	lcd.displayString(tmpoutput);
	pc.printf("%s\n",tmpoutput);
	sprintf(tmpoutput,"+<%d> +<%d> +<%d>",FreshData[0]+10,FreshData[1]+10,FreshData[2]+10);
	pc.printf("%s\n",tmpoutput);
	*/
	//motionDetector.attach(&IRQ_MotionDetector,0.6);
}

/*
//urèený pre dodatoèny vyvoj GSM read commands
void IRQ_SecCounter_(){
	char tempArray[20];
	if(toogleVar_SecCounter == true){
		toogleVar_SecCounter = false;

	}
	else{
		toogleVar_SecCounter = true;

	}
}
*/
void IRQ_RING_detection(){
	ReadFromUart.detach();
	char tmp[20];

	sprintf(tmp,"test ringing");
	lcd.clearDisplay();
	lcd.displayStringToPos(tmp,1,1);
	int status;
	GSM_module.IsRinging(status);
	if(status == 0){
		RingStatus = true;
		sprintf(tmp,"RING");
		lcd.displayStringToPos(tmp,2,1);
	}
	else{
		RingStatus = false;
		lcd.clearDisplay();
	}
	sprintf(tmp,"-> %d",RingStatus);
	lcd.displayStringToPos(tmp,1,1);
	wait_ms(500);
	ReadFromUart.attach(&IRQ_ReadFromBus,3);
}


void IRQ_ReadFromBus(){
	ReadFromUart.detach();
	char tmp[50];
	Read_GSM_UART_Wait(2000,tmp);
	lcd.displayStringToPos(tmp,2,1);
	ReadFromUart.attach(&IRQ_ReadFromBus,3);
}
/*
void IRQ_WaitOnCall(){
	WaitOnCall.detach();

	char tmp[50];
	Read_GSM_UART_Wait(2000,tmp);
	if(strcmp(tmp,"RING") == 0){
		lcd.displayStringToPos(tmp,2,1);
		GSM_module.AnswerTheCall(tmp);
		lcd.clearDisplay();
		lcd.displayStringToPos(tmp,1,1);

		wait_ms(1000);
		EndAlgorithm = true;
		WaitOnCall.attach(&IRQ_WaitOnCall,3);
	}
	else{
		//Miesto pre attach ak nepride RING
		//Ak pride RING Interrupt bude attachnuty až na konci sluèky
		wait_ms(10);
		WaitOnCall.attach(&IRQ_WaitOnCall,3);
	}

}
*/

/*
void IRQ_TestAlgorithm(){
	char tmp[50];
	Counter_TestAlgorithm++;
	if(Counter_TestAlgorithm > 5){
		if(toogleVar_TestAlgorithm == 1){
		}
		else{
		}
		Counter_TestAlgorithm = 0;
	}
}
*/
void IRQ_BLEmenu_sendAccel(){
	int X;
	int Y;
	int Z;
	//accel.GetNewData(X,Y,Z);
	accel.read_axis();
	X = accel.X_axis;
	Y = accel.Y_axis;
	Z = accel.Z_axis;
	char tmpFeedback[50];
	//sprintf(tmpText,"x-%d,y-%d||,z-%d",X,Y,Z);
	//GSM_module.send(tmpFeedback);
	sprintf(tmpFeedback,"x-%d,y-%d||,z-%d",X,Y,Z);
	lcd.displayString(tmpFeedback);
	//GPS_module.send(tmpFeedback);	//Feedback row
	//BLE_SendChar(tmpText);
	//BLE_SendData(tmpText);
}


int Ble_Menu(char Command[50]){
 Bluetooth.detach();
	char *pCH;
	int step = 0;
	char tmpFeedback[50];
	lcd.displayString(Command);
	pCH = strtok(Command,",");
	while((pCH != NULL)&&(step < 10)){
		strcpy(SeparatedCommand[step].wordArray,pCH);
		pCH = strtok(NULL,",");
		step++;
	}
/*
	for(int n=0;n<3;n++){
		char tmp[20];
		sprintf(tmp,"-> [%d] - %s\n",SeparatedCommand[n].wordArray);
		GSM_module.send(SeparatedCommand[n].wordArray);
	}
*/
//GPS menu
	if(strcmp(SeparatedCommand[0].wordArray,"#GPS") == 0){
		GPS_module.activate();
		GSM_module.deactivate();
		wait_ms(1);
	  //Position
		if(strcmp(SeparatedCommand[1].wordArray,"posOn") == 0){
			GPS_module.control(2,true);
			Bluetooth.attach(&IRQ_Bluetooth_Communication,0.7);
			//return 0;
		}
		else if(strcmp(SeparatedCommand[1].wordArray,"posOff") == 0){
			GPS_module.control(2,false);
			Bluetooth.attach(&IRQ_Bluetooth_Communication,0.7);
			//return 0;
		}
	  //Time
		else if(strcmp(SeparatedCommand[1].wordArray,"timeOn") == 0){
			GPS_module.control(1,true);
			Bluetooth.attach(&IRQ_Bluetooth_Communication,0.7);
			//return 0;
		}
		else if(strcmp(SeparatedCommand[1].wordArray,"timeOff") == 0){
			GPS_module.control(1,false);
			Bluetooth.attach(&IRQ_Bluetooth_Communication,0.7);
			//return 0;
		}
	  //AllComands
		/*else if(strcmp(SeparatedCommand[1].wordArray,"allOn") == 0){
			GPS_module.control(99,true);
			Bluetooth.attach(&IRQ_Bluetooth_Communication,0.7);
			return 0;
		}*/
		else if(strcmp(SeparatedCommand[1].wordArray,"allOff") == 0){
			GPS_module.control(99,false);
			Bluetooth.attach(&IRQ_Bluetooth_Communication,0.7);
			//return 0;
		}
		else if(strcmp(SeparatedCommand[1].wordArray,"SFP") == 0){
			char Latitude[9];
			char Longitude[9];
			char FinalTxT[20];
		  //GPS_module.FakeDataGenerator();
		  //Step-by-Step send saved position
			/*
			for(int pos=0;pos<=10;pos++){
				//GPS_module.ShowStruct(pos,Latitude,Longitude);
				sprintf(FinalTxT,"%s %s",49.211789,18.351985);
				lcd.displayString(FinalTxT);
				//BLE_SendChar(FinalTxT);
				BLE_SendData(FinalTxT);
				wait_ms(500);
			}
			*/
			//wait_ms(20);
			sprintf(FinalTxT,"49.211789 18.351985");
			//lcd.displayString(FinalTxT);
			//BLE_SendChar(FinalTxT);
			BLE_SendData(FinalTxT);
			Bluetooth.attach(&IRQ_Bluetooth_Communication,0.7);
			//return 0;
		}
		else if(strcmp(SeparatedCommand[1].wordArray,"DownPos") == 0){
			//Operation with position interrupt
			if(strcmp(SeparatedCommand[2].wordArray,"On") == 0){
				ReadFromGPS.attach(&IRQ_TakePosition,1);
			}
			else{
				ReadFromGPS.detach();
			}
		}
		else{
			GPS_module.send("NOP\n");
			Bluetooth.attach(&IRQ_Bluetooth_Communication,0.7);
			//return 0;
		}
		Bluetooth.attach(&IRQ_Bluetooth_Communication,0.7);
		//GPS_module.deactivate();
		return 0;
	}
//GSM menu
	else if (strcmp(SeparatedCommand[0].wordArray,"#GSM") == 0) {
		GSM_module.activate();
		GPS_module.deactivate();
		wait_ms(1);
		if(strcmp(SeparatedCommand[1].wordArray,"Call") == 0){
			if(strcmp(SeparatedCommand[2].wordArray,"defeault") == 0){
				Bluetooth.attach(&IRQ_Bluetooth_Communication,0.7);
				sprintf(tmpFeedback,"ATD+421944542714;\r");
				//lcd.displayString(tmpFeedback);
				GSM_module.send(tmpFeedback);
				//return 0;
			}
			else{
				//GSM::CallwithEnd(char mobileNumber[13], int TimeOfCalling_s, char TypeOfOutput[20])
				/*sprintf(tmpFeedback,"ATD+421%s;\r\n",SeparatedCommand[3].wordArray);
				//lcd.displayString(tmpFeedback);
				GSM_module.send(tmpFeedback);*/
				//ReCall
				Bluetooth.attach(&IRQ_Bluetooth_Communication,0.7);
				char feedback[15];
				char number[15];
				sprintf(number,"%s",SeparatedCommand[3]);
				GSM_module.CallwithEnd(number,80,feedback);
				//lcd.displayStringToPos(feedback,2,1);
				//return 0;
			}
		}
		else if(strcmp(SeparatedCommand[1].wordArray,"sms") == 0){
			char txtSMS[64];
			//string txtsms;
			//txtsms = SeparatedCommand[2].wordArray;
			sprintf(txtSMS,"%s\n",SeparatedCommand[2].wordArray);
			GSM_module.WriteSMS("+421944542714",txtSMS);
			//GSM_module.WriteSMS("+421944542714","TestTxT\n.");
		}
		else if(strcmp(SeparatedCommand[1].wordArray,"AT") == 0){
			Bluetooth.attach(&IRQ_Bluetooth_Communication,0.7);
			char tmp[13];
			GSM_module.TestAT(tmp);
			BLE_SendChar(tmp);
			lcd.displayStringToPos(tmp,1,1);
			//return 0;
		}
		else if(strcmp(SeparatedCommand[1].wordArray,"BatteryStatus") == 0){
			Bluetooth.attach(&IRQ_Bluetooth_Communication,0.7);
			//AT+CSQ
			char outputTXT[100];
			sprintf(tmpFeedback,"AT+CBC\r");
			GSM_module.Communication(tmpFeedback,outputTXT, 1500);
			//Read from uart finding tokens
			//return 0;
		}
		Bluetooth.attach(&IRQ_Bluetooth_Communication,0.7);
		GSM_module.deactivate();
		return 0;
	}


//Accelerometer menu
	else if (strcmp(SeparatedCommand[0].wordArray,"#Accel") == 0) {
		sprintf(tmpFeedback,"Accelerometer MENU");
		GPS_module.send(tmpFeedback);
		GSM_module.send(tmpFeedback);
		if(strcmp(SeparatedCommand[1].wordArray,"On") == 0){
			Bluetooth.attach(&IRQ_Bluetooth_Communication,0.7);
			//char tmp[20];
			//sprintf(tmp,"Accel on OK!");
			//lcd.displayString(tmp);
			//cycleBLEsendData.attach(&IRQ_BLEmenu_sendAccel,0.8);
			cycleBLEsendData.attach(&IRQ_BLEmenu_sendAccel,0.5);
			return 0;
		}
		else if(strcmp(SeparatedCommand[1].wordArray,"Off") == 0){
			Bluetooth.attach(&IRQ_Bluetooth_Communication,0.7);
			//Accel turn off read data
			lcd.clearDisplay();
			cycleBLEsendData.detach();
			return 0;
		}
		else if(strcmp(SeparatedCommand[1].wordArray,"axis") == 0){
			Bluetooth.attach(&IRQ_Bluetooth_Communication,0.7);
			int x,y,z;
			char tmpTxT[20];
			accel.GetNewData(x,y,z);
			sprintf(tmpTxT,"x -> %d",x);
			BLE_SendChar(tmpTxT);
			return 0;
		}
		else if (SeparatedCommand[1].wordArray[1] == 'm') {				//branch for command of old structure
			accel.devSetup(SeparatedCommand[1].wordArray);
			Bluetooth.attach(&IRQ_Bluetooth_Communication,0.7);
			return 0;
		}
		else if((strcmp(SeparatedCommand[1].wordArray,"DetOn") == 0)){
			Bluetooth.attach(&IRQ_Bluetooth_Communication,0.7);
			lcd.displayString("DETECTOR");
			motionDetector.attach(&IRQ_MotionDetector,0.6);
		}
		else if((strcmp(SeparatedCommand[1].wordArray,"DetOff") == 0)){
			Bluetooth.attach(&IRQ_Bluetooth_Communication,0.7);
			motionDetector.detach();
		}
		else{
			sprintf(tmpFeedback,"---NOP---\n");
			GPS_module.send(tmpFeedback);
			Bluetooth.attach(&IRQ_Bluetooth_Communication,0.7);
			return 1;
		}
	}
//Other Commands
	else if (strcmp(SeparatedCommand[0].wordArray,"##") == 0) {
		if(strcmp(SeparatedCommand[1].wordArray,"GSMactivate") == 0){
			GPS_module.deactivate();
			wait_us(5);
			GSM_module.activate();
			char txt[20];
			sprintf(txt,"GPS_ON");
			BLE_SendChar(txt);
			Bluetooth.attach(&IRQ_Bluetooth_Communication,0.7);
			return 1;
		}
		else if(strcmp(SeparatedCommand[1].wordArray,"GPSactivate") == 0){
			char txt[20];
			GSM_module.deactivate();
			wait_us(5);
			GPS_module.activate();
			sprintf(txt,"GSM_ON");
			BLE_SendChar(txt);
			Bluetooth.attach(&IRQ_Bluetooth_Communication,0.7);
			return 1;
		}
		else if(strcmp(SeparatedCommand[1].wordArray,"AllOff") == 0){
			char txt[20];
			GSM_module.deactivate();
			wait_us(5);
			GPS_module.deactivate();
			sprintf(txt,"All deactivate");
			BLE_SendChar(txt);
			Bluetooth.attach(&IRQ_Bluetooth_Communication,0.7);
			return 1;
		}
		else if(strcmp(SeparatedCommand[1].wordArray,"FreeRun") == 0){
			//char txt[20];
			 MainAlgorithm = false;
			 Algorithm_Call = false;
			 Algorithm_Alarm = false;
			 Algorithm_SendPosition = false;
			 FirstRun = true;
			Bluetooth.attach(&IRQ_Bluetooth_Communication,0.7);
			return 1;
		}
		else if(strcmp(SeparatedCommand[1].wordArray,"Call") == 0){
			//char txt[20];
			 MainAlgorithm = false;
			 Algorithm_Call = true;
			 Algorithm_Alarm = false;
			 Algorithm_SendPosition = false;
			Bluetooth.attach(&IRQ_Bluetooth_Communication,0.7);
			return 1;
		}
		else if(strcmp(SeparatedCommand[1].wordArray,"Alarm") == 0){
			//char txt[20];
			MainAlgorithm = false;
			Algorithm_Call = false;
			Algorithm_Alarm = true;
			Algorithm_SendPosition = false;
			Bluetooth.attach(&IRQ_Bluetooth_Communication,0.7);
			return 1;
		}
		else if(strcmp(SeparatedCommand[1].wordArray,"PosSend") == 0){
			//char txt[20];
			MainAlgorithm = false;
			Algorithm_Call = false;
			Algorithm_Alarm = false;
			Algorithm_SendPosition = true;
			Bluetooth.attach(&IRQ_Bluetooth_Communication,0.7);
			return 1;
		}
		else{
			sprintf(tmpFeedback,"---NOP---\n");
			GPS_module.send(tmpFeedback);
			Bluetooth.attach(&IRQ_Bluetooth_Communication,0.7);
			return 1;
		}
	}
	else{
		BLE_SendData(GLOBAL_BLE_RECEIVED_DATA_s);
	}
 Bluetooth.attach(&IRQ_Bluetooth_Communication,0.7);
 return 0;
}
