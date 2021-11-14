/*
 * Accelerometer.cpp
 *
 *  Created on: 13. 5. 2020
 *      Author: RPlsicik
 */

#include "Accelerometer.h"

#include "mbed.h"
#include "Shields.h"
#include "uart.h"
#include "UARTSerial.h"
#include <stdio.h>	/*printf; fgets */
#include <stdlib.h>	/* atol */
#include <wctype.h>
#include "Bluetooth.h"
#include "GPS_commands.h"
#include "UART_own.h"
#include "GPS.h"
#include "GSM.h"
//not tested
#include "GPS_module.h"
#include "GSM_commands.h"
#include "GSM_module.h"
#include "TestingFcn.h"

#include "i2c.h"

I2C i2c(GPIO3,GPIO2);
Ticker timer_continuously;
Ticker motionDetector;
NHD_C0216CZ lcd;

	/*
	//IRQ functions
	void Accelerometer::IRQ_timer_continuouslyReadAxisValue(){
		Accelerometer::read_axis();
	}
	*/
	void Accelerometer::accel_initialize(){
		char ByteArray[2];
		//turn on measurement
		ByteArray[0] =  POWER_CTL;
		ByteArray[1] = (1<<3);
		i2c.write(AccelAddres,ByteArray,2);
		//FIFO bypas
		ByteArray[0] = FIFO_CTL;
		ByteArray[1] = ~((1<<7)|(1<<6));
		i2c.write(AccelAddres,ByteArray,2);
		//Change sensitivity on 2g
		char TempArray[4];
		TempArray[0] = DATA_FORMAT;
		TempArray[1] = ~((1<<1)|(1<<0));	//D0 and D1
		i2c.write(AccelAddres,TempArray,2);
		accel_initStatus = true;
	}

	//@Continuously_read_axis
	//@	INPUT parameters
	//@ bool TurnOn - on/off sending axis;
	//@ int period of scanning axis [seconds]
	void Accelerometer::continuously_read_axis(bool TurnOn, float period){
		if(TurnOn == true){						//turn on continuously scan of axis
			//timer_continuously.attach(&IRQ_timer_continuouslyReadAxisValue,period);
		}
		else{									//turn off  continuously scan of axis
			timer_continuously.detach();
		}
	}

	void Accelerometer::GetNewData(int x,int y,int z){
		read_axis();
		wait_us(50);
		//show_axis(x,y,z);
		x = X_axis;
		y = Y_axis;
		z = Z_axis;
	}

	void Accelerometer::show_axis(int x,int y, int z){
		x = X_axis;
		y = Y_axis;
		z = Z_axis;
	}

	void Accelerometer::read_axis(){
		read_X();
		read_y();
		read_z();
	}
	void Accelerometer::read_X(){
		char output_RegistryValues[2];
		char TempArray[2];
		TempArray[0] =  DATA_X0;
		//i2c.write(AccelAddres,&TempArray[1],1);
		i2c.write(AccelAddres,&TempArray[0],1);
		i2c.read(AccelAddres,output_RegistryValues,2);
		//X_axis = output_RegistryValues[1];	//pretypovanie
		X_axis = output_RegistryValues[1];
		//X_axis =  atoi(output_RegistryValues);
		//char outputTxt[30];
		//sprintf(outputTxt,"x-%d",output_RegistryValues[1]);
		//sprintf(outputTxt,"x-%d",X_axis);
		//lcd.displayStringToPos(outputTxt,1,1);
		X_axis =  output_RegistryValues[1];
	}
	void Accelerometer::read_y(){
		char output_RegistryValues[2];
		char TempArray[2];
		TempArray[0] =  DATA_Y0;
		i2c.write(AccelAddres,&TempArray[0],1);
		i2c.read(AccelAddres,output_RegistryValues,2);
//DEFEAULT
		Y_axis =  output_RegistryValues[1];
//DEFEAULT		//Y_axis =  atoi(output_RegistryValues);
		//char outputTxt[30];
		//sprintf(outputTxt,"y-%d",output_RegistryValues[1]);
		//lcd.displayStringToPos(outputTxt,1,10);
	}
	void Accelerometer::read_z(){
		char output_RegistryValues[2];
		char TempArray[2];
		TempArray[0] =  DATA_Z0;
		i2c.write(AccelAddres,&TempArray[0],1);
		i2c.read(AccelAddres,output_RegistryValues,2);
		Z_axis =  output_RegistryValues[1];
		//Z_axis =  atoi(output_RegistryValues);
		//char outputTxt[30];
		//sprintf(outputTxt,"z-%d",output_RegistryValues[1]);
		//lcd.displayStringToPos(outputTxt,2,1);
	}

	void Accelerometer::devSetup(char inputCommand[10]){
		char TempArray[4];
			//First char of string must be specific
			if(inputCommand[0] == 'm'){
				//ble.send("HaloHalo",8);
				if (inputCommand == "m2g"){
					//Change sensitivity on 2g
					TempArray[0] = DATA_FORMAT;
					TempArray[1] = ~((1<<1)|(1<<0));	//D0 and D1
					i2c.write(AccelAddres,TempArray,2);
				}
				else if (inputCommand == "m4g"){
					//Change sensitivity on 4g
					TempArray[0] = DATA_FORMAT;
					TempArray[1] = ~(1<<1)|(1<<0);	//D0 and D1
					i2c.write(AccelAddres,TempArray,2);
				}
				else if (inputCommand == "m8g"){
					//Change sensitivity on 8g
					TempArray[0] = DATA_FORMAT;
					TempArray[1] = (1<<1)|~(1<<0);	//D0 and D1
					i2c.write(AccelAddres,TempArray,2);
				}
				else if (inputCommand == "m16g"){
					//Change sensitivity on 16g
					TempArray[0] = DATA_FORMAT;
					TempArray[1] = (1<<1)|(1<<0);	//D0 and D1
					i2c.write(AccelAddres,TempArray,2);
				}
				else if (inputCommand == "mFIFO"){
					//Change FIFO registry mode
					TempArray[0] = FIFO_CTL;
					TempArray[1] = ~((1<<7)|(1<<6));
					i2c.write(AccelAddres,TempArray,2);
				}
				else if (inputCommand == "mFIFOcollects"){
					//Change FIFO registry mode
					TempArray[0] = FIFO_CTL;
					TempArray[1] = ~(1<<7)|(1<<6);
					i2c.write(AccelAddres,TempArray,2);
				}
				else if (inputCommand == "mFIFOstream"){
					//Change FIFO registry mode
					TempArray[0] = FIFO_CTL;
					TempArray[1] = (1<<7)|~(1<<6);
					i2c.write(AccelAddres,TempArray,2);
				}
				else if (inputCommand == "mFIFOTrigger"){
					//Change FIFO registry mode
					TempArray[0] = FIFO_CTL;
					TempArray[1] = (1<<0)|(1<<1);
					i2c.write(AccelAddres,TempArray,2);
				}
				else if (strcmp(inputCommand,"m_init") == 0) {
					accel_initialize();
					TempArray[0] = FIFO_CTL;
					TempArray[1] = ~((1<<7)|(1<<6));
					i2c.write(AccelAddres,TempArray,2);
				}
				//Place for add next comands for controlling accelerometer
				else{}
			}
	}

/*
void Accelerometer::MotionDetection(bool On){
	//add ticket
	if(On = true){
		//attach
		motionDetector.attach(&IRQ_MotionDetector,0.3);
	}
	else{
		//detach
	}
}
*/


Accelerometer::Accelerometer() {
	// TODO Auto-generated constructor stub

}

Accelerometer::~Accelerometer() {
	// TODO Auto-generated destructor stub
}


