#!/bin/bash
../../../JLink_Linux_V480_x86_64/JLinkGDBServer -Device RM48L9x &
/usr/bin/arm-none-eabi-gdb --batch --command=runme.gdb
pkill JLinkGDBServer