#ifndef INTERRUPTS_H_
#define INTERRUPTS_H_	1

// #define SCI1_LOOPBACK	1
// #define SCI2_LOOPBACK	1

#include "hal_common_includes.h"
#include "serial_comms_highlevel.h"

void rti_callback(void);
void sci1_tx_callback(void);
void sci1_rx_callback(void);
void sci2_rx_callback(void);

#endif