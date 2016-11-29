#ifndef VEHICLE_GNC_H_
#define VEHICLE_GNC_H_		1

#include "imu.h"
#include "iir_filters.h"

#include <stdint.h>
#include <math.h>

// #define SF10_UGV_THRESHOLD	0.18f // in meters, was 0.1f in last flight test
// #define UGV_HEIGHT			0.50f // in meters

// #define RAPID_DESCENT_INITIAL_HEIGHT		0.80f 		// In Meters
// #define RAPID_DESCENT_VELOCITY				0.65f 		// In Meters*s^-1
// #define RAPID_DESCENT_MOTOR_CUTOFF_HEIGHT	0.08f 		// In Meters

// #define VELOCITY_CONTROL 	1

// typedef enum {
// 	STATE_VEHICLE_ABOVE_GROUND,
// 	STATE_TRANSITIONING_TO_UGV,
// 	STATE_VEHICLE_ABOVE_UGV,
// 	STATE_TRANSITIONING_TO_GROUND
// } vehicle_relative_height_tracker;

// void get_vehicle_state(imu_scaled_data_struct *imu_data, filtered_quadrotor_state *st_vector);
// void velocity_controller_update(float vel_cmd_x, float vel_cmd_y, float vel_x, float vel_y, float *roll_cmd, float *pitch_cmd);
// void vehicle_stabilization_outerloop_update(filtered_quadrotor_state *st_vector,
// 													imu_scaled_data_struct *imu_data,
// 													float roll_cmd_in,
// 													float pitch_cmd_in,
// 													float yaw_cmd_in,
// 													float *roll_rate_cmd_out,
// 													float *pitch_rate_cmd_out,
// 													float *yaw_rate_cmd_out);
// void vehicle_stabilization_innerloop_update(imu_scaled_data_struct *imu_data,
// 													float roll_rate_cmd_in,
// 													float pitch_rate_cmd_in,
// 													float yaw_rate_cmd_in,
// 													float throttle_value_in,
// 													double *motor_commands_out);
// void height_kalman_struct_init(height_kalman_data_struct *str, float filter_dt_sec, float r_lidar);
// void height_kalman_update(height_kalman_data_struct *str, float lidar_height_measurement, float accelerometer_z_measurement, float vehicle_roll_deg, float vehicle_pitch_deg);
// float bno055_get_relative_heading(float raw_heading, float prior_heading);
// float height_controller_thrust_offset(float rotor_dia_meters, float height_meters);
// float get_compensated_sf10_data(vehicle_relative_height_tracker *tr, 
// 											float sf10_raw_measurement, float sf10_previous_raw_measurement,
// 											float current_height_cmd, float previous_height_cmd);
// void automated_landing_sequence(float relative_time, float *height_setpoint_output, float actual_height);
// void estimate_lateral_velocity(float* velocity_x_output, float *velocity_y_output, pxflow_flow_data_struct flow_data_input, 
// 								float height_input, float current_roll_degrees, float current_pitch_degrees);
// void px4flow_get_bias(int16_t* bias_x, int16_t* bias_y);

// typedef struct {
// 	/*Filter Constants:*/

// 	/*Time between filter updates in seconds:*/
// 	float filter_dt;
// 	/*Noise characteristic of LIDAR Altimeter:*/
// 	float r_lidar;

// 	/*========================================*/

// 	/*Filter Variables*/

// 	/*Filter state estimate (aka The Money :))*/
// 	float height_estimated;
// 	float vertical_velocity_estimated;

// 	/*Latest sensor data fed into the filter as of the last update cycle:*/
// 	float h_lidar_global_coords;
// 	float a_accel_global_coords;

// 	/*P matrix elements:*/
// 	float p11;
// 	float p12;
// 	float p21;
// 	float p22;

// 	/*G (Kalman Gain) matrix elements:*/
// 	float g11;
// 	float g21;
// } height_kalman_data_struct;

#endif