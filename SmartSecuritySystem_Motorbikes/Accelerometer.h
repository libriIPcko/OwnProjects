/*
 * Accelerometer.h
 *
 *  Created on: 13. 5. 2020
 *      Author: RPlsicik
 */

#ifndef ACCELEROMETER_H_
#define ACCELEROMETER_H_

//Registry maps of accelerometer
//#define DEVID		0x00	//DeviceID
#define THRESH_TAP	0x1D	//Tap threshold
#define OFS_X		0x1E	//X-axis offset
#define OFS_Y		0x1F	//Y-axis offset
#define OFS_Z		0x20	//Z-axis offset
#define DUR			0x21	//Tap duration
#define Latent		0x22	//Tap latency
#define Window		0x23	//Tap window
#define THRESH_ACT		0x24	//Activity threshold
#define THRESH_INACT 	0x25	//Inactivity threshold
#define TIME_INACT		0x26	//Inactivity time
#define ACT_INACT_CTL 	0x27 	//Axis enable control for activity and inactivity detection
#define THRESH_FF		0x28	//Free-fall threshold
#define TIME_FF			0x29	//Free-fall time
#define TAP_AXES		0x2A	//Axis control for single tap/double tap
#define ACT_TAP_STATUS	0x2B	//Source of single tap/double tap
#define BW_RATE			0x2C	//Data rate and power mode control
#define POWER_CTL		0x2D	//Power-saving features control
//#define INT_ENABLE		0x2E	//Interrupt enable
#define INT_MAP			0x2F	//Interrpt mapping control
#define INT_SOUCE		0x30	//Source of interrupts
#define DATA_FORMAT		0x31	//Data format Control
#define DATA_X0			0x32	//X-axis Data 0
#define DATA_X1			0x33	//X-axis Data 1
#define DATA_Y0			0x34	//Y-axis Data 0
#define DATA_Y1			0x35	//Y-axis Data 1
#define DATA_Z0			0x36	//Z-axis Data 0
#define DATA_Z1			0x37	//Z-axis Data 1
#define FIFO_CTL		0x38	//FIFO control
#define FIFO_STATUS		0x39	//FIFO status

int AccelAddres  = 0xA6;	//Accelerometer addres

class Accelerometer {

public:
	int X_axis;
	int Y_axis;
	int Z_axis;

	bool accel_initStatus = false;
	void accel_initialize();

	//IRQ functions
	//void IRQ_timer_continuouslyReadAxisValue();

	//Next methods

	//Give me data from axis
	void GetNewData(int x,int y,int z);	//take new data and put to outpu
	void show_axis(int x,int y, int z);	//saved data from old reading

	//Read from axis
	void continuously_read_axis(bool TurnOn, float period);
	void read_axis();
	void read_X();
	void read_y();
	void read_z();

	//device control
	void devSetup(char inputCommand[10]);

	Accelerometer();
	virtual ~Accelerometer();
};

#endif /* ACCELEROMETER_H_ */
