#include <avr/io.h>
#include <util/delay.h>
#include "peri.h"


int main()
{
	init_peripheral_portB();
	init_peripheral_portC();
	while(1)
	{
		while(!IS_AUTOSWITCH_PRESSED()) _delay_ms(100);
		_delay_ms(100);
		set_led_portC(PC5,1);
		while(IS_AUTOSWITCH_PRESSED()) _delay_ms(100);
		set_led_portC(PC5,0);
		_delay_ms(100);
		while(!IS_MANUALSWITCH_PRESSED()) _delay_ms(100);
		_delay_ms(100);
		set_led_portC(PC3,1);
		while(IS_MANUALSWITCH_PRESSED()) _delay_ms(100);
		set_led_portC(PC3,0);
		_delay_ms(100);
	}
}
