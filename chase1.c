#include <avr/io.h>
#include <util/delay.h>
#include "peri.h"

#define ON 1
#define OFF 0
#define H_RED PB5
#define H_YELLOW PB3
#define H_GREEN PB1
#define L_RED PB0
#define L_YELLOW PB2
#define L_GREEN PB4
#define LED_AUTO PC5
#define LED_MANUAL PC3


int main()
{
    init_peripheral_portB();
	init_peripheral_portC();
    while (1)
    {
		set_led_portC(LED_AUTO,ON);
		_delay_ms(500);
		set_led_portC(LED_AUTO,OFF);
		set_led_portC(LED_MANUAL,ON);
		_delay_ms(500);
		set_led_portC(LED_MANUAL,OFF);
		set_led_portB(H_RED,ON);
		_delay_ms(500);
		set_led_portB(H_RED,OFF);
        set_led_portB(H_YELLOW,ON);
        _delay_ms(500);
		set_led_portB(H_YELLOW,OFF);
        set_led_portB(H_GREEN,ON);
        _delay_ms(500);
		set_led_portB(H_GREEN,OFF);
		set_led_portB(L_RED,ON);
		_delay_ms(500);
		set_led_portB(L_RED,OFF);
		set_led_portB(L_YELLOW,ON);
		_delay_ms(500);
		set_led_portB(L_YELLOW,OFF);
		set_led_portB(L_GREEN,ON);
		_delay_ms(500);
		set_led_portB(L_GREEN,OFF);
    }
}
