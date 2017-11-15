#include <avr/io.h>
#include <util/delay.h>
#include "peri.h"

#define ON 1
#define OFF 0

int main()
{
    init_peripheral_portB();
    while (1)
    {
		set_led(0,ON);
		_delay_ms(500);
		set_led(0,OFF);
        set_led(1,ON);
        _delay_ms(500);
		set_led(1,OFF);
        set_led(2,ON);
        _delay_ms(500);
		set_led(2,OFF);
		set_led(3,ON);
		_delay_ms(500);
		set_led(3,OFF);
		set_led(4,ON);
		_delay_ms(500);
		set_led(4,OFF);
		set_led(5,ON);
		_delay_ms(500);
		set_led(5,OFF);
    }
}
