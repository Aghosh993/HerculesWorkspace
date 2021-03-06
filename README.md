# HerculesWorkspace

##Overview:
This repo contains the codebase to compile and load C code onto a TI Hercules RM48L MCU-based autopilot board.

For the purposes of this README, the assumption is usage of a Linux or UNIX-like Operating System on the development machine.

##Requirements:

To use this codebase, one must have the "arm-none-eabi-gcc" toolchain 
installed. Go to https://launchpad.net/gcc-arm-embedded/+download to 
grab the correct archive for your platform. Download and unzip to a 
known location. Go to the Makefile in this workspace's root folder, and 
change the TOOLCHAIN_ROOT variable in line 3 to the path to where you 
unzipped the toolchain.

To be able to flash the target board using JTAG, one must have the 
J-Link adapter. an EDU version can be obtained for about $60 from 
Digi-Key. Go to https://www.segger.com/downloads/jlink and download the 
"J-Link Software and Documentation pack" for Linux (get the TGZ 
archive.) unzip the archive and put it somewhere in your home directory 
for convenience. Next, set the JLINK_ROOT variable in line 7 of the Makefile
to the correct location pointing to the unzipped contents of JLink package.

##Usage:
Cloning this repo to your local machine:

git clone https://github.com/Aghosh993/HerculesWorkspace.git

After cloning, change directories into the repo root:

cd HerculesWorkspace/

To build (assuming you've set up the path to your toolchain as mentioned 
in the Requirements section):

make (or make -j5 if you have a multi-core system, to speed things up)

To flash, connect a J-Link debugger to your machine, and plug the JTAG 
cable into the target board. Power up the board if it isn't already. 
Type:

make load

After a few seconds, the firmware should be transferred and ready to run 
after a reset. Enjoy!!
