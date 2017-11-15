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

#define PT_DELAY(pt,ms,ts) \
	ts = timer_millis();\
	PT_WAIT_WHILE(pt,timer_millis()-ts <(ms));


struct pt pt_taskHColor;
struct pt pt_taskLColor;
struct pt pt_taskAutoSwitch;
struct pt pt_taskManualSwitch;

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
		set_led_portB(LED_L1,ON);
		PT_DELAY(pt,300,ts);
		set_led_portB(LED_L1,OFF);
		set_led_portB(LED_L2,ON);
		PT_DELAY(pt,300,ts);
		set_led_portB(LED_L2,OFF);
		set_led_portB(LED_L3,ON);
		PT_DELAY(pt,300,ts);
		set_led_portB(LED_L3,OFF);
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
		set_led_portC(LED_MANUAL,status);

		PT_WAIT_WHILE(pt,IS_MANUALSWITCH_PRESSED());
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

	PT_INIT(&pt_taskHColor);
	PT_INIT(&pt_taskLColor);
	PT_INIT(&pt_taskAutoSwitch);
	PT_INIT(&pt_taskManualSwitch);
	
	for(;;){
		taskHColor(&pt_taskHColor);
		taskLColor(&pt_taskLColor);
		taskAutoSwitch(&pt_taskAutoSwitch);
		taskManualSwitch(&pt_taskManualSwitch);
	}
}
