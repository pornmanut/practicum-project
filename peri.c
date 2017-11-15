#include <avr/io.h>
#include <util/delay.h>

void init_peripheral_portB()
{
	//set all portb output only 6 port led;
	DDRB = 0b00111111;
	//turn off all led at start;
	PORTB = 0b00000000;
}
void set_led(uint8_t pin,uint8_t state)
{
	if(state) PORTB |= (1<<pin);
	else PORTB &= !(1<<pin);
} 
void set_led_value(uint8_t value)
{
	PORTB &= ~(7);
	PORTB |= (value&7);
}
