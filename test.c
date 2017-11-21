#include <stddef.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "./include/dhtxx.h"
#include "peri.h"

int main( )
{
	init_peripheral();
	uint8_t ec=4; //Exit code
	int temp=0, humid=0; //Temperature and humidity

	while( 1 )
	{
		//Request DHT sensor to give it time to prepare data
		dhtxxconvert( DHTXX_DHT22, &PORTC, &DDRC, &PINC, ( 1 << 0 ) );

		_delay_ms(1000);

		//Read data from sensor to variables `temp` and `humid` (`ec` is exit code)
		ec = dhtxxread( DHTXX_DHT22, &PORTC, &DDRC, &PINC, ( 1 << 0 ), &temp, &humid );
		if(humid >700 && humid < 800)set_led_portB(5,1);
		else set_led_portB(5,0);
		if(humid >800 && humid < 900)set_led_portB(3,1);
		else set_led_portB(3,0);
		if(humid >900 && humid < 1000)set_led_portB(1,1);
		else set_led_portB(1,0);

		if(temp >250 && temp < 270)set_led_portB(0,1);
		else set_led_portB(0,0);
		if(temp >270 && temp < 290)set_led_portB(2,1);
		else set_led_portB(2,0);
		if(temp >290 && temp < 310)set_led_portB(4,1);
		else set_led_portB(4,0);
	}
	return 0;
}
