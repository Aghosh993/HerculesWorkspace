/*
	File: misc_utils.h

	(c) Abhimanyu Ghosh, 2016
 */

#ifndef MISC_UTILS_H_
#define MISC_UTILS_H_	1

#include "stdint.h"
#include "hal_common_includes.h"

void insert_delay(int ms);
void blocking_sci_SendData(uint8_t *dataptr, uint8_t len);

#endif