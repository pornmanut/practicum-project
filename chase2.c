#include <avr/io.h>
#include <util/delay.h>
#include "peri.h"

#define ON 1
#define OFF 0

#define LED_H1 PB5
#define LED_H2 PB3
#define LED_H3 PB1
#define LED_L1 PB0
#define LED_L2 PB2
#define LED_L3 PB4
#define LED_AUTO PC5
#define LED_MANUAL PC3


int main()
{
	init_peripheral_portD();
    while (1)
    {
		set_motor(1);
		_delay_ms(1000);
		set_motor(0);
		_delay_ms(1000);
		set_motor(2);
		_delay_ms(1000);
		set_motor(0);
		_delay_ms(1000);
    }
}
