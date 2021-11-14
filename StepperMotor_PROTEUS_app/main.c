#include <inttypes.h>
#include <avr/sleep.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include <string.h> 
#include "xstepper.h"
#include "lcd.h"
#include "uart.h"

int a=0,b=0;
short end =0;
short status =0;
int i,start, control;
char pole[20]="";
char savePole[20]="";

//Status
// if status ==0 --> nothing
// if status ==1 --> fast
// if status ==2 --> stepping
ISR(INT0_vect){
      //test LED
      DDRD |= (1<<7);
      PORTD |=(1<<PD7);
   if(status==1){
   //dekrementacia rychlost
   a++;
   b=a*1000;
   i=b+6000;
   }
   if(status == 2){
      //krokovanie
      StepperStepCW();
   }
}

ISR(INT1_vect){
      //test LED
      DDRD |= (1<<7);
      PORTD &=~(1<<PD7);
   if(status==1){
      //inkrementacia rychlost
      a--;
      if(a<=0){
	 a=0;
	 i=0;
      }
      else{
	 b=a*1000;
	 i=b+6000;
      }
   }     
   if(status == 2){
      //krokovanie
      StepperStepCCW();
   }
}

int main()
{
   lcd_init(LCD_DISP_ON_CURSOR_BLINK);
	 lcd_gotoxy(0,0);
	 lcd_puts("Start system");
	 lcd_gotoxy(0,1);
	 lcd_puts("Wait on Start");
	 //OSCCAL = 0xC0; //<-calibration constant from REAL.MCU
	 OSCCAL = 0x80;   //<-calibration constant from default simulation
	 uart_init(9600);
	//inicializacia EXT.Interrupt
	 MCUCR |= (1<<ISC01)|(1<<ISC11);
	 GICR |= (1<<INT1)|(1<<INT0);
	//enable all interrupt is in CASE
	sei();
   //------------------------------
   // Core of program
   //-----------------------------
	   while(start==0){

		   if((strcmp(pole,"")!=0) | (strcmp(pole,savePole)!=0) | (strcmp(pole,"Start") ==0)){
		     start = 1;
		   }
		    else{
		     start = 0;
		     uart_gets(pole);
		     strcpy(savePole, pole);
		   }
		}
//----------START----------------------------------------
	if(start ==1){
	while(1){
	    lcd_clrscr();
	    start = 0;
	    control = 0;
	    //--------------------------------
	    lcd_gotoxy(0,0);
	    lcd_puts("Vyber ovladania");
	    lcd_gotoxy(0,1);
	    lcd_puts("Rychlost|Poloha");
	    //uart_puts("Vyber ovladania: Rychlost |Poloha");
	    while(control==0){	//<--- ovladane pomocou control
		  uart_gets(pole);
		  if((strcmp(pole,"")!=0) & (strcmp(pole,savePole)!=0)){ //cut & (strcmp(pole,savePole)!=0)
		     strcpy(savePole,pole);
		     //lcd_puts(pole);
		     switch (savePole[1]){
			case 'y':
			// Zvolena rychlost
			  lcd_clrscr();
			  lcd_puts(" zvolena: "); 
			  lcd_puts(pole);
			  StepperInit();
			  status = 1;
			  while(1){
			     //DOPLNIT if(volba smeru otaèok)
			   StepperStepCW();     //Step Clock wise
			   _delay_loop_2(i);//15000
			     //DOPLNIT REAKCIU NA uart_gets(pole) == "Stop"	      
			    uart_gets(pole);
			      if(pole[1]=='t'){
				 StepperDeinit();
				 //end = 1;
				 control = 1;
				 break;
			      }
			   }
			break;
			  
			case 'o':
			// Zvolena poloha
			   lcd_clrscr();
			   lcd_puts(" zvolena: "); 
			   lcd_puts(pole);
			   StepperInit();
			   status = 2;
			   //REAKCIU NA uart_gets(pole) == "Stop"
			   while(end==0){			      
			      uart_gets(pole);
			      if(pole[1]=='t'){
				 StepperDeinit();
				 end = 1;
				 control = 1;
				 break;
			      }
			   }
			break;
			   
			case 't':
			// Zvolene stop
			control = 1;
			StepperDeinit();
			break;
		     }
		     
		  }
	       }
	 }
      } 
return 0;
}