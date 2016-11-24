/*Standard HAL (Hardware Abstraction Layer) includes:*/
#include "hal_common_includes.h"
#include "interrupt_utils.h"

// User/flight software libraries:
#include "basic_pid_controller.h"
#include "iir_filters.h"
#include "Quadcopter_PWM_HAL.h"
#include "rt_telemetry.h"
#include "misc_utils.h"
#include "pwm_input.h"

#include <stdio.h>

/* USER CODE END */

serialport ftdi_dbg_port, tm4c_comms_aux_port;
serialport *ftdi_dbg_port_ptr;
serialport *tm4c_comms_aux_port_ptr;

// float arr3[9000];

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

	// uint8_t *testmsg1 = (uint8_t *)"Alive and well\r\n";

	// int32_t arr1[3];
	// arr1[0] = -42;
	// arr1[1] = 9001;
	// arr1[2] = -3;

	// float arr2[3];
	// arr2[0] = 2.5f;
	// arr2[1] = -3.14159f;
	// arr2[2] = 4.5f;

	// float arr3[9];
	// arr3[0] = 10.0f;
	// arr3[1] = -1.1f;
	// arr3[2] = 2.0f;
	// arr3[3] = 3.0f;
	// arr3[4] = -5.0f;
	// arr3[5] = 10.0f;
	// arr3[6] = 2.5f;
	// arr3[7] = -1.25f;
	// arr3[8] = 1.4f;

	// int32_t arr4[10];
	// arr4[0] = -5;
	// arr4[1] = 10;
	// arr4[2] = 15;
	// arr4[3] = 50;
	// arr4[4] = -100;
	// arr4[5] = 500;
	// arr4[6] = 15;
	// arr4[7] = 20;
	// arr4[8] = -2;
	// arr4[9] = 125;

	// send_telem_msg_string_blocking(&telem0, "msg0", 4, testmsg1, 16U);
	// send_telem_msg_n_ints_blocking(&telem0, (uint8_t *)"stuff1", 6, arr1, 3U);
	// send_telem_msg_n_floats_blocking(&telem0, (uint8_t *)"coords", 6, arr2, 3U);
	// send_telem_msg_m_n_float_matrix_blocking(&telem0, (uint8_t *)"mat_flt", 7, arr3, 3U, 3U);
	// send_telem_msg_m_n_int_matrix_blocking(&telem0, (uint8_t *)"mat_int", 7, arr4, 3U, 3U);

	pwm_info_t rc_cap0;
	pwm_info_t rc_cap1;
	pwm_info_t rc_cap2;
	pwm_info_t rc_cap3;
	pwm_info_t rc_cap4;

	int32_t duty[5];

	while(1)
	{
		// capGetSignal(hetRAM1, cap0, &rc_cap);
		pwmGetSignalHigherPrecision(hetRAM1, cap0, &rc_cap0);
		pwmGetSignalHigherPrecision(hetRAM1, cap1, &rc_cap1);
		pwmGetSignalHigherPrecision(hetRAM2, cap0, &rc_cap2);
		pwmGetSignalHigherPrecision(hetRAM2, cap1, &rc_cap3);
		pwmGetSignalHigherPrecision(hetRAM1, cap2, &rc_cap4);

		duty[0] = (int32_t)rc_cap0.duty_us;
		duty[1] = (int32_t)rc_cap1.duty_us;
		duty[2] = (int32_t)rc_cap2.duty_us;
		duty[3] = (int32_t)rc_cap3.duty_us;
		duty[4] = (int32_t)rc_cap4.duty_us;

		send_telem_msg_n_ints_blocking(&telem0, (uint8_t *)"rc_pwm", 6, &duty, 5);
		insert_delay(100);
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
		// int bytes_read = serialport_receive_data_buffer(ftdi_dbg_port_ptr, msgbuf, 100); // Attempt to read 100 bytes
		// if(bytes_read > 0) // If bytes_read is nonzero, we have a byte!
		// {
		// 	serialport_send_data_buffer(ftdi_dbg_port_ptr, msgbuf, bytes_read);		
		// }
	}
/* USER CODE END */
}

/* USER CODE BEGIN (4) */
/* USER CODE END */
	