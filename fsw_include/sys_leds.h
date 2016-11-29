#ifndef SYS_LEDS_H_
#define SYS_LEDS_H_		1

#include <hal_common_includes.h>

// Red LED on interface board:
#define SYS_LED1	mibspiPORT3, PIN_SOMI
// Green LED on interface board:
#define SYS_LED2	mibspiPORT3, PIN_CLK

#define sys_led_init()		mibspiInit()
#define sys_ledOn(led)		gioSetBit(led, 1)
#define sys_ledOff(led)		gioSetBit(led, 0)
#define sys_ledToggle(led)	gioToggleBit(led)

#endif