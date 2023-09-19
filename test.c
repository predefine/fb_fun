/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2022, Ivaylo Ivanov <ivo.ivanov.ivanov1@gmail.com>
 * Copyright (c) 2022, Markuss Broks <markuss.broks@gmail.com>
 * Copyright (c) 2022, Michael Srba <Michael.Srba@seznam.cz>
 * Copyright (c) 2023, Leandro Friedrich <email@leandrofriedrich.de>
 */

#include <stdint.h>
#include "simplefb.h"
#include "font.h"
#include "debug.h"

#define MSM_UART1_PHYS        0xA9A00000
#define MSM_UART1_SIZE        SZ_4K


#define UART_MR1			0x0000

#define UART_MR1_AUTO_RFR_LEVEL0	0x3F
#define UART_MR1_AUTO_RFR_LEVEL1	0x3FF00
#define UART_MR1_RX_RDY_CTL    		(1 << 7)
#define UART_MR1_CTS_CTL       		(1 << 6)

#define UART_MR2			0x0004
#define UART_MR2_ERROR_MODE		(1 << 6)
#define UART_MR2_BITS_PER_CHAR		0x30
#define UART_MR2_BITS_PER_CHAR_5	(0x0 << 4)
#define UART_MR2_BITS_PER_CHAR_6	(0x1 << 4)
#define UART_MR2_BITS_PER_CHAR_7	(0x2 << 4)
#define UART_MR2_BITS_PER_CHAR_8	(0x3 << 4)
#define UART_MR2_STOP_BIT_LEN_ONE	(0x1 << 2)
#define UART_MR2_STOP_BIT_LEN_TWO	(0x3 << 2)
#define UART_MR2_PARITY_MODE_NONE	0x0
#define UART_MR2_PARITY_MODE_ODD	0x1
#define UART_MR2_PARITY_MODE_EVEN	0x2
#define UART_MR2_PARITY_MODE_SPACE	0x3
#define UART_MR2_PARITY_MODE		0x3

#define UART_CSR	0x0008
#define UART_CSR_115200	0xFF
#define UART_CSR_57600	0xEE
#define UART_CSR_38400	0xDD
#define UART_CSR_28800	0xCC
#define UART_CSR_19200	0xBB
#define UART_CSR_14400	0xAA
#define UART_CSR_9600	0x99
#define UART_CSR_4800	0x77
#define UART_CSR_2400	0x55
#define UART_CSR_1200	0x44
#define UART_CSR_600	0x33
#define UART_CSR_300	0x22

#define UART_TF		0x000C

#define UART_CR				0x0010
#define UART_CR_CMD_NULL		(0 << 4)
#define UART_CR_CMD_RESET_RX		(1 << 4)
#define UART_CR_CMD_RESET_TX		(2 << 4)
#define UART_CR_CMD_RESET_ERR		(3 << 4)
#define UART_CR_CMD_RESET_BREAK_INT	(4 << 4)
#define UART_CR_CMD_START_BREAK		(5 << 4)
#define UART_CR_CMD_STOP_BREAK		(6 << 4)
#define UART_CR_CMD_RESET_CTS		(7 << 4)
#define UART_CR_CMD_PACKET_MODE		(9 << 4)
#define UART_CR_CMD_MODE_RESET		(12 << 4)
#define UART_CR_CMD_SET_RFR		(13 << 4)
#define UART_CR_CMD_RESET_RFR		(14 << 4)
#define UART_CR_TX_DISABLE		(1 << 3)
#define UART_CR_TX_ENABLE		(1 << 3)
#define UART_CR_RX_DISABLE		(1 << 3)
#define UART_CR_RX_ENABLE		(1 << 3)

#define UART_IMR		0x0014
#define UART_IMR_TXLEV		(1 << 0)
#define UART_IMR_RXSTALE	(1 << 3)
#define UART_IMR_RXLEV		(1 << 4)
#define UART_IMR_DELTA_CTS	(1 << 5)
#define UART_IMR_CURRENT_CTS	(1 << 6)

#define UART_IPR_RXSTALE_LAST		0x20
#define UART_IPR_STALE_LSB		0x1F
#define UART_IPR_STALE_TIMEOUT_MSB	0x3FF80

#define UART_IPR	0x0018
#define UART_TFWR	0x001C
#define UART_RFWR	0x0020
#define UART_HCR	0x0024

