TARGET = neo
SOURCES = main.c msp430_uart.c
OBJECTS = $(SOURCES:.c=.o)

MCU=msp430g2553

LD = msp430-elf-ld
CC = msp430-elf-gcc

SIZE     = msp430-elf-size
OBJCOPY  = msp430-elf-objcopy
MAKETXT  = srec_cat
READELF  = msp430-readelf
CP       = cp -p
MV       = mv
#RM       = rm -f
RM = del /Q
INCLUDES = -IC:\SysGCC\msp430-elf\include
#######################################################################################
CFLAGS   = -mmcu=$(MCU) -g -Os -Wall -Wunused $(INCLUDES)   
ASFLAGS  = -mmcu=$(MCU) -x assembler-with-cpp -Wa,-gstabs
LDFLAGS  = -mmcu=$(MCU) -Wl,-Map=$(TARGET).map -T C:\SysGCC\msp430-elf\include\msp430g2553.ld
#######################################################################################

all: $(TARGET).elf $(TARGET).hex $(TARGET).txt
$(TARGET).elf: $(OBJECTS)
	@echo [LD $@]
	$(CC) $(OBJECTS) $(LDFLAGS) $(LIBS) -o $@
	@echo [Size of Firmware]
	$(SIZE) $(TARGET).elf

%.o : %.c
	@echo [CC $@]
	$(CC) -c $(CFLAGS) -o $@ $<

erase:
	MSP430Flasher.exe -e ERASE_ALL

upload:
	make
	MSP430Flasher.exe -w "$(TARGET).txt" -v -g -z [VCC]

size:
	$(SIZE) $(TARGET).elf

%.hex: %.elf
	@echo [ELFtoHEX $@ from $<]
	$(OBJCOPY) -O ihex $< $@

%.txt: %.hex
	@echo [HEXtoTXT $@ from $<]
	$(MAKETXT) -O $@ -TITXT $< -I
clean:
	-$(RM) $(OBJECTS)
	-$(RM) $(TARGET).elf
	-$(RM) $(TARGET).hex
	-$(RM) $(TARGET).txt
	-$(RM) $(TARGET).map
