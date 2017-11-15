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
	init_peripheral_portB();
	init_peripheral_portC();
    while (1)
    {
		while(!IS_SWITCH_PRESSED());	
		_delay_ms(10);
		set_led_portC(LED_AUTO,ON);
		while(IS_SWITCH_PRESSED());
		_delay_ms(10);
		set_led_portC(LED_AUTO,OFF);
		_delay_ms(500);
    }
}
