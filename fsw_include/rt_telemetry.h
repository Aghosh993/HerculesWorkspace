/*
	File: rt_telemetry.h

	(c) Abhimanyu Ghosh, 2016
 */

#ifndef RT_TELEMETRY_H_
#define RT_TELEMETRY_H_	1

#include "message_defs.h"
#include "serial_comms_highlevel.h"

#define MAX_TELEM_MSG_TX_QUEUE_DEPTH	10U
#define MAX_TELEM_MSG_RX_QUEUE_DEPTH	10U

#define TELEM_PACKET_CHECKSUM_OFFSET	0x75

typedef struct {
	uint8_t start_byte;
	uint8_t message_buffer[MAX_MSG_PAYLOAD_SIZE];
	uint16_t checksum;
} rt_telemetry_msg;

typedef struct {
	serialport *comm_port_ptr;
	
	rt_telemetry_msg tx_message_buffer[MAX_TELEM_MSG_TX_QUEUE_DEPTH];
	rt_telemetry_msg rx_message_buffer[MAX_TELEM_MSG_RX_QUEUE_DEPTH];

	uint32_t tx_buffer_latest_write_position;
	uint32_t rx_buffer_latest_write_position;

	uint32_t tx_buffer_latest_read_position;
	uint32_t rx_buffer_latest_read_position;
} rt_telemetry_comm_channel;

void rt_telemetry_init_channel(rt_telemetry_comm_channel *ch, serialport *comm_port);

void send_telem_msg_string(rt_telemetry_comm_channel *ch, uint8_t *descriptor_string, uint8_t descriptor_string_len, uint8_t *string_to_transmit, uint8_t string_len);
// void send_telem_msg_n_floats(rt_telemetry_comm_channel *ch, uint8_t *descriptor_string, uint8_t descriptor_string_len, float *data_payload, uint8_t n_floats_len);
// void send_telem_msg_n_ints(rt_telemetry_comm_channel *ch, uint8_t *descriptor_string, uint8_t descriptor_string_len, int32_t *data_payload, uint8_t n_ints_len);
// void send_telem_msg_m_n_float_matrix(rt_telemetry_comm_channel *ch, uint8_t *descriptor_string, uint8_t descriptor_string_len, float *data_payload, uint8_t m, uint8_t n);
// void send_telem_msg_m_n_int_matrix(rt_telemetry_comm_channel *ch, uint8_t *descriptor_string, uint8_t descriptor_string_len, int32_t *data_payload, uint8_t m, uint8_t n);

void send_telem_msg_string_blocking(rt_telemetry_comm_channel *ch, uint8_t *descriptor_string, uint8_t descriptor_string_len, uint8_t *string_to_transmit, uint8_t string_len);
void send_telem_msg_n_floats_blocking(rt_telemetry_comm_channel *ch, uint8_t *descriptor_string, uint8_t descriptor_string_len, float *data_payload, uint8_t n_floats_len);
void send_telem_msg_n_ints_blocking(rt_telemetry_comm_channel *ch, uint8_t *descriptor_string, uint8_t descriptor_string_len, int32_t *data_payload, uint8_t n_ints_len);
void send_telem_msg_m_n_float_matrix_blocking(rt_telemetry_comm_channel *ch, uint8_t *descriptor_string, uint8_t descriptor_string_len, float *data_payload, uint8_t m, uint8_t n);
void send_telem_msg_m_n_int_matrix_blocking(rt_telemetry_comm_channel *ch, uint8_t *descriptor_string, uint8_t descriptor_string_len, int32_t *data_payload, uint8_t m, uint8_t n);

#endif