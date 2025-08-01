#include "console.h"
#include <stdint.h>
#include <stdarg.h>
//typedef unsigned int uint32_t;
//typedef unsigned short uint16_t;
volatile uint16_t* const vga_buffer = (uint16_t*)0xB8000;
static const int VGA_WIDTH = 80;
static const int VGA_HEIGHT = 25;
static int cursor_row = 0;
static int cursor_col = 0;
static uint8_t attribute = 0x0F; // White foreground, Black background

static void scroll_if_needed()
{
	if (cursor_row < VGA_HEIGHT) return;

	for (int r = 1; r < VGA_HEIGHT; r++) {
		for (int c = 0; c < VGA_WIDTH; c++) {
			vga_buffer[(r - 1) * VGA_WIDTH + c] = vga_buffer[r * VGA_WIDTH + c];
		}
	}

	int last = (VGA_HEIGHT - 1) * VGA_WIDTH;
	uint16_t blank = ((uint16_t)attribute << 8) | ' ';
	for (int c = 0; c < VGA_WIDTH; c++) {
		vga_buffer[last + c] = blank;
	}
	cursor_row = VGA_HEIGHT - 1;
}

// put single character, handle newline/carriage return and wrap
void kputchar(char ch)
{
	if (ch == '\n') {
		cursor_col = 0;
		cursor_row++;
	} else if (ch == '\r') {
		cursor_col = 0;
	} else {
		int index = cursor_row * VGA_WIDTH + cursor_col;
		vga_buffer[index] = ((uint16_t)attribute << 8) | (uint8_t)ch;
		cursor_col++;
		if (cursor_col >= VGA_WIDTH) {
			cursor_col = 0;
			cursor_row++;
		}
	}
	scroll_if_needed();
}

// print null terminated string
void kprint(const char *s)
{
	for (int i = 0; s[i]; i++) {
		kputchar(s[i]);
	}
}

void kprintln(const char *s)
{
	kprint(s);
	kputchar('\n');
}

// clear the vga buffer
void kclear(void)
{
	uint16_t blank = ((uint16_t)attribute << 8) | ' ';
	for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
		vga_buffer[i] = blank;
	}
	cursor_row = 0;
	cursor_col = 0;
}

// int to str, return pointer to buffer
static char* itoa_unsigned(uint32_t value, char *buf, int base)
{
	static const char digits[] = "0123456789abcdef";
	char temp[33];
	int i = 0;
	if (value == 0) {
		buf[0] = '0';
		buf[1] = '\0';
		return buf;
	}
	while (value != 0) {
		temp[i++] = digits[value % base];
		value /= base;
	}
	int pos = 0;
	while (i--) {
		buf[pos++] = temp[i];
	}
	buf[pos] = '\0';
	return buf;
}

static char* itoa_signed(int32_t value, char *buf)
{
	if (value < 0) {
		buf[0] = '-';
		itoa_unsigned((uint32_t)(-value), buf + 1, 10);
	} else {
		itoa_unsigned((uint32_t)value, buf, 10);
	}
	return buf;
}

// printf (currently supports only %s %d %u %x %%)
void kprintf(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	char numbuf[35];
	for (int i = 0; fmt[i]; i++) {
		if (fmt[i] != '%') {
			kputchar(fmt[i]);
			continue;
		}
		i++;
		if (fmt[i] == '\0') break;
		switch (fmt[i]) {
			case 's': {
				const char *s = va_arg(ap, const char*);
				kprint(s);
				break;
			}
			case 'd': {
				int val = va_arg(ap, int);
				itoa_signed(val, numbuf);
				kprint(numbuf);
				break;
			}
			case 'u': {
				unsigned int val = va_arg(ap, unsigned int);
				itoa_unsigned(val, numbuf, 10);
				kprint(numbuf);
				break;
			}
			case 'x': {
				unsigned int val = va_arg(ap, unsigned int);
				itoa_unsigned(val, numbuf, 16);
				kprint(numbuf);
				break;
			}
			case '%': {
				kputchar('%');
				break;
			}
			default: {
				//unknown specifier
				kputchar('%');
				kputchar(fmt[i]);
				break;
			}
		}
	}
	va_end(ap);
}



