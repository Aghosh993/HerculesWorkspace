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
	rt_telemetry_comm_channel telem0;

	serialport_hal_init();
	serialport_init(ftdi_dbg_port_ptr, PORT1);
	rt_telemetry_init_channel(&telem0, ftdi_dbg_port_ptr);
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

	// serialport_send_data_buffer_blocking(ftdi_dbg_port_ptr, (uint8_t *)"\r\nAP Initialized. Enabled all configured interrupts, proceeding to main loop.\r\n", 79U);

	// uint8_t msgbuf[100U];

	// uint8_t *testmsg = (uint8_t *)"Hello!!!\r\n";
	// uint8_t *testmsg1 = (uint8_t *)"Alive and well\r\n";
	// send_telem_msg_string_blocking(&telem0, "Test_msg0", 9, testmsg, 10U);

	float arr[3];
	arr[0] = 2.5f;
	arr[1] = -3.14159f;
	arr[2] = 4.5f;

	int arr2[3];
	arr2[0] = -42;
	arr2[1] = 9001;
	arr2[2] = -3;
	// send_telem_msg_n_ints_blocking(&telem0, (uint8_t *)"stuff1", 6, arr2, 3U);
	send_telem_msg_n_floats_blocking(&telem0, (uint8_t *)"coords", 6, arr, 3U);

	while(1)
	{
		/*
			Firmware functionality tests
		 */
		// send_telem_msg_string_blocking(&telem0, "Test_msg1", 9, testmsg1, 16U);		
		// while(!canIsRxMessageArrived(canREG3, canMESSAGE_BOX10))
		// {
		// 	serialport_send_data_buffer_blocking(ftdi_dbg_port_ptr, (uint8_t *)"Business as usual...\r\n", 22);
		// }
		// canGetData(canREG3, canMESSAGE_BOX10, can_msg);
		// if(can_msg[7] == 100U)
		// {
		// 	serialport_send_data_buffer_blocking(ftdi_dbg_port_ptr, (uint8_t *)"Received CAN packet :)\r\n", 24);
		// }

		// Simple polling echo:
		// serialport_receive_data_buffer_blocking(ftdi_dbg_port_ptr, msgbuf, 1);
		// serialport_send_data_buffer_blocking(ftdi_dbg_port_ptr, msgbuf, 1);

		// Simple asynch echo:
		// int bytes_read = serialport_receive_data_buffer(ftdi_dbg_port_ptr, msgbuf, 100); // Attempt to read 1 byte
		// if(bytes_read > 0) // If bytes_read is nonzero, we have a byte!
		// {
		// 	serialport_send_data_buffer(ftdi_dbg_port_ptr, msgbuf, bytes_read);		
		// }
	}
/* USER CODE END */
}

/* USER CODE BEGIN (4) */
/* USER CODE END */
	