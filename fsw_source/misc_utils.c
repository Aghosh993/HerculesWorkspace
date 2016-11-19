/*
	File: misc_utils.c

	(c) Abhimanyu Ghosh, 2016
 */
#include "misc_utils.h"

/*
 * Simple blocking delay. Normally not used, instead the timekeeper_delay() function in
 * mission_timekeeper module is used due to tighter tolerance and accuracy (down to 1 millisecond).
 * However, that function requires interrupts to be enabled, and cannot run within an ISR.
 */

void insert_delay(int ms)
{
	int i = 0;
	int j = 0;
	for(i=0;i<ms;++i)
	{
		for(j=0;j<6250;++j)
		{
			++j;
		}
	}
}

void blocking_sci_SendData(uint8_t *dataptr, uint8_t len)
{
	uint8_t i = 0U;
	for(i=0; i<len; ++i)
	{
		sciSendByte(sciREG, dataptr[i]);
	}
}