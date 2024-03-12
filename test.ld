ENTRY(_Reset)
SECTIONS
{
 . = 0x80008000;
 .startup . : { startup.o(.text) }
 .text : { *(.text) }
 .data : { *(.data) }
 .bss : { *(.bss COMMON) }
 . = ALIGN(8);
 . = . + 0x10000; /* 4kB of stack memory */
 stack_top = .;
}
