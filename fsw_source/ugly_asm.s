/* THIS IS UGLY STUFF 															*/
    .section .text 
    .syntax unified
    .cpu cortex-r4
    .arm 

/*-------------------------------------------------------------------------------*/
    .weak _get_SP_
    .type _get_SP_, %function

_get_SP_:
        mov   r0, SP
        bx    lr
