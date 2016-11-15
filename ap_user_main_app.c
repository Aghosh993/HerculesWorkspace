/*Standard HAL (Hardware Abstraction Layer) includes:*/
#include "hal_common_includes.h"
#include "interrupt_utils.h"

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

// #define __ASM            __asm                                      /*!< asm keyword for GNU Compiler          */
// #define __INLINE         inline                                     /*!< inline keyword for GNU Compiler       */
// #define __STATIC_INLINE  static inline

// extern unsigned int _get_SP_(void);

// __attribute__( ( always_inline ) ) __STATIC_INLINE uint32_t __get_MSP2(void)
// {
//   register uint32_t result;

//   // __ASM volatile ("MRS %0, msp\n" : "=r" (result) ); // What do we do instead for the Cortex-R4???
//   // __ASM volatile ("MOV %0, sp\n" : "=r" (result) );
//   return(_get_SP_());
// }


int main(void)
{
/* USER CODE BEGIN (3) */

	/*Globally disable R4 IRQs AND FIQs:*/
	_disable_interrupts();

	systemInit();
	muxInit();

	/*
	* Initialize the serial port (SCI module) and enable SCI Receive interrupts:
	* (Explanation: mibSPI peripheral must be initialized as well since SCI requires mibSPI pins)
	*/
	mibspiInit();
	sciInit();
	sciEnableNotification(sciREG, SCI_RX_INT);
	// sciEnableNotification(scilinREG, SCI_RX_INT);
	
	QuadRotor_PWM_init();
	QuadRotor_motor1_start();
	QuadRotor_motor2_start();
	QuadRotor_motor3_start();
	QuadRotor_motor4_start();

	QuadRotor_motor1_setDuty(0.25f);
	QuadRotor_motor2_setDuty(0.50f); 
	QuadRotor_motor3_setDuty(0.75f);
	QuadRotor_motor4_setDuty(0.85f);

	// printf("Ready to execute loop!!\r\n");

	rtiInit();
	rtiEnableNotification(rtiNOTIFICATION_COMPARE0);
	rtiStartCounter(rtiCOUNTER_BLOCK0);

	vimInit();

	canInit();

	_enable_interrupts();

	uint8_t can_msg[8] = {0, 1, 2, 3, 4, 5, 6, '\n'};
	while(1)
	{
		canTransmit(canREG3, canMESSAGE_BOX1, can_msg);
		/*Toggle GPIO line to indicate we're alive, and for timing purposes:*/
		// gioToggleBit(mibspiPORT3, PIN_SIMO);
		insert_delay(500);
	}
/* USER CODE END */
}

/* USER CODE BEGIN (4) */
/* USER CODE END */
	