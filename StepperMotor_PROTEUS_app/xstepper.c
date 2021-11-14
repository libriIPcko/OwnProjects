#include <avr/io.h>

#include "xstepper.h"

void StepperInit()
{
	__STEPPER_DDR|=(0X0F<<STEPPER_POS);
	__STEPPER_PORT|=(1<<STEPPER_POS);
}
void StepperDeinit(){
      	__STEPPER_DDR|=(0X00<<STEPPER_POS);
	__STEPPER_PORT&=~(1<<STEPPER_POS);
}
void StepperStepCCW() //anti clock wise
{
	uint8_t temp=(__STEPPER_PORT & (0x0F<<STEPPER_POS));
	temp=temp>>(STEPPER_POS);
	temp=temp<<1;

	if(temp>(8))
		temp=1;
	
	__STEPPER_PORT=(__STEPPER_PORT & ~(0x0F<<STEPPER_POS))|(temp<<STEPPER_POS);
}

void StepperStepCW() //clock wise
{
	uint8_t temp=(__STEPPER_PORT & (0x0F<<STEPPER_POS));
	temp=temp>>(STEPPER_POS);
	temp=temp>>1;

	if(temp==0)
		temp=8;//8
	
	__STEPPER_PORT=(__STEPPER_PORT & ~(0x0F<<STEPPER_POS))|(temp<<STEPPER_POS);
}
//


