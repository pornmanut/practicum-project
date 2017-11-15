#include <avr/io.h>
#include <util/delay.h>
#include "peri.h"

#define ON 1
#define OFF 0
#define H_RED 5
#define H_YELLOW 3
#define H_GREEN 1
#define L_RED 0
#define L_YELLOW 2
#define L_GREEN 4


int main()
{
    init_peripheral_portB();
    while (1)
    {
		set_led(H_RED,ON);
		_delay_ms(500);
		set_led(H_RED,OFF);
        set_led(H_YELLOW,ON);
        _delay_ms(500);
		set_led(H_YELLOW,OFF);
        set_led(H_GREEN,ON);
        _delay_ms(500);
		set_led(H_GREEN,OFF);
		set_led(L_RED,ON);
		_delay_ms(500);
		set_led(L_RED,OFF);
		set_led(L_YELLOW,ON);
		_delay_ms(500);
		set_led(L_YELLOW,OFF);
		set_led(L_GREEN,ON);
		_delay_ms(500);
		set_led(L_GREEN,OFF);
    }
}
