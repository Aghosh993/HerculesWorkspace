#include "interrupts.h"

extern serialport ftdi_dbg_port, tm4c_comms_aux_port;
extern serialport *ftdi_dbg_port_ptr;
extern serialport *tm4c_comms_aux_port_ptr;

void rti_callback(void)
{
	gioToggleBit(mibspiPORT3, PIN_SIMO);
	
	static uint8_t can_msg[8] = {0, 1, 2, 3, 4, 5, 90, 7};
	canTransmit(canREG3, canMESSAGE_BOX1, can_msg);
	++can_msg[0];
	canTransmit(canREG3, canMESSAGE_BOX2, can_msg);
	++can_msg[1];
	canTransmit(canREG3, canMESSAGE_BOX3, can_msg);
	++can_msg[2];
}

void sci1_tx_callback(void)
{
	serialport_highlevel_tx_isr(ftdi_dbg_port_ptr);
}

void sci1_rx_callback(void)
{
	#ifdef SCI1_LOOPBACK
		uint8_t data = sciReceiveByte(sciREG);
		sciSendByte(sciREG, data);
	#endif
	#ifndef SCI1_LOOPBACK
		serialport_highlevel_rx_isr(ftdi_dbg_port_ptr);
	#endif
}

void sci2_tx_callback(void)
{
	serialport_highlevel_tx_isr(tm4c_comms_aux_port_ptr);
}

void sci2_rx_callback(void)
{
	#ifdef SCI2_LOOPBACK
		uint8_t data = sciReceiveByte(scilinREG);
		sciSendByte(scilinREG, data);
	#endif
	#ifndef SCI2_LOOPBACK
		serialport_highlevel_rx_isr(tm4c_comms_aux_port_ptr);
	#endif
}