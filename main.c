#include <stddef.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "pt/pt.h"
#include "peri.h"
#include "timer.h"


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

#define PT_DELAY(pt,ms,ts) \
	ts = timer_millis();\
	PT_WAIT_WHILE(pt,timer_millis()-ts <(ms));


#define LOW 1
#define MEDIUM 2
#define HIGH 3

#define OPEN 0
#define CLOSE 1
#define AUTOSTATE 0
#define MANUALSTATE 1

struct pt pt_taskLColor;
struct pt pt_taskReadLight;
struct pt pt_taskCheckLight;
struct pt pt_taskCloseCurtain;
struct pt pt_taskOpenCurtain;
struct pt pt_taskShowCurrentState;
struct pt pt_taskAutoState;

uint16_t lightValue = 0;
uint8_t lightState = LOW;
uint8_t currentState = OPEN;
uint8_t state = AUTOSTATE;
#define AUTO_IS_OPEN() (lightState==HIGH && currentState==OPEN && state==AUTOSTATE)
#define MANUAL_IS_OPEN() (currentState==OPEN && state==MANUALSTATE)
#define AUTO_IS_CLOSE() (lightState==LOW && currentState==CLOSE && state==AUTOSTATE)
#define MANUAL_IS_CLOSE() (currentState==CLOSE && state == MANUALSTATE)
/////////////////////////////////////////////////////////////
PT_THREAD(taskHColor(struct pt* pt))
{
	static uint32_t ts;
	
	PT_BEGIN(pt);

	for(;;)
	{
		set_led_portB(LED_H1,ON);
		PT_DELAY(pt,500,ts);
		set_led_portB(LED_H1,OFF);
		set_led_portB(LED_H2,ON);
		PT_DELAY(pt,500,ts);
		set_led_portB(LED_H2,OFF);
		set_led_portB(LED_H3,ON);
		PT_DELAY(pt,500,ts);
		set_led_portB(LED_H3,OFF);
	}

	PT_END(pt);
}
/////////////////////////////////////////////////////////////
PT_THREAD(taskLColor(struct pt* pt)){
	
	static uint32_t ts;
	
	PT_BEGIN(pt);

	for(;;)
	{
		switch(lightState){
			case LOW:	set_led_portB(LED_L1,ON);
						set_led_portB(LED_L2,OFF);
					break;
			case MEDIUM:set_led_portB(LED_L2,ON);
						set_led_portB(LED_L1,OFF);
						set_led_portB(LED_L3,OFF);
					break;
			case HIGH:	set_led_portB(LED_L3,ON);
						set_led_portB(LED_L2,OFF);
					break;
		}
		set_led_portB(LED_H1,IS_TRACKER_RIGHT());
		PT_DELAY(pt,100,ts);

	}
	PT_END(pt);
}
/////////////////////////////////////////////////////////////
PT_THREAD(taskCheckLight(struct pt* pt)){
	
	static uint32_t ts;
	
	PT_BEGIN(pt);

	for(;;)
	{
		switch((int)lightValue/300){
			case 1:	lightState = LOW;
					break;
			case 2: lightState = MEDIUM;
					break;
			case 3: lightState = HIGH;
					break;
		}
		PT_DELAY(pt,100,ts);
	}
	PT_END(pt);
}

/////////////////////////////////////////////////////////////
PT_THREAD(taskCloseCurtain(struct pt* pt))
{
	static uint32_t ts;

	PT_BEGIN(pt);

	for(;;)
	{
		set_motor(0);
		PT_WAIT_UNTIL(pt,AUTO_IS_OPEN() || MANUAL_IS_OPEN());
		PT_DELAY(pt,10,ts);
		set_motor(1);
		PT_WAIT_UNTIL(pt,IS_TRACKER_LEFT());
		PT_DELAY(pt,10,ts);
		currentState = CLOSE;
	}

	PT_END(pt);
}
/////////////////////////////////////////////////////////////
PT_THREAD(taskReadLight(struct pt* pt)){
	
	static uint32_t ts;

	PT_BEGIN(pt);

	for(;;)
	{
		lightValue = read_light();
		PT_DELAY(pt,10,ts);
	}
	
	PT_END(pt);
}
/////////////////////////////////////////////////////////////
PT_THREAD(taskOpenCurtain(struct pt* pt))
{
	static uint32_t ts;

	PT_BEGIN(pt);

	for(;;)
	{
		set_motor(0);
		PT_WAIT_UNTIL(pt,AUTO_IS_CLOSE() || MANUAL_IS_CLOSE());
		PT_DELAY(pt,10,ts);
		set_motor(2);
		PT_WAIT_UNTIL(pt,IS_TRACKER_RIGHT());
		PT_DELAY(pt,10,ts);
		currentState = OPEN;
	}

	PT_END(pt);
}
/////////////////////////////////////////////////////////////
PT_THREAD(taskShowCurrentState(struct pt* pt))
{
	static uint32_t ts;
	
	PT_BEGIN(pt);

	for(;;)
	{
		switch(currentState){
		case OPEN:	set_led_portC(LED_MANUAL,ON);
					break;
		case CLOSE: set_led_portC(LED_MANUAL,OFF);
					break;
		}
		PT_DELAY(pt,10,ts);
	}
	
	PT_END(pt);
}
/////////////////////////////////////////////////////////////
int main()
{
	init_peripheral();
	timer_init();
	sei();

	PT_INIT(&pt_taskLColor);
	PT_INIT(&pt_taskReadLight);
	PT_INIT(&pt_taskCheckLight);
	PT_INIT(&pt_taskCloseCurtain);
	PT_INIT(&pt_taskOpenCurtain);
	PT_INIT(&pt_taskShowCurrentState);

	for(;;)
	{
		taskReadLight(&pt_taskReadLight);
		taskCheckLight(&pt_taskCheckLight);
		taskLColor(&pt_taskLColor);
		taskCloseCurtain(&pt_taskCloseCurtain);
		taskOpenCurtain(&pt_taskOpenCurtain);
		taskShowCurrentState(&pt_taskShowCurrentState);
	}
}
