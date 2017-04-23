#include "console.h"
#include "string.h"
#include "vargs.h"

static int vsprintf(char *buff, const char *format, va_list args);

void printk(const char *format, ...) {
	// 避免频繁创建临时变量，内核的栈很宝贵
	static char buff[1024]; 			// space
	va_list args; 						// char *
	int i;

	va_start(args, format); 			// allocate  
	i = vsprintf(buff, format, args);	// handle
	va_end(args);						// finish

	buff[i] = '\0';	

	console_write(buff);
}

void printk_color(real_color_t back, real_color_t fore, const char *format, ...) {
	// 避免频繁创建临时变量，内核的栈很宝贵
	static char buff[1024];
	va_list args;
	int i;

	va_start(args, format);
	i = vsprintf(buff, format, args);
	va_end(args);

	buff[i] = '\0';

	console_write_color(buff, back, fore);
}

#define is_digit(c)	((c) >= '0' && (c) <= '9')

static int skip_atoi(const char **s) {
	int i = 0;

	while (is_digit(**s)) {
		i = i * 10 + *((*s)++) - '0';
	}

	return i;
}

#define ZEROPAD		1		// pad with zero
#define SIGN	 	2   	// unsigned/signed long
#define PLUS    	4		// show plus
#define SPACE	  	8   	// space if plus
#define LEFT	 	16  	// left justified
#define SPECIAL		32  	// 0x
#define SMALL	  	64  	// use 'abcdef' instead of 'ABCDEF'

#define do_div(n,base) ({ \
		int __res; \
		__asm__("divl %4":"=a" (n),"=d" (__res):"0" (n),"1" (0),"r" (base)); \
		__res; })

static char *number(char *str, int num, int base, int size, int precision, int type) {
	char c, sign, tmp[36];
	const char *digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	int i;

	if (type & SMALL) {
		digits ="0123456789abcdefghijklmnopqrstuvwxyz";
	}
	if (type & LEFT) {
		type &= ~ZEROPAD;
	}
	if (base < 2 || base > 36) {
		return 0;
	}

	c = (type & ZEROPAD) ? '0' : ' ' ;

	if (type & SIGN && num < 0) {
		sign = '-';
		num = -num;
	} else {
		sign = (type&PLUS) ? '+' : ((type&SPACE) ? ' ' : 0);
	}

	if (sign) {
	      size--;
	}
	if (type & SPECIAL) {
		if (base == 16) {
			size -= 2;
		} else if (base == 8) {
			size--;
		}
	}
	i = 0;
	if (num == 0) {
		tmp[i++] = '0';
	} else {
		while (num != 0) {
			tmp[i++] = digits[do_div(num,base)];
		}
	}

	if (i > precision) {
		precision = i;
	}
	size -= precision;

	if (!(type&(ZEROPAD+LEFT))) {
		while (size-- > 0) {
			*str++ = ' ';
		}
	}
	if (sign) {
		*str++ = sign;
	}
	if (type & SPECIAL) {
		if (base == 8) {
			*str++ = '0';
		} else if (base == 16) {
			*str++ = '0';
			*str++ = digits[33];
		}
	}
	if (!(type&LEFT)) {
		while (size-- > 0) {
			*str++ = c;
		}
	}
	while (i < precision--) {
		*str++ = '0';
	}
	while (i-- > 0) {
		*str++ = tmp[i];
	}
	while (size-- > 0) {
		*str++ = ' ';
	}

	return str;
}

// static int vsprintf(char *buff, const char *format, va_list args) {}
// buff = space; format = string arg; args = start pointer.

// 
// Hint - vsprintf
// 
// Func: translate the format string into assigned string with the args;
//
// Usages:
// 
// char c, idx; for format string: 
// 
// if c != % do: buff[index] = c
// else if c == % do:
//     if %- || %+ || %  || %# || %0 do: update flags
//
//	   if % + integer do: get field width
//     else if idx == * do: get field width, update flags
// 	   
//     if idx == . do: get the precision
//     
//     if idx == h || idx == l || idx == L do: conversion qualifier
//     
//     if %c || %s || ... do: something...
// 
// *(str) = '\0' // end of string
//
// // After that, get assigned string str, 
// 
// return length = str-buff

