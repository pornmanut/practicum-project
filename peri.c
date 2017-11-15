#include <avr/io.h>
#include <util/delay.h>

void init_peripheral_portB()
{
	//set all portb output only 6 port led;
	DDRB = 0b00111111;
	//turn off all led at start;
	PORTB = 0b00000000;
}
void init_peripheral_portC()
{
	DDRC = 0b00101000;
	PORTC = 0b00000000;
}
void set_led_portB(uint8_t pin,uint8_t state)
{
	if(state) PORTB |= (1<<pin);
	else PORTB &= !(1<<pin);
} 
void set_led_portC(uint8_t pin,uint8_t state)
{
	if(state) PORTC |= (1<<pin);
	else PORTC &= !(1<<pin);
}
