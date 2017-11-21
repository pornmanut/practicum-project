TARGET=main
MCU=atmega168
F_CPU=16000000
DHTXX_TIMEOUT = 60

CFLAGS= -DDHTXX_TIMEOUT=$(DHTXX_TIMEOUT) -mmcu=$(MCU) -Os -DF_CPU=$(F_CPU) -Iusbdrv -I.
OBJS=peri.o timer.o ./obj/dhtxx.o
USB_OBJS=usbdrv/usbdrv.o usbdrv/usbdrvasm.o

.SECONDARY:

all: $(TARGET).hex

flash: $(TARGET).flash

usb.elf: usb.o $(OBJS) $(USB_OBJS)
	avr-gcc $(CFLAGS) -o $@ $^

%.flash: %.hex
	avrdude -p $(MCU) -c usbasp -u -U flash:w:$<

%.hex: %.elf
	avr-objcopy -j .text -j .data -O ihex $< $@

%.elf: %.o $(OBJS)
	avr-gcc $(CFLAGS) -o $@ $^

%.o: %.c
	avr-gcc $(CFLAGS) -c -o $@ $<

%.i: %.c
	avr-gcc $(CFLAGS) -E -o $@ $<

%.o: %.S
	avr-gcc $(CFLAGS) -x assembler-with-cpp -c -o $@ $<

clean:
	rm -f *~
	rm -f *.o
	rm -f *.i
	rm -f *.elf
	rm -f *.hex
	rm -f $(OBJS) $(USB_OBJS)
	rm -f $(TARGET)
