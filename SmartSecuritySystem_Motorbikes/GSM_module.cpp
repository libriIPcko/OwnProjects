/*
 * GSM_module.cpp
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
#include "GSM.h"

Ticker IRQ_GSM_Ticker;
/*
void Communication(char input[100],char output[100]){
	if(GSM_module.TurnOn == 0){
		GPS_module.deactivate();
		GSM_module.activate();
	}
}

*/
