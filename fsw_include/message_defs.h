#ifndef MESSAGE_DEFS_H_
#define MESSAGE_DEFS_H_		1

#include <stdint.h>

#define MAX_MSG_PAYLOAD_SIZE			60U

#define DESCRIPTOR_STRING_MAX_LEN		9U

#define	TELEM_MSG_ID_STRING				0
#define	TELEM_MSG_ID_N_FLOATS			1
#define	TELEM_MSG_ID_N_INTS				2
#define	TELEM_MSG_ID_M_N_FLOAT_MATRIX	3
#define	TELEM_MSG_ID_M_N_INT_MATRIX		4

typedef struct {
	uint8_t	descriptor_string_len;
	uint8_t descriptor_string[DESCRIPTOR_STRING_MAX_LEN];
	uint8_t msg_id;
	uint8_t string_len;
	uint8_t string_to_transmit[(MAX_MSG_PAYLOAD_SIZE - DESCRIPTOR_STRING_MAX_LEN - 3U)/sizeof(uint8_t)];
} telem_msg_string;

typedef struct {
	uint8_t	descriptor_string_len;
	uint8_t descriptor_string[DESCRIPTOR_STRING_MAX_LEN];
	uint8_t msg_id;
	uint8_t n_floats_len;
	union{
		float data_payload[(MAX_MSG_PAYLOAD_SIZE - DESCRIPTOR_STRING_MAX_LEN - 3U)/sizeof(float)];
		uint8_t data_payload_uint8_t[MAX_MSG_PAYLOAD_SIZE - DESCRIPTOR_STRING_MAX_LEN - 3U];
	} data_payload_convert;
} telem_msg_n_floats;

typedef struct {
	uint8_t	descriptor_string_len;
	uint8_t descriptor_string[DESCRIPTOR_STRING_MAX_LEN];
	uint8_t msg_id;
	uint8_t n_ints_len;
	union{
		int32_t data_payload[(MAX_MSG_PAYLOAD_SIZE - DESCRIPTOR_STRING_MAX_LEN - 3U)/sizeof(int32_t)];
		uint8_t data_payload_uint8_t[MAX_MSG_PAYLOAD_SIZE - DESCRIPTOR_STRING_MAX_LEN - 3U];
	} data_payload_convert;
} telem_msg_n_ints;

typedef struct {
	uint8_t	descriptor_string_len;
	uint8_t descriptor_string[DESCRIPTOR_STRING_MAX_LEN];
	uint8_t msg_id;
	uint8_t m;
	uint8_t n;
	float data_payload[(MAX_MSG_PAYLOAD_SIZE - DESCRIPTOR_STRING_MAX_LEN - 4U)/sizeof(float)];
} telem_msg_m_n_float_matrix;

typedef struct {
	uint8_t	descriptor_string_len;
	uint8_t descriptor_string[DESCRIPTOR_STRING_MAX_LEN];
	uint8_t msg_id;
	uint8_t m;
	uint8_t n;
	int32_t data_payload[(MAX_MSG_PAYLOAD_SIZE - DESCRIPTOR_STRING_MAX_LEN - 4U)/sizeof(float)];
} telem_msg_m_n_int_matrix;

void create_telem_msg_string(telem_msg_string *dst, uint8_t *descriptor_string, uint8_t descriptor_string_len, uint8_t *string_to_transmit, uint8_t string_len);
void create_telem_msg_n_floats(telem_msg_n_floats *dst, uint8_t *descriptor_string, uint8_t descriptor_string_len, float *data_payload, uint8_t n_floats_len);
void create_telem_msg_n_ints(telem_msg_n_ints *dst, uint8_t *descriptor_string, uint8_t descriptor_string_len, int32_t *data_payload, uint8_t n_ints_len);
void create_telem_msg_m_n_float_matrix(telem_msg_m_n_float_matrix *dst, uint8_t *descriptor_string, uint8_t descriptor_string_len, float *data_payload, uint8_t m, uint8_t n);
void create_telem_msg_m_n_int_matrix(telem_msg_m_n_int_matrix *dst, uint8_t *descriptor_string, uint8_t descriptor_string_len, int32_t *data_payload, uint8_t m, uint8_t n);

#endif