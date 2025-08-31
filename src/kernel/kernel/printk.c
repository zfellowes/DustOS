#include <stdarg.h>
#include <kernel/kernel.h>
#include <drivers/display.h>

static void print_int(int value, int base, bool is_unsigned)
{
	char buffer[64];
	int i = 0;
	unsigned int v;

	if (!is_unsigned && value < 0) {
		print_string("-");
		v = (unsigned int)(-value);
	} else {
		v = (unsigned int)value;
	}

	do {
		int digit = v % base;
		buffer[i++] = (digit < 10) ? '0' + digit : 'a' + digit - 10;
		v /= base;
	} while (v > 0);

	buffer[i] = '\0';
	reverse(buffer);
	print_string(buffer);
}

static void print_float(double value, int precision)
{
	if (value < 0) {
		print_string("-");
		value = -value;
	}

	int int_part = (int)value;
	double frac_part = value - int_part;
	print_int(int_part, 10, true);

	if (precision > 0) {
		print_string(".");
		for (int i = 0; i < precision; i++) {
			frac_part *= 10;
			int digit = (int)frac_part;
			char tmp[2] = { (char)('0' + digit), '\0' };
			print_string(tmp);
			frac_part -= digit;
		}
	}
}

int printk(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	for (const char *p = fmt; *p; p++) {
		if (*p != '%') {
			char str[2] = { *p, '\0' };
			print_string(str);
			continue;
		}

		p++;

		int precision = -1;
		if (*p == '.') {
			p++;
			precision = *p - '0';
			p++;
		}

		switch (*p) {
			case 'd': // signed int
				print_int(va_arg(args, int), 10, false);
				break;
			case 'u': // unsigned int
				print_int(va_arg(args, unsigned int), 10, true);
				break;
			case 'x': // hex
				print_int(va_arg(args, unsigned int), 16, true);
				break;
			case 'c': { // char
				char str[2] = { (char)va_arg(args, int), '\0' };
				print_string(str);
				break;
			}
			case 's': { // string
				char *str = va_arg(args, char *);
				print_string(str ? str : "(null)");
				break;
			}
			case 'f': { // float/double 
				double val = va_arg(args, double);
				if (precision < 0) precision = 6;
				print_float(val, precision);
				break;
			}
			case '%': // literal %
				print_string("%");
				break;
			default: // unknown specifier
				print_string("%?");
				break;
		}
	}

	va_end(args);
	return 0;
}


