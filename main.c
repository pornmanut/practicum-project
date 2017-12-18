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

#define H1LED_PB PB5
#define H2LED_PB PB3
#define H3LED_PB PB1

#define L1LED_PB PB0
#define L2LED_PB PB2
#define L3LED_PB PB4

#define AUTOLED_PC PC5
#define STATUSLED_PC PC3

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

#define DRY 2
#define NORMAL 5
#define WET 9

#define MOTOR_STOP 0
#define MOTOR_FORWARD 1
#define MOTOR_BACKWARD 2

#define STATUS_OPEN 0
#define STATUS_TO_CLOSE 1
#define STATUS_CLOSE 2
#define STATUS_TO_OPEN 3

#define AUTO_TO_CLOSE() (auto_state == ON && (light_state >= HIGH || humid_state >= WET))
#define AUTO_TO_OPEN() (auto_state == ON && (light_state <= LOW || humid_state <= DRY))

struct pt pt_taskReadWriteColor;
struct pt pt_taskControlCurtain;
struct pt pt_taskAutoSwitch;

uint8_t light_state = LOW;
uint8_t status = STATUS_OPEN;
uint8_t auto_state = OFF;
uint8_t humid_state = NORMAL;

int humid = 0;
int temp = 0;


/////////////////////////////////////////////////////////////
PT_THREAD(taskAutoSwitch(struct pt* pt)){
	
	static uint32_t ts;

	PT_BEGIN(pt);
	for(;;)
	{
		PT_WAIT_UNTIL(pt,IS_AUTOSWITCH_PRESSED());
		PT_DELAY(pt,100,ts);
		
		auto_state = (auto_state+1)%2;

		PT_WAIT_WHILE(pt,IS_AUTOSWITCH_PRESSED());
		PT_DELAY(pt,100,ts);
	}	
	PT_END(pt);

}
/////////////////////////////////////////////////////////////
PT_THREAD(taskReadWriteColor(struct pt* pt)){
	
	static uint32_t ts;
	static uint8_t blink=OFF;	
	
	PT_BEGIN(pt);

	for(;;)
	{
		/*	
		*	readlight form cytocell
		*	1023/300	
		*	light_state = 0,1,2,3,4
		*/
		light_state = (int)(read_light()/300.0);
	
		/*show humid level*/
		if(humid_state <= DRY){
			set_led_portB(H1LED_PB,ON);
			set_led_portB(H2LED_PB,OFF);
		}else if(humid_state >= WET){
			set_led_portB(H2LED_PB,OFF);
			set_led_portB(H3LED_PB,ON);
		}else{
			set_led_portB(H1LED_PB,OFF);
			set_led_portB(H2LED_PB,ON);
			set_led_portB(H3LED_PB,OFF);
		}
		/*show light level*/
		switch(light_state){
			case ULTRA_LOW:
			case LOW:
				set_led_portB(L1LED_PB,ON);
				set_led_portB(L2LED_PB,OFF);
				break;
			case MEDIUM:
				set_led_portB(L2LED_PB,ON);
				set_led_portB(L1LED_PB,OFF);
				set_led_portB(L3LED_PB,OFF);
				break;
			case ULTRA_HIGH:
			case HIGH:	
				set_led_portB(L3LED_PB,ON);
				set_led_portB(L2LED_PB,OFF);
				break;
		}
		/*show status*/	
		switch(status){
			case STATUS_OPEN:
				set_led_portC(STATUSLED_PC,ON);
				break;
			case STATUS_CLOSE:
				set_led_portC(STATUSLED_PC,OFF);
				break;
			case STATUS_TO_CLOSE:
			case STATUS_TO_OPEN:
				set_led_portC(STATUSLED_PC,blink);
				blink = (blink+1)%2;
				break;
		}
		/*show auto_state*/	
		set_led_portC(AUTOLED_PC,auto_state);
		/*delay for sure*/	
		PT_DELAY(pt,100,ts);

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
		/*start with open*/
		status = STATUS_OPEN;
		set_motor(MOTOR_STOP);
		
		/*if manual have been pressed auto will turn off*/
		PT_WAIT_UNTIL(pt,AUTO_TO_CLOSE() || IS_MANUALSWITCH_PRESSED());
		if(IS_MANUALSWITCH_PRESSED()) auto_state = OFF;
		
		/*delay for sure*/	
		PT_DELAY(pt,100,ts);
		
		/*motor going to moving*/	
		status = STATUS_TO_CLOSE;
		set_motor(MOTOR_BACKWARD);
			
		/*Wait for the end of line*/
		PT_WAIT_UNTIL(pt,IS_TRACKER_LEFT());
		if(IS_MANUALSWITCH_PRESSED()) auto_state = OFF;
		PT_DELAY(pt,150,ts);
		
		/*stop*/	
		status = STATUS_CLOSE;	
		set_motor(MOTOR_STOP);

		/*wait for new order*/
		PT_WAIT_UNTIL(pt,AUTO_TO_OPEN() || IS_MANUALSWITCH_PRESSED());
		if(IS_MANUALSWITCH_PRESSED()) auto_state = OFF;
		
		PT_DELAY(pt,100,ts);
		/*motor goint to moving*/	
		status = STATUS_TO_OPEN;
		set_motor(MOTOR_FORWARD);

		/*Wait for the start of line*/	
		PT_WAIT_UNTIL(pt,IS_TRACKER_RIGHT());
		if(IS_MANUALSWITCH_PRESSED()) auto_state = OFF;
		PT_DELAY(pt,150,ts);
	}

	PT_END(pt);
}
/////////////////////////////////////////////////////////////

uint8_t read_humid(){
	uint8_t e;
	dhtxxconvert(DHTXX_DHT22,&PORTC,&DDRC,&PINC,(1<<DHT_PC));
	_delay_ms(200);	
	e = dhtxxread(DHTXX_DHT22,&PORTC,&DDRC,&PINC,(1<<DHT_PC),&temp,&humid);
	humid_state = (int)(humid/100);
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

	for(;;)
	{
		read_humid();
		taskReadWriteColor(&pt_taskReadWriteColor);
		taskControlCurtain(&pt_taskControlCurtain);
		taskAutoSwitch(&pt_taskAutoSwitch);
	}
}
