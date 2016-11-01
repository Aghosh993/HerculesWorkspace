/*Standard HAL (Hardware Abstraction Layer) includes:*/
#include "hal_common_includes.h"

// User/flight software libraries:
#include "basic_pid_controller.h"
#include "iir_filters.h"

#include <stdio.h>

/* USER CODE END */

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

int main(void)
{
/* USER CODE BEGIN (3) */

	/*Globally disable R4 IRQs AND FIQs:*/
	_disable_interrupt_();

	systemInit();

	/*
	* Initialize the serial port (SCI module) and enable SCI Receive interrupts:
	* (Explanation: mibSPI peripheral must be initialized as well since SCI requires mibSPI pins)
	*/
	mibspiInit();
	sciInit();

	printf("Hello from APv2.4!!\r\n");
	char *msg = (char *)malloc(100*sizeof(char));
	// char msg[100];
	// sprintf(msg, "Hello from AP %f!!\r\n", 2.40f);
	sprintf(msg, "Hi there!!\r\n");
	sciSend(sciREG, strlen(msg), (uint8_t *)msg);
	int i = 0;
	while(1)
	{
		char *msg = (char *)malloc(100*sizeof(char));
		/*Toggle GPIO line to indicate we're alive, and for timing purposes:*/
		gioToggleBit(mibspiPORT3, PIN_SIMO);
		// sprintf(msg, "Hello, iter %d\r\n", i);
		// sciSend(sciREG, strlen(msg), (uint8_t *)msg);
		// ++i;
		insert_delay(50);
	}
/* USER CODE END */
}

/* USER CODE BEGIN (4) */
/* USER CODE END */
	