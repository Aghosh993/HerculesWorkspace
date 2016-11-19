/*Standard HAL (Hardware Abstraction Layer) includes:*/
#include "hal_common_includes.h"
#include "interrupt_utils.h"

// User/flight software libraries:
#include "basic_pid_controller.h"
#include "iir_filters.h"
#include "Quadcopter_PWM_HAL.h"
#include "rt_telemetry.h"
#include "misc_utils.h"

#include <stdio.h>

/* USER CODE END */

serialport ftdi_dbg_port, tm4c_comms_aux_port;
serialport *ftdi_dbg_port_ptr;
serialport *tm4c_comms_aux_port_ptr;

int main(void)
{
/* USER CODE BEGIN (3) */

	/*Globally disable R4 IRQs AND FIQs:*/
	_disable_interrupts();

	systemInit();
	muxInit();

	rtiInit();
	rtiEnableNotification(rtiNOTIFICATION_COMPARE0);
	rtiStartCounter(rtiCOUNTER_BLOCK0);

	vimInit();

	// Initialize all configured serial ports, including HAL subsystems + data structs for asynch tx/rx:

	ftdi_dbg_port_ptr = &ftdi_dbg_port;
	tm4c_comms_aux_port_ptr = &tm4c_comms_aux_port;

	serialport_hal_init();
	serialport_init(ftdi_dbg_port_ptr, PORT1);
	// serialport_init(tm4c_comms_aux_port_ptr, PORT2);

	/*
	* Initialize the serial port (SCI module) and enable SCI Receive interrupts:
	* (Explanation: mibSPI peripheral must be initialized as well since SCI requires mibSPI pins)
	*/
	mibspiInit();
	
	QuadRotor_PWM_init();
	QuadRotor_motor1_start();
	QuadRotor_motor2_start();
	QuadRotor_motor3_start();
	QuadRotor_motor4_start();

	QuadRotor_motor1_setDuty(0.25f);
	QuadRotor_motor2_setDuty(0.50f); 
	QuadRotor_motor3_setDuty(0.75f);
	QuadRotor_motor4_setDuty(0.85f);

	canInit();
	uint8_t can_msg[9] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
	uint32_t can_id_default = canGetID(canREG3, canMESSAGE_BOX1);
	canUpdateID(canREG3, canMESSAGE_BOX1, 0x6000000A);

	can_id_default = canGetID(canREG3, canMESSAGE_BOX2);
	canUpdateID(canREG3, canMESSAGE_BOX2, 0x6000000B);

	can_id_default = canGetID(canREG3, canMESSAGE_BOX3);
	canUpdateID(canREG3, canMESSAGE_BOX3, 0x6000000C);

	can_id_default = canGetID(canREG3, canMESSAGE_BOX10);
	canUpdateID(canREG3, canMESSAGE_BOX10, 0xC00000AB);

	_enable_interrupts();

	serialport_send_data_buffer_blocking(ftdi_dbg_port_ptr, (uint8_t *)"\r\nAP Initialized. Enabled all configured interrupts, proceeding to main loop.\r\n", 79U);

	uint8_t msgbuf[100U];

	while(1)
	{
		// while(!canIsRxMessageArrived(canREG3, canMESSAGE_BOX10))
		// {
		// 	serialport_send_data_buffer_blocking(ftdi_dbg_port_ptr, (uint8_t *)"Business as usual...\r\n", 22);
		// }
		// canGetData(canREG3, canMESSAGE_BOX10, can_msg);
		// if(can_msg[7] == 100U)
		// {
		// 	serialport_send_data_buffer_blocking(ftdi_dbg_port_ptr, (uint8_t *)"Received CAN packet :)\r\n", 24);
		// }
		serialport_receive_data_buffer_blocking(ftdi_dbg_port_ptr, msgbuf, 1);
		serialport_send_data_buffer_blocking(ftdi_dbg_port_ptr, msgbuf, 1);
	}
/* USER CODE END */
}

/* USER CODE BEGIN (4) */
/* USER CODE END */
	