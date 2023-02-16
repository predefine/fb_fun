.global _Reset
_Reset:
 LDR sp, =stack_top
 MRC p15, 0, r0, c1, c0, 1
 BIC r0, r0, #0x40
 BIC r0, r0, #0x200000
 MCR p15, 0, r0, c1, c0, 1
 ldr     r4, =0xa9000864        @ bank6_in (phys)
 ldr     r5, =0xa9000814        @ bank6_out (phys)
 orr     r6, r4, #0x200000      @ 22nd bit for flash
 str     r6, [r5, #0]           @ store in out (enables bright for 500ms, limited by hardware)
 BL c_entry
 B .
