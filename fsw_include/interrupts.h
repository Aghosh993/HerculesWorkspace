#ifndef INTERRUPTS_H_
#define INTERRUPTS_H_	1

#define SCI1_LOOPBACK	1
#define SCI2_LOOPBACK	1

void rti_callback(void);
void sci1_rx_callback(uint8_t data);
void sci2_rx_callback(uint8_t data);

#endif