#include <avr/io.h>
#include <util/delay.h>

void init_peripheral()
{
	/*	PORTB
	 *	LED PB0 PB1 PB2 PB3 PB4 PB5
	 *
	 */

	DDRB = 0b00111111;
	PORTB = 0b00000000;

	/*	PORTC
	 *	LED PC3 PC5
	 *	SWITCH PC2 PC4 NO EXTRANAL PULL-UP
	 */ 
	
	DDRC = 0b00101000;
	PORTC = 0b00000000;

	/*	PORTD
	 *	MOTOR PD1 PD6
	 *
	 */

	DDRD = 0b1000010;
	PORTD = 0;
}

void set_led_portB(uint8_t pin,uint8_t state)
{
	if(state) PORTB |= (1<<pin);
	else PORTB &= ~(1<<pin);
}
 
void set_led_portC(uint8_t pin,uint8_t state)
{
	if(state) PORTC |= (1<<pin);
	else PORTC &= ~(1<<pin);
}

/*	void set_motor(uint8_t state)
 *	0 TURNOFF
 *	1 MOVE_FORWARD
 *	2 MOVE_BACKWARD
 */

void set_motor(uint8_t state)
{
	PORTD &= !0b1000010;
	switch(state){
	case 1: PORTD |= (1<<1);
			break;
	case 2: PORTD |= (1<<6);
			break;
	}	
}