#define UART_MREG		0x0028
#define UART_NREG		0x002C
#define UART_DREG		0x0030
#define UART_MNDREG		0x0034
#define UART_IRDA		0x0038
#define UART_MISR_MODE		0x0040
#define UART_MISR_RESET		0x0044
#define UART_MISR_EXPORT	0x0048
#define UART_MISR_VAL		0x004C
#define UART_TEST_CTRL		0x0050

#define UART_SR			0x0008
#define UART_SR_HUNT_CHAR	(1 << 7)
#define UART_SR_RX_BREAK	(1 << 6)
#define UART_SR_PAR_FRAME_ERR	(1 << 5)
#define UART_SR_OVERRUN		(1 << 4)
#define UART_SR_TX_EMPTY	(1 << 3)
#define UART_SR_TX_READY	(1 << 2)
#define UART_SR_RX_FULL		(1 << 1)
#define UART_SR_RX_READY	(1 << 0)

#define UART_RF		0x000C
#define UART_MISR	0x0010
#define UART_ISR	0x0014

volatile void *debug_port_base = MSM_UART1_PHYS;

void clean_fb(volatile char *fb, char *text, int width, int height, int stride) {
        for (volatile char *addr = fb; addr < fb + (width * height * stride); addr += stride)
                *(int*) (addr) = 0x0;
}

void draw_pixel(volatile char *fb, int x, int y, int width, int stride) {
        long int location = (x * stride) + (y * width * stride);

        *(fb + location) = 255; // Blue
        *(fb + location + 1) = 255;     // Green
        *(fb + location + 2) = 255;     // Red
        *(fb + location + 3) = 255;     // Full opacity
}

void draw_horizontal_line(volatile char *fb, int x1, int x2, int y, color c, int width, int stride) {
        for (int i = x1; i < x2; i++)
                draw_pixel(fb, i, y, width, stride);
}

void draw_vertical_line(volatile char *fb, int x, int y1, int y2, color c, int width, int stride) {
        for (int i = y1; i < y2; i++)
                draw_pixel(fb, x, i, width, stride);
}

void draw_filled_rectangle(volatile char *fb, int x1, int y1, int w, int h, color c, int width, int stride) {
        for (int i = y1; i < (y1 + h); i++)
                draw_horizontal_line(fb, x1, (x1 + w), i, c, width, stride);
}

void draw_text(volatile char *fb, char *text, int textX, int textY, int width, int stride) {
        // loop through all characters in the text string
        int l = strlen(text);

        for (int i = 0; i < l; i++) {
                if(text[i] < 32)
                        continue;

                int ix = font_index(text[i]);
                unsigned char *img = letters[ix];

                for (int y = 0; y < FONTH; y++) {
                        unsigned char b = img[y];

                        for (int x = 0; x < FONTW; x++) {
                                if (((b << x) & 0b10000000) > 0)
                                        draw_pixel(fb, textX + i * FONTW + x, textY + y, width, stride);
                        }
                }
        }
}

/* Helper functions */ 
font_params get_font_params() {
        font_params params = {.width=FONTW, .height=FONTH};

        return params;
}

static inline void msm_write(unsigned int val, unsigned int off)
{
	//__raw_writel(val, debug_port_base + off);
        *(int*)(debug_port_base + off) = val;
}

static inline unsigned int msm_read(unsigned int off)
{
	//return __raw_readl(debug_port_base + off);
        return *(int*)(debug_port_base + off);
}


static inline int debug_getc(void)
{
	if (msm_read(UART_SR) & UART_SR_RX_READY) {
		return msm_read(UART_RF);
	} else {
		return -1;
	}
}

static inline void debug_putc(unsigned int c)
{
	while (!(msm_read(UART_SR) & UART_SR_TX_READY)) ;
	msm_write(c, UART_TF);
}

static void debug_puts(char *s)
{
	unsigned c;
	while ((c = *s++)) {
		if (c == '\n')
			debug_putc('\r');
		debug_putc(c);
	}
}

void printk(char *text) {
	if(debug_linecount > 100 || debug_linecount < 0)
		debug_linecount = 0;

//	draw_text((char*)0x2a00000, "[uniLoader] ", 0, (20 + (debug_linecount * 30)), 480, 4);
	draw_text((char*)0x3b00000, text, 0, (20 + (debug_linecount * 30)), 480, 4);

	debug_linecount++;
}

int c_entry() {
	for(;;) {
		printk("hi\n");
                debug_puts("hi\n");
	}
}
