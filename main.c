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

struct pt pt_taskLColor;
struct pt pt_taskAutoSwitch;
struct pt pt_taskManualSwitch;
struct pt pt_taskReadLight;
struct pt pt_taskCheckLight;
struct pt pt_taskCloseCurtain;



uint16_t lightValue = 0;
uint8_t motorValue = 0;
uint8_t lightState = 0;

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
		PT_WAIT_UNTIL(pt,lightState=HIGH);
		PT_DELAY(pt,10,ts);
		set_motor(1);
		PT_WAIT_WHILE(pt,IS_TRACKER_LEFT());
		PT_DELAY(pt,10,ts);
	}

	PT_END(pt);
}
/////////////////////////////////////////////////////////////
PT_THREAD(taskAutoSwitch(struct pt* pt)){
	
	static uint32_t ts;
	static uint8_t status;

	PT_BEGIN(pt);

	for(;;)
	{
		PT_WAIT_UNTIL(pt,IS_AUTOSWITCH_PRESSED());
		PT_DELAY(pt,10,ts);

		status = (status+1)%2;
		set_led_portC(LED_AUTO,status);

		PT_WAIT_WHILE(pt,IS_AUTOSWITCH_PRESSED());
		PT_DELAY(pt,10,ts);	
	}
	
	PT_END(pt);
}
/////////////////////////////////////////////////////////////
PT_THREAD(taskManualSwitch(struct pt* pt)){
	
	static uint32_t ts;
	static uint32_t status;

	PT_BEGIN(pt);

	for(;;)
	{
		PT_WAIT_UNTIL(pt,IS_MANUALSWITCH_PRESSED());
		PT_DELAY(pt,10,ts);
		
		status = (status+1)%2;
		motorValue = (motorValue+1)%3;
		set_led_portC(LED_MANUAL,status);
		
		PT_WAIT_WHILE(pt,IS_MANUALSWITCH_PRESSED());
		PT_DELAY(pt,10,ts);
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
int main()
{
	init_peripheral();
	timer_init();
	sei();

	PT_INIT(&pt_taskLColor);
	PT_INIT(&pt_taskAutoSwitch);
	PT_INIT(&pt_taskManualSwitch);
	PT_INIT(&pt_taskReadLight);
	PT_INIT(&pt_taskCheckLight);
	PT_INIT(&pt_taskCloseCurtain);

	for(;;)
	{
		taskReadLight(&pt_taskReadLight);
		taskManualSwitch(&pt_taskManualSwitch);
		taskLColor(&pt_taskLColor);
		taskAutoSwitch(&pt_taskAutoSwitch);
		taskCheckLight(&pt_taskCheckLight);
		taskCloseCurtain(&pt_taskCloseCurtain);
	}
}
