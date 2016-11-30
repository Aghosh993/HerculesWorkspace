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
#include "telem_config.h"
#include "vehicle_gnc.h"

#include <stdio.h>

// #define ENABLE_MOTORS	1

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

	init_mission_timekeeper();

	_enable_interrupts();

	float imudata_telemetry_output[7];

	uint8_t telemetry_gnc_250hz_flag = create_flag(3U);
	uint8_t gnc_125Hz_flag = create_flag(7U);
	uint8_t rc_update_50hz_flag = create_flag(19U);
	uint8_t imu_sample_1000hz_flag = create_flag(0U);
	uint8_t rc_watchdog_10hz_flag = create_flag(99U);
	uint8_t heartbeat_1hz_flag = create_flag(999U);
	
	sys_ledOn(SYS_LED1);
	sys_ledOff(SYS_LED2);

	gnc_init();
	gnc_raw_data rd;
	gnc_state_data sd;

	sys_ledOff(SYS_LED1);
	timekeeper_delay(1000U);

	float att_data[3];
	att_data[2] = 0.0f;

	float sensor_data[6];

	float roll_cmd, pitch_cmd, yaw_cmd;
	float roll_rate_cmd, pitch_rate_cmd, yaw_rate_cmd;
	float throttle_value_common;
	double motor_output_commands[4];

	roll_cmd = 0.0f;
	pitch_cmd = 0.0f;
	yaw_cmd = 0.0f;

	throttle_value_common = 0.0f;

	motor_output_commands[0] = 0.0f;
	motor_output_commands[1] = 0.0f;
	motor_output_commands[2] = 0.0f;
	motor_output_commands[3] = 0.0f;

	float motor_vals[4];

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
			
			gnc_get_vehicle_state();
			gnc_get_raw_sensor_data(&rd);
			gnc_get_state_vector_data(&sd);
			att_data[0] = sd.roll;
			att_data[1] = sd.pitch;

			sensor_data[0] = rd.x_accel;
			sensor_data[1] = rd.y_accel;
			sensor_data[2] = rd.z_accel;

			sensor_data[3] = rd.roll_gyro;
			sensor_data[4] = rd.pitch_gyro;
			sensor_data[5] = rd.yaw_gyro;

			sys_ledToggle(SYS_LED1);
			
			if(get_flag_state(telemetry_gnc_250hz_flag) == STATE_PENDING)
			{
				// Reset flag for next cycle:				
				reset_flag(telemetry_gnc_250hz_flag);

				// Send telemetry items as configured:
				#ifdef SEND_MPU_IMU_TELEMETRY
					send_telem_msg_n_floats_blocking(&telem0, (uint8_t *)"state", 5, att_data, 2);
					send_telem_msg_n_floats_blocking(&telem0, (uint8_t *)"imu", 3, sensor_data, 6);
				#endif

				// Run GNC angular outer loop control to generate rate commands for inner loop:
				if(get_flag_state(gnc_125Hz_flag) == STATE_PENDING)
				{
					reset_flag(gnc_125Hz_flag);
					gnc_vehicle_stabilization_outerloop_update(roll_cmd, pitch_cmd, yaw_cmd,
																&roll_rate_cmd, &pitch_rate_cmd, &yaw_rate_cmd);
				}

				gnc_vehicle_stabilization_innerloop_update(roll_rate_cmd, pitch_rate_cmd, yaw_rate_cmd,
															throttle_value_common,
															motor_output_commands);

				motor_vals[0] = (float)motor_output_commands[0];
				motor_vals[1] = (float)motor_output_commands[1];
				motor_vals[2] = (float)motor_output_commands[2];
				motor_vals[3] = (float)motor_output_commands[3];

				send_telem_msg_n_floats_blocking(&telem0, (uint8_t *)"cmds", 4, motor_vals, 4);

				QuadRotor_motor1_setDuty((float)motor_output_commands[0]);
				QuadRotor_motor2_setDuty((float)motor_output_commands[1]);
				QuadRotor_motor3_setDuty((float)motor_output_commands[2]);
				QuadRotor_motor4_setDuty((float)motor_output_commands[3]);

				if(get_flag_state(rc_update_50hz_flag) == STATE_PENDING)
				{
					reset_flag(rc_update_50hz_flag);
					get_rc_input_values(&rc_data);
					if(rc_data.mode_switch_channel_validity == CHANNEL_VALID)
					{
						#ifdef SEND_RC_INPUT_TELEMETRY
							// send_telem_msg_string_blocking(&telem0, (uint8_t *)"rcvalid", 7, "TRUE\r\n", 6);
							// send_telem_msg_n_floats_blocking(&telem0, (uint8_t *)"roll", 4, &(rc_data.roll_channel_value), 1);
							// send_telem_msg_n_floats_blocking(&telem0, (uint8_t *)"pitch", 5, &(rc_data.pitch_channel_value), 1);
							// send_telem_msg_n_floats_blocking(&telem0, (uint8_t *)"yaw", 3, &(rc_data.yaw_channel_value), 1);
							// send_telem_msg_n_floats_blocking(&telem0, (uint8_t *)"height", 6, &(rc_data.vertical_channel_value), 1);

							if(get_ch5_mode(rc_data) == MODE_NORMAL)
							{
								send_telem_msg_string_blocking(&telem0, (uint8_t *)"CH5", 3, (uint8_t *)"Normal", 6);
							}
							if(get_ch5_mode(rc_data) == MODE_FAILSAFE)
							{
								send_telem_msg_string_blocking(&telem0, (uint8_t *)"CH5", 3, (uint8_t *)"FAILSAFE", 8);
							}
						#endif
						roll_cmd = rc_data.roll_channel_value;
						pitch_cmd = rc_data.pitch_channel_value;
						yaw_cmd = rc_data.yaw_channel_value * -1.0f;
						throttle_value_common = 0.50f*(rc_data.vertical_channel_value + 1.0f);
					}
					else
					{
						/*
							Emergency-stop all motors upon loss of signal. In the future this may become something a bit more graceful...
						 */
						send_telem_msg_string_blocking(&telem0, (uint8_t *)"rcvalid", 7, "FALSE\r\n", 7);
						QuadRotor_motor1_setDuty(0.0f);
						QuadRotor_motor2_setDuty(0.0f);
						QuadRotor_motor3_setDuty(0.0f);
						QuadRotor_motor4_setDuty(0.0f);
						// Wait for serial transmit of telemetry to complete, and give ESCs a chance to latch the last valid command prior to PWM shutdown.
						insert_delay(100);
						_disable_interrupts();
						QuadRotor_motor1_stop();
						QuadRotor_motor2_stop();
						QuadRotor_motor3_stop();
						QuadRotor_motor4_stop();
						sys_ledOn(SYS_LED1); // Red ERROR LED = ON
						sys_ledOff(SYS_LED2); // Green OK LED = OFF
						while(1); // Block here indefinitely pending system power-off/reset by user
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
	