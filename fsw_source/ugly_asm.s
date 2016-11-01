/* THIS IS UGLY STUFF 															*/
    .section .text 
    .syntax unified
    .cpu cortex-r4
    .arm 

/*-------------------------------------------------------------------------------*/

.globl _get_SP_
_get_SP_:
	mov r0, sp
	bx lr
