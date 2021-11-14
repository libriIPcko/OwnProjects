/*
 * Bluetooth.cpp
 *
 *  Created on: 10. 5. 2020
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
#include "main.cpp"


RSL10_BLE ble;

//#define LCD_MSG_LENGTH                 ((size_t)10)
#define LCD_MSG_LENGTH                 ((size_t)20)
char message[RSL10_MAX_MSG_LENGTH + 1];
int msg_len = 0;
unsigned uptime = 0;
int BLE_SendData(string message){
	// Send 10 character long periodic message with changing counter.
	msg_len = ble.send(message.c_str(), message.length());
	//Core Row for sending data
	if (msg_len == LCD_MSG_LENGTH){
		return 0;	//Correct sent data
	}
	else{
		return 1;  //ERROR
	}
}

void BLE_SendChar(char CharArray[20]){
	// Send 10 character long periodic message with changing counter.
	//msg_len = ble.send(CharArray, sizeof(CharArray));
	msg_len = ble.send(CharArray, (sizeof(CharArray)/sizeof(CharArray[0])));
	memset(CharArray,' ',(sizeof(CharArray)/sizeof(CharArray[0])));

}


string BLE_ReceiveData(){
	if (ble.readable()){
				// Receive message of UP TO 20 characters long.
				msg_len = ble.recv(message, RSL10_MAX_MSG_LENGTH); 					//Core Row for receive data
					//message[msg_len] = '\0';
					// Display first 10 characters on LCD display.
					//message[LCD_MSG_LENGTH] = '\0';
	}
	return message;
}
//@ output message is message
/*
int BLE_ReadReceivedData(){
	if (ble.readable()){
				// Receive message of UP TO 20 characters long.
				msg_len = ble.recv(message, RSL10_MAX_MSG_LENGTH); 					//Core Row for receive data
				strcpy(GLOBAL_BLE_RECEIVED_DATA,message);
				//message[msg_len] = '\0';
				// Display first 10 characters on LCD display.
				//message[LCD_MSG_LENGTH] = '\0';
	}
	return msg_len;
}
*/

int BLE_ReadReceivedData(){
	if (ble.readable()){
				// Receive message of UP TO 20 characters long.
				memset(message,' ',20);
				msg_len = ble.recv(message, RSL10_MAX_MSG_LENGTH); 					//Core Row for receive data

				strcpy(GLOBAL_BLE_RECEIVED_DATA,message);
				//message[msg_len] = '\0';
				// Display first 10 characters on LCD display.
				//message[LCD_MSG_LENGTH] = '\0';
				return 1;
	}
	else{
		return 0;
	}

}
