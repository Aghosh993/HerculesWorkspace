/*Standard HAL (Hardware Abstraction Layer) includes:*/
#include "hal_common_includes.h"

// User/flight software libraries:
#include "basic_pid_controller.h"
#include "iir_filters.h"
#include "Quadcopter_PWM_HAL.h"

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

#define __ASM            __asm                                      /*!< asm keyword for GNU Compiler          */
#define __INLINE         inline                                     /*!< inline keyword for GNU Compiler       */
#define __STATIC_INLINE  static inline

extern unsigned int _get_SP_(void);

__attribute__( ( always_inline ) ) __STATIC_INLINE uint32_t __get_MSP2(void)
{
  register uint32_t result;

  // __ASM volatile ("MRS %0, msp\n" : "=r" (result) ); // What do we do instead for the Cortex-R4???
  // __ASM volatile ("MOV %0, sp\n" : "=r" (result) );
  return(_get_SP_());
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

	// Not using heap:
	printf("Hello from APv2.4!!\r\n");

	// Attempt to use heap:
	char *msg = (char *)malloc(100*sizeof(char));
	sprintf(msg, "Hello from AP %f!!\r\n", 2.40f);
	sciSend(sciREG, strlen(msg), (uint8_t *)msg);

	int i = 0;

	printf("Ready to execute loop!!\r\n");
	
	printf("addr of msg1 buffer = %0x\r\n", msg);

	QuadRotor_PWM_init();
	QuadRotor_motor1_start();
	QuadRotor_motor2_start();
	QuadRotor_motor3_start();
	QuadRotor_motor4_start();

	QuadRotor_motor1_setDuty(0.25f);
	QuadRotor_motor2_setDuty(0.50f);
	QuadRotor_motor3_setDuty(0.75f);
	QuadRotor_motor4_setDuty(0.85f);

	while(1)
	{
		char *msg2 = (char *)malloc(10*sizeof(char));
		// printf("addr of msg2 buffer = %d\r\n", msg2);
		// free(msg2);
		/*Toggle GPIO line to indicate we're alive, and for timing purposes:*/
		gioToggleBit(mibspiPORT3, PIN_SIMO);
		// sprintf(msg, "Hello, iter %d\r\n", i);
		// sciSend(sciREG, strlen(msg), (uint8_t *)msg);
		// ++i;
		insert_delay(500);
	}
/* USER CODE END */
}

/* USER CODE BEGIN (4) */
/* USER CODE END */
	