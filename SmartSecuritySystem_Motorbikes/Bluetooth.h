/*
 * Bluetooth.h
 *
 *  Created on: 10. 5. 2020
 *      Author: RPlsicik
 */

#ifndef BLUETOOTH_H_
#define BLUETOOTH_H_

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

//Library for operation with string
#include <string.h>
#include <stdio.h>

int BLE_SendData(string message);
void BLE_SendChar(char CharArray[]);
string BLE_ReceiveData();
//int BLE_ReadReceivedData();
int BLE_ReadReceivedData();

string convertToString(char* a,int size);

#endif /* BLUETOOTH_H_ */
