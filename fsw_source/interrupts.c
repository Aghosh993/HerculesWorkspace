#include "hal_common_includes.h"

#include "interrupts.h"

void rti_callback(void)
{
	gioToggleBit(mibspiPORT3, PIN_SIMO);
}

void sci1_rx_callback(uint8_t data)
{
	#ifdef SCI1_LOOPBACK
		sciSendByte(sciREG, data);
	#endif
}

void sci2_rx_callback(uint8_t data)
{
	#ifdef SCI2_LOOPBACK
		sciSendByte(scilinREG, data);
	#endif
}