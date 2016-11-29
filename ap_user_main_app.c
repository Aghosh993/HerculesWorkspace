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
#include "imu.h"
#include "mission_timekeeper.h"
#include "sys_leds.h"

#include <stdio.h>

// #define ENABLE_MOTORS	100

/* USER CODE END */

serialport ftdi_dbg_port, tm4c_comms_aux_port;
serialport *ftdi_dbg_port_ptr;
serialport *tm4c_comms_aux_port_ptr;

rt_telemetry_comm_channel telem0;

int main(void)
{
/* USER CODE BEGIN (3) */

	/*Globally disable R4 IRQs AND FIQs:*/
	_disable_interrupts();

	systemInit();
	muxInit();

	sys_led_init(); // Initialize the MibSPI in GPIO mode to be able to use system LEDs on interface board.

	/*
		Set up 1 kHz RTI interrupt:
	 */
	rtiInit();
	rtiEnableNotification(rtiNOTIFICATION_COMPARE0);
	rtiStartCounter(rtiCOUNTER_BLOCK0);

	/*
		Set up Halcogen-derived vector table and priorities:
	 */
	vimInit();

	// Initialize all configured serial ports, including HAL subsystems + data structs for asynch tx/rx:

	ftdi_dbg_port_ptr = &ftdi_dbg_port;
	tm4c_comms_aux_port_ptr = &tm4c_comms_aux_port;

	serialport_hal_init();
	serialport_init(ftdi_dbg_port_ptr, PORT1);
	rt_telemetry_init_channel(&telem0, ftdi_dbg_port_ptr);

	/*
	* Initialize the serial port (SCI module) and enable SCI Receive interrupts:
	* (Explanation: mibSPI peripheral must be initialized as well since SCI requires mibSPI pins)
	*/	
	
	QuadRotor_PWM_init();
	QuadRotor_motor1_start();
	QuadRotor_motor2_start();
	QuadRotor_motor3_start();
	QuadRotor_motor4_start();

	/*
		All motors stopped by default:
	 */
	QuadRotor_motor1_setDuty(0.0f);
	QuadRotor_motor2_setDuty(0.0f); 
	QuadRotor_motor3_setDuty(0.0f);
	QuadRotor_motor4_setDuty(0.0f);

	/*
		Initialize the PWM input functionality. Technically this just makes a duplicate call to hetInit and zeroes the
		edge counter that's used for LOS (Loss of Signal) detection functionality.
	 */
	pwm_input_init();

	/*
		CAN Bus initialization for communications to the TM4C:
	 */

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

	imu_hal_init();
	init_mission_timekeeper();

	_enable_interrupts();

	imu_raw_data_struct mpu_data;
	int32_t imudata_telemetry_output[7];

	uint8_t telemetry_250hz_flag = create_flag(3U);
	uint8_t rc_update_50hz_flag = create_flag(19U);
	uint8_t imu_sample_1000hz_flag = create_flag(0U);
	uint8_t rc_watchdog_10hz_flag = create_flag(99U);
	uint8_t heartbeat_1hz_flag = create_flag(999U);
	
	sys_ledOn(SYS_LED1);
	sys_ledOff(SYS_LED2);

	timekeeper_delay(5000U);

	rc_joystick_data_struct rc_data;
	init_rc_inputs(&rc_data);

	while(1)
	{
		if(get_flag_state(heartbeat_1hz_flag) == STATE_PENDING)
		{
			reset_flag(heartbeat_1hz_flag);
			sys_ledToggle(SYS_LED2); // Toggle green LED on interface board as heartbeat
		}

		if(get_flag_state(imu_sample_1000hz_flag) == STATE_PENDING)
		{
			reset_flag(imu_sample_1000hz_flag);

			if(get_flag_state(rc_watchdog_10hz_flag) == STATE_PENDING)
			{
				reset_flag(rc_watchdog_10hz_flag);
				rc_input_validity_watchdog_callback();
			}
			
			if(get_raw_imu_data(&mpu_data)==0)
			{
				imudata_telemetry_output[0] = (int32_t)mpu_data.accel_data[0];
				imudata_telemetry_output[1] = (int32_t)mpu_data.accel_data[1];
				imudata_telemetry_output[2] = (int32_t)mpu_data.accel_data[2];

				imudata_telemetry_output[3] = (int32_t)mpu_data.temp_sensor_data;

				imudata_telemetry_output[4] = (int32_t)mpu_data.gyro_data[0];
				imudata_telemetry_output[5] = (int32_t)mpu_data.gyro_data[1];
				imudata_telemetry_output[6] = (int32_t)mpu_data.gyro_data[2];

				sys_ledToggle(SYS_LED1);
			}
			
			if(get_flag_state(telemetry_250hz_flag) == STATE_PENDING)
			{
				reset_flag(telemetry_250hz_flag);
				send_telem_msg_n_ints_blocking(&telem0, (uint8_t *)"mpudata", 7, imudata_telemetry_output, 7);

				if(get_flag_state(rc_update_50hz_flag) == STATE_PENDING)
				{
					reset_flag(rc_update_50hz_flag);
					get_rc_input_values(&rc_data);
					if(rc_data.mode_switch_channel_validity == CHANNEL_VALID)
					{
						send_telem_msg_string_blocking(&telem0, (uint8_t *)"rcvalid", 7, "TRUE\r\n", 6);
						send_telem_msg_n_floats_blocking(&telem0, (uint8_t *)"roll", 4, &(rc_data.roll_channel_value), 1);
						send_telem_msg_n_floats_blocking(&telem0, (uint8_t *)"pitch", 5, &(rc_data.pitch_channel_value), 1);
						send_telem_msg_n_floats_blocking(&telem0, (uint8_t *)"yaw", 3, &(rc_data.yaw_channel_value), 1);
						send_telem_msg_n_floats_blocking(&telem0, (uint8_t *)"height", 6, &(rc_data.vertical_channel_value), 1);

						if(get_ch5_mode(rc_data) == MODE_NORMAL)
						{
							send_telem_msg_string_blocking(&telem0, (uint8_t *)"CH5", 3, (uint8_t *)"Normal", 6);
						}
						if(get_ch5_mode(rc_data) == MODE_FAILSAFE)
						{
							send_telem_msg_string_blocking(&telem0, (uint8_t *)"CH5", 3, (uint8_t *)"FAILSAFE", 8);
						}
					}
					else
					{
						/*
							Emergency stop all motors upon loss of signal. In the future this may become something a bit more graceful...
						 */
						send_telem_msg_string_blocking(&telem0, (uint8_t *)"rcvalid", 7, "FALSE\r\n", 7);
						QuadRotor_motor1_setDuty(0.05f);
						QuadRotor_motor2_setDuty(0.05f);
						QuadRotor_motor3_setDuty(0.05f);
						QuadRotor_motor4_setDuty(0.05f);
						// Wait for serial transmit of telemetry to complete, and give ESCs a chance to latch the last valid command prior to PWM shutdown.
						insert_delay(100);
						_disable_interrupts();
						QuadRotor_motor1_stop();
						QuadRotor_motor2_stop();
						QuadRotor_motor3_stop();
						QuadRotor_motor4_stop();
						while(1);
					}
				}
			}
		}
		/*
			Serial port library and CAN functionality tests:
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
	