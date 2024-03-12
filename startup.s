.global _Reset
_Reset:
 LDR sp, =stack_top
 BL c_entry
 B .
nop
nop
nop
nop
nop
nop
nop
// should be setted by fix_kernel.py
kernel_size:
.long 0,0