static int vsprintf(char *buff, const char *format, va_list args) {
	int len, i;
	char *str, *s;		// str => buff
	int *ip;

	int flags;			// flags to number()

	int field_width;	// width of output field
	
	// min number of digits for integers; max number of chars for from string
	int precision;

	// Handler
	for (str = buff; *format; ++format) {
		if (*format != '%') { 	// isnot placeholder
			*str++ = *format;
			continue;
		}
		
		flags = 0;
		repeat:
			++format;						// this also skips first '%'
			switch (*format) {
				case '-': flags |= LEFT; 	// left precision
					  goto repeat;
				case '+': flags |= PLUS;
					  goto repeat;
				case ' ': flags |= SPACE;
					  goto repeat;
				case '#': flags |= SPECIAL;
					  goto repeat;
				case '0': flags |= ZEROPAD;	// add 0
					  goto repeat;
			}
		
		// eg. %10d => length = 10

		// get field width
		field_width = -1;
		if (is_digit(*format)) { // number
			field_width = skip_atoi(&format);
		} else if (*format == '*') { 
			// it's the next argument
			field_width = va_arg(args, int);
			if (field_width < 0) {
				field_width = -field_width;
				flags |= LEFT;
			}
		}

		// eg. %10.9f 
		// eg. %5.9s <=> 5 <= len(string) <= 9

		// get the precision
		precision = -1;
		if (*format == '.') { // '.'
			++format;	
			if (is_digit(*format)) {
				precision = skip_atoi(&format);
			} else if (*format == '*') { // eg. printf("%*.*s\n",m,n,ch);
				// it's the next argument
				precision = va_arg(args, int);
			}
			if (precision < 0) {
				precision = 0;
			}
		}

		// get the conversion qualifier
		// int qualifier = -1;	// 'h', 'l', or 'L' for integer fields
		if (*format == 'h' || *format == 'l' || *format == 'L') {
			// qualifier = *format;
			++format;
		}

		switch (*format) { // placeholder
		case 'c': // %c => char
			if (!(flags & LEFT)) {
				while (--field_width > 0) {
					*str++ = ' ';
				}
			}
			*str++ = (unsigned char) va_arg(args, int);
			while (--field_width > 0) {
				*str++ = ' ';
			}
			break;

		case 's': // %s => string
			s = va_arg(args, char *);
			len = strlen(s);
			if (precision < 0) {
				precision = len;
			} else if (len > precision) {
				len = precision;
			}

			if (!(flags & LEFT)) {
				while (len < field_width--) {
					*str++ = ' ';
				}
			}
			for (i = 0; i < len; ++i) {
				*str++ = *s++;
			}
			while (len < field_width--) {
				*str++ = ' ';
			}
			break;

		case 'o': // %o => 8
			str = number(str, va_arg(args, unsigned long), 8,
				field_width, precision, flags);
			break;

		case 'p': // %p => pointer
			if (field_width == -1) {
				field_width = 8;
				flags |= ZEROPAD;
			}
			str = number(str, (unsigned long) va_arg(args, void *), 16,
				field_width, precision, flags);
			break;

		case 'x': // %x 
			flags |= SMALL;
		case 'X': // %X => 16
			str = number(str, va_arg(args, unsigned long), 16,
				field_width, precision, flags);
			break;

		case 'd': 
		case 'i': // %d & %i => integer
			flags |= SIGN;
		case 'u': // %u => unsigned 10
			str = number(str, va_arg(args, unsigned long), 10,
				field_width, precision, flags);
			break;
		case 'b': // %b 
			str = number(str, va_arg(args, unsigned long), 2,
				field_width, precision, flags);
			break;

		case 'n': // %n
			ip = va_arg(args, int *);
			*ip = (str - buff);
			break;

		default: 
			if (*format != '%')
				*str++ = '%';
			if (*format) {
				*str++ = *format;
			} else {
				--format;
			}
			break;
		}
	}

	*str = '\0';

	return (str-buff);
}
