# Based on https://github.com/sergioprado/bare-metal-arm/blob/master/Makefile
# But heavily-edited for our purposes:
TOOLCHAIN_ROOT=/home/aghosh01/gcc-arm-none-eabi-4_7-2012q4
TOOLCHAIN=$(TOOLCHAIN_ROOT)/bin/
PREFIX=$(TOOLCHAIN)/arm-none-eabi-

JLINK_ROOT=../JLink_Linux_V480_x86_64/

ARCHFLAGS=-mcpu=cortex-r4f -march=armv7-r -mfloat-abi=soft -mfpu=vfpv3-d16 -I$(TOOLCHAIN_ROOT)/arm-none-eabi/include  -g -gdwarf-3 -c -mlittle-endian -r
CFLAGS=-nostdlib -I./hal_include/ -I./fsw_include -I./fsw_include/hal_interface -g -O0 -Wall
LDFLAGS= -Wl,--gc-sections,-Map,$(TARGET).map,-Tsys_link.ld

CC=$(PREFIX)gcc
LD=$(PREFIX)gcc
OBJCOPY=$(PREFIX)objcopy
SIZE=$(PREFIX)size
RM=rm -f

TARGET=ap_user_main_app

SRC=$(wildcard hal_source/*.c)
ASM_SRC=$(wildcard hal_source/*.s)
USER_SRC=$(TARGET).c
FSW_SRC=$(wildcard fsw_source/*.c)
FSW_SRC+=$(wildcard fsw_source/hal_interface/*.c)
NEWLIB_ASM_SRC=$(wildcard fsw_source/*.s)

OBJ=$(patsubst %.c, %.o, $(SRC))
ASM_OBJS=$(patsubst %.s, %.o, $(ASM_SRC))
USER_OBJ=$(TARGET).o
FSW_OBJ=$(patsubst %.c, %.o, $(FSW_SRC))
NEWLIB_ASM_OBJ=$(patsubst %.s, %.o, $(NEWLIB_ASM_SRC))

all: build size
build: elf srec bin 
elf: $(TARGET).elf
srec: $(TARGET).srec
bin: $(TARGET).bin
load: all
	./load_fw.sh $(JLINK_ROOT) /usr/bin # The second argument here is the path to arm-none-eabi-gdb. This should really just be $(TOOLCHAIN) but Arch Linux throws a stupid libncurses.so error so we're just using the distro's shipped arm-none-eabi-gdb for the time being... :/

clean:
	$(RM) $(TARGET).srec $(TARGET).elf $(TARGET).bin $(TARGET).map $(OBJ) $(ASM_OBJS) $(NEWLIB_ASM_OBJ) $(USER_OBJ) $(FSW_OBJ)

%.o: %.s
	$(CC) -c $(ARCHFLAGS) $(CFLAGS) -o $@ $<

%.o: %.c
	$(CC) -c $(ARCHFLAGS) $(CFLAGS) -o $@ $<

$(TARGET).elf: $(ASM_OBJS) $(NEWLIB_ASM_OBJ) $(OBJ) $(USER_OBJ) $(FSW_OBJ)
	$(LD) $(LDFLAGS) -o $@ $(OBJ) $(ASM_OBJS) $(NEWLIB_ASM_OBJ) $(USER_OBJ) $(FSW_OBJ)

%.srec: %.elf
	$(OBJCOPY) -O srec $< $@

%.bin: %.elf
	    $(OBJCOPY) -O binary $< $@

size: $(TARGET).elf
	$(SIZE) $(TARGET).elf