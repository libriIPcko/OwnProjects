/*
 * GPS.h
 *
 *  Created on: 1. 5. 2020
 *      Author: RPlsicik
 */

#ifndef GPS_H_
#define GPS_H_

using namespace std;

class GPS {

public:
	bool TurnOn;
	char OLD_send_array[100];
	char readed_data[100];

	char ShowInputdData_OLD[100];
	char ShowDecodedData_outputVar[30];
	char ShowDecodedData_longitude[30];
	//string ShowDecodedData_longitude;
	char ShowDecodedData_latitude[30];
	//string ShowDecodedData_latitude;

	//Global Variable for save position
	int posArray = 0;	//global variable -> active position in array used in DecodeReadedData
	int posArray_AbsoluteLenght;
	char SavePosOldData[100];


	struct GPS_data {
		bool  Filled;
		char Latitude[20];
		char  North_South[20];
		char Longitude[20];
		char  East_West[20];
	}SavedPositionData[10];

//methods
	//Commands for control module
	void activate();
	void deactivate();
	void control(int function,bool On_Off);
	//R&W commands
	void send(char inputArray[100]);
	void only_read();
	void waitTo_read();
	void only_read_waitOnInput(float waitTime_uS);


	//Save & Read struct data
	//void save_position();
	//void load_position();


	void take_position();

	//Graphical & Data Output
	void ShowInputdData(bool target);
	void ShowDecodedData(int target,int PosStruct);
	//void ShowStruct(int PosStruct);					//Basic FCN show position on LCD
	void ShowStruct(int PosStruct,char Latitude[9],char Longitude[9]);			//Final Function
	void SavePosition();
	int LastPosOfStruct();
	int SavePosition_modif();
	void FakeDataGenerator();


	//Constructor & Destructor
	GPS();
	virtual ~GPS();



};

#endif /* GPS_H_ */
