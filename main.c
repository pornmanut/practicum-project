#include <stddef.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "pt/pt.h"
#include "peri.h"
#include "timer.h"
#include "./include/dhtxx.h"

#define OFF 0
#define ON 1

#define LED_H1 PB5
#define LED_H2 PB3
#define LED_H3 PB1

#define LED_L1 PB0
#define LED_L2 PB2
#define LED_L3 PB4

#define LED_AUTO PC5
#define LED_MANUAL PC3

#define IS_AUTOSWITCH_PRESSED() !(PINC & (1<<PC2))
#define IS_MANUALSWITCH_PRESSED() !(PINC & (1<<PC4))

#define IS_TRACKER_LEFT() !(PIND & (1<<PD0))
#define IS_TRACKER_RIGHT() !(PIND & (1<<PD5))

#define DHT_PC PC0 

#define PT_DELAY(pt,ms,ts) \
	ts = timer_millis();\
	PT_WAIT_WHILE(pt,timer_millis()-ts <(ms));

#define ULTRA_LOW 0
#define LOW 1
#define MEDIUM 2
#define HIGH 3
#define ULTRA_HIGH 4

#define ULTRA_DRY 0
#define DRY 1
#define NORMAL 2
#define WET 3
#define ULTRA_WET 4

#define CLOSE 0
#define OPEN 1

#define MANUALSTATE 0
#define AUTOSTATE 1

#define AUTO_IS_OPEN() ((humidState == ULTRA_WET || lightState==HIGH) && currentState==OPEN && state==AUTOSTATE)
#define MANUAL_IS_OPEN() (currentState==OPEN && state==MANUALSTATE && status==ON)
#define AUTO_IS_CLOSE() ((humidState == ULTRA_DRY || lightState==LOW) && currentState==CLOSE && state==AUTOSTATE)
#define MANUAL_IS_CLOSE() (currentState==CLOSE && state == MANUALSTATE && status ==ON)

struct pt pt_taskReadWriteColor;
struct pt pt_taskControlCurtain;
struct pt pt_taskAutoSwitch;
struct pt pt_taskManualSwitch;
struct pt pt_taskReadDHT;

uint8_t lightState = LOW;
uint8_t currentState = OPEN;
uint8_t state = AUTOSTATE;
uint8_t status = OFF;
uint8_t humidState = DRY;
int humid = 0;
int temp = 0;


/////////////////////////////////////////////////////////////
PT_THREAD(taskManualSwitch(struct pt* pt)){

	static uint32_t ts;

	PT_BEGIN(pt);
	for(;;)
	{
		PT_WAIT_UNTIL(pt,IS_MANUALSWITCH_PRESSED());
		PT_DELAY(pt,50,ts);
		state = MANUALSTATE;	
		status = (status+1)%2;

		PT_WAIT_WHILE(pt,IS_MANUALSWITCH_PRESSED());
		PT_DELAY(pt,50,ts);
	}
	PT_END(pt);

}
/////////////////////////////////////////////////////////////
PT_THREAD(taskAutoSwitch(struct pt* pt)){
	
	static uint32_t ts;

	PT_BEGIN(pt);
	for(;;)
	{
		PT_WAIT_UNTIL(pt,IS_AUTOSWITCH_PRESSED());
		PT_DELAY(pt,50,ts);
		
		state = (state+1)%2;

		PT_WAIT_WHILE(pt,IS_AUTOSWITCH_PRESSED());
		PT_DELAY(pt,50,ts);
	}	
	PT_END(pt);

}
/////////////////////////////////////////////////////////////
PT_THREAD(taskReadWriteColor(struct pt* pt)){
	
	static uint32_t ts;
	
	PT_BEGIN(pt);

	for(;;)
	{
		lightState = (int)(read_light()/300.0);
		switch(humidState){
			case ULTRA_DRY:
				set_led_portB(LED_H1,ON);
				set_led_portB(LED_H2,OFF);
				break;
			case DRY:
			case NORMAL:
			case WET:
				set_led_portB(LED_H1,OFF);
				set_led_portB(LED_H2,ON);
				set_led_portB(LED_H3,OFF);
				break;
			case ULTRA_WET:	
				set_led_portB(LED_H2,OFF);
				set_led_portB(LED_H3,ON);
				break;
		}
		switch(lightState){
			case ULTRA_LOW:
			case LOW:
				set_led_portB(LED_L1,ON);
				set_led_portB(LED_L2,OFF);
				break;
			case MEDIUM:
				set_led_portB(LED_L2,ON);
				set_led_portB(LED_L1,OFF);
				set_led_portB(LED_L3,OFF);
				break;
			case ULTRA_HIGH:
			case HIGH:	
				set_led_portB(LED_L3,ON);
				set_led_portB(LED_L2,OFF);
				break;
		}

		set_led_portC(LED_MANUAL,currentState);	
		set_led_portC(LED_AUTO,state);
			
		PT_DELAY(pt,50,ts);

	}
	PT_END(pt);
}
/////////////////////////////////////////////////////////////
PT_THREAD(taskControlCurtain(struct pt* pt))
{
	static uint32_t ts;

	PT_BEGIN(pt);

	for(;;)
	{
		set_motor(0);
		PT_WAIT_UNTIL(pt,AUTO_IS_OPEN() || MANUAL_IS_OPEN());
		PT_DELAY(pt,100,ts);
		
		status = OFF;
		set_motor(2);

		PT_WAIT_UNTIL(pt,IS_TRACKER_RIGHT());
		PT_DELAY(pt,50,ts);
		
		currentState = CLOSE;
		PT_DELAY(pt,100,ts);
		
		set_motor(0);
		PT_WAIT_UNTIL(pt,AUTO_IS_CLOSE() || MANUAL_IS_CLOSE());
		PT_DELAY(pt,100,ts);
		
		status = OFF;
		set_motor(1);
		
		PT_WAIT_UNTIL(pt,IS_TRACKER_LEFT());
		PT_DELAY(pt,50,ts);

		currentState = OPEN;
		PT_DELAY(pt,100,ts);
	}

	PT_END(pt);
}
/////////////////////////////////////////////////////////////

uint8_t read_humid(){
	uint8_t e;
	dhtxxconvert(DHTXX_DHT22,&PORTC,&DDRC,&PINC,(1<<DHT_PC));
	_delay_ms(200);	
	e = dhtxxread(DHTXX_DHT22,&PORTC,&DDRC,&PINC,(1<<DHT_PC),&temp,&humid);
	humidState = (int)(humid-500)/100;
	return e;
}

int main()
{
	init_peripheral();
	timer_init();
	sei();

	PT_INIT(&pt_taskReadWriteColor);
	PT_INIT(&pt_taskControlCurtain);
	PT_INIT(&pt_taskAutoSwitch);
	PT_INIT(&pt_taskManualSwitch);	

	for(;;)
	{
		read_humid();
		taskReadWriteColor(&pt_taskReadWriteColor);
		taskControlCurtain(&pt_taskControlCurtain);
		taskAutoSwitch(&pt_taskAutoSwitch);
		taskManualSwitch(&pt_taskManualSwitch);
	}
}
