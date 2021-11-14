/*
 * TestingFCN.cpp
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
/*
//Ticker UART_tic;
//Serial uart(GPIO0,GPIO1,9600);
NHD_C0216CZ lcd;

Ticker LockStatus;
DigitalInOut StatusOfLock(GPIO17);
bool LockStatus_Toogle = false;
//bool SecurityStatus;





void IRQ_LockStatus(){
	//if(LockStatus_Toogle == true){
		LockStatus_Toogle = false;
		StatusOfLock.input();
		if(StatusOfLock.read()==0){
			//LOCK
			char temp[20];
			lcd.clearDisplay();
			sprintf(temp,"Locked");
			SecurityStatus = true;
			lcd.displayStringToPos(temp,2,1);

		}
		else{
			//UNLOCK
			char temp[20];
			lcd.clearDisplay();
			sprintf(temp,"Unlocked");
			SecurityStatus = false;
			lcd.displayStringToPos(temp,2,1);
		//}
	}
}


//StatusOfLock.mode(PullNone);
*/

