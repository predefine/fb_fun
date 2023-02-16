#include <stdio.h>
#include <stdint.h>
uint8_t *start = (uint8_t *)0x2a00000;
uint8_t *end = (uint8_t *)0x2abbb00;
int c_entry() {
	for (uint8_t *ptr = start; ptr < end; ptr++) {
    	*ptr = 255;
	}
}
