/*
 * UART_own.h
 *
 *  Created on: 29. 4. 2020
 *      Author: RPlsicik
 */

#ifndef UART_OWN_H_
#define UART_OWN_H_

//Interrupt callback fcn
void IRQ_ReadUART();
void IRQ_GSM_ReadUART();
void IRQ_ReadUART_wait();



//Basic Fcn
int Send_UART(char inputArray[]);
void Send_UART(char inputArray[],char OLD_inputArray[]);
int Send_UART_nonPrintableChar(int inputArray[],int SizeArray);
void Reading_UART(char output[100]);
void Read_UART();
void Read_UART_Wait(int WaitOnSignal_ms);
void Read_UART_scanf();
void Read_GSM_UART_Wait(float WaitOnSignal_ms);
void Read_GSM_UART_Wait(int WaitOnSignal_ms,char outputData[50]);
bool uart_IsReadable();

void Take_dataUART(char GlobalVarOutput[]);			//like C++ method for acces to private variable
void Take_commandUART(char GlobalVarOutput[]);		//like C++ method for acces to private variable

//Intermediate Fcn

//int Save_UART_command(char inputArray[],char outputArray[]);
int Save_UART_command(char outputArray[]);
//void ShowDecodedData(bool target,int PosStruct);

//void DecodeReadedData(char inputArrayOfChar[]);
//void DecodeReadedData();
//void Decode_NMEA_code(char inputArray[100],string outputData[30]);
//void GPS_control(int function, bool On_Off);
//int Dissassemly_array(char inputArray[100],char outputArray[100],int position); //Output is dis-assembled string from input string

//Testing Fcn
void Send_HexUART();
void ShowStruct(int PosStruct);

#endif /* UART_OWN_H_ */
