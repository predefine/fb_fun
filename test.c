/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2022, Ivaylo Ivanov <ivo.ivanov.ivanov1@gmail.com>
 * Copyright (c) 2022, Markuss Broks <markuss.broks@gmail.com>
 * Copyright (c) 2022, Michael Srba <Michael.Srba@seznam.cz>
 * Copyright (c) 2023, Leandro Friedrich <email@leandrofriedrich.de>
 */

#define FB_ADDR_PATH "/chosen"
#define FB_ADDR_PROP "atag,videolfb"

#include <stdint.h>
#include "simplefb.h"
#include "font.h"
#include "debug.h"
#include <libfdt.h>

static char* fb_address;

void clean_fb(volatile char *fb, char *text, int width, int height, int stride) {
        for (volatile char *addr = fb; addr < fb + (width * height * stride); addr += stride)
                *(int*) (addr) = 0x0;
}

void draw_pixel(volatile char *fb, int x, int y, int width, int stride) {
        long int location = (x * stride) + (y * width * stride);

        *(fb + location) = 0xff; // Blue
        *(fb + location + 1) = 0xff;     // Green
        *(fb + location + 2) = 0xff;     // Red
        *(fb + location + 3) = 0xff;     // Full opacity
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

void printk(char *text) {
	if(debug_linecount > 100 || debug_linecount < 0)
		debug_linecount = 0;

	draw_text(fb_address, text, 0, (debug_linecount * FONTH), 800, 4);

	debug_linecount++;
}
void detect_fb_address(void* fdt){
	int node = fdt_path_offset(fdt,FB_ADDR_PATH);
	if(node < 0){
		//printk("Failed find "FB_ADDR_PATH" in fdt!\n");
		return;
	}
	int len;
	const fdt64_t* value = fdt_getprop(fdt, node, FB_ADDR_PROP, &len);
	if(!value){
		//printk("Value of "FB_ADDR_PATH"/"FB_ADDR_PROP" is null!\n");
		return;
	}
	printk("framebuffer address was found :D\n");
	fb_address = ((char*)0 + *value);
	return;
}

int c_entry(void* a, void* b, void* fdt) {
	detect_fb_address(fdt);
	printk("hi\n");
	printk("hi\n");
	printk("hi\n");
	printk("hi\n");
	printk("hi\n");
	printk("fb_fun on MT8321(aka MT6580) works :D\n");
        for(;;);
}
