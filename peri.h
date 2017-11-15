#define IS_AUTOSWITCH_PRESSED() !(PINC & (1<<PC2))
#define IS_MANUALSWITCH_PRESSED() !(PINC & (1<<PC4))

//uint16_t read_adc(uint8_t channel);
void init_peripheral_portB();
//void set_led(uint8_t,uint8_t);
void set_led_value(uint8_t);
