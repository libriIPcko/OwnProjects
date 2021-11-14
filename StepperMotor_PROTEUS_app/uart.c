#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include "uart.h"


#define RX_UKONCENE 1
#define RX_NEUKONCENE 2


#define	BUF_SIZE 128 					// Velkost rx pola
char rx_buff[BUF_SIZE];					// Inicializacia rx pola
volatile unsigned char rx_pos,rx_stav = RX_NEUKONCENE;	// ukazovatel pola, stav rx dat

//--------------------------------------------------------------------
/******************************************************
    inicializacia
    - nastavenie baudRate - ako vstupna premenna
    - zapne vysielac a prijimac
    - 8 datovych a 1 stop bity
******************************************************/
void uart_init(unsigned int baud)
{
    unsigned int ubrr = F_CPU/16/baud-1;

	//nastavenie prenosovej rychlosti v UBRR
	UBRRH = (unsigned char) (ubrr>>8);
	UBRRL = (unsigned char) ubrr;

	//zapni prijmac a vysielac, prerusenie pri prijime noveho znaku
	 UCSRA = (1<<RXC)|(1<<TXC)|(1<<UDRE)|(1<<FE)|(1<<DOR);
	 UCSRB = (1<<RXEN)|(1<<TXEN)|(1<<RXCIE)|(1<<TXCIE)|(1<<TXEN);

	//nastav format ramca: 8data, 1stop bity
	UCSRC = (1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);
}

/******************************************************
    odosle bajt (znak)
******************************************************/
void uart_putc(unsigned char data)
{
	//cakaj na vyprazdnenie buffera
	while (!( UCSRA & (1<<UDRE)));

	//vloz data do buffer-a a odosli
	UDR = data;
}

/******************************************************
    odosle retazec bajtov (znakov)
******************************************************/
void uart_puts(char *s)
{
    while (*s)
      uart_putc(*s++);
}

/******************************************************
    prijem bajt-u (znak)
******************************************************/
unsigned char uart_getc(void)
{
	//cakaj na prijatie dat
	while (!(UCSRA & (1<<RXC)));

	//zober data z buffer-a
	return UDR;
}

/*
    prijem retazca znakov
*/
unsigned char uart_gets(char* rx_pole){	
	// ak je prijimanie retazca znakov ukoncene...
	if(rx_stav == RX_UKONCENE) {
		// skopiruj buffer do pola
		strcpy(rx_pole, rx_buff);
		//zacni prijimanie retazca znova
		rx_stav = RX_NEUKONCENE;
		rx_pos = 0;
		// navratova hodnota, boli prijate data	
		return 1;	

	}else{
		// data neboli prijate...
		rx_pole = 0;
		return 0;	
	}

}

/******************************************************
prerusenie rx complete
******************************************************/
ISR(USART_RXC_vect)
{

	// ak pretieklo rx pole tak ho vynuluj
	if (rx_pos == BUF_SIZE) rx_pos = 0;

	// ak neprisiel ukoncovaci znak
	if(rx_stav != RX_UKONCENE){
		
		rx_buff[rx_pos] = UDR;	// ulozenie znaku do buffer-a
	
		//ak prisiel ukoncovaci znak
		if ((rx_buff[rx_pos] == '\r') | (rx_buff[rx_pos] == '\n')){	
			rx_buff[rx_pos+1] = '\0';	// ukoncenie retazca
			rx_stav = RX_UKONCENE;	
		}else{			
			rx_pos++;	 // inkrementuj pocitadlo
		}
	}
}

/******************************************************
    vycistenie buffer-a
******************************************************/
void uart_flush(void)
{
	unsigned char dummy;

	while ( UCSRA & (1<<RXC) ) dummy = UDR;
}
