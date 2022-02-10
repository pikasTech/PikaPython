#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <ctype.h>

#include "stm32_p103.h"
#include "stm32f10x_usart.h"


//extern void USART_SendData(USART_TypeDef* USARTx, uint16_t Data);
//extern uint16_t USART_ReceiveData(USART_TypeDef* USARTx);

#define mydo_div(n,base) ({ \
	int _res; \
	_res = ((unsigned long) n) % (unsigned) base; \
	n = ((unsigned long) n) / (unsigned) base; \
	_res; \
})

#define ZEROPAD	1		/* pad with zero */
#define SIGN   	2		/* unsigned/signed long */
#define PLUS   	4		/* show plus */
#define SPACE  	8		/* space if plus */
#define LEFT  	16		/* left justified */
#define SPECIAL	32		/* 0x */
#define LARGE  	64		/* use 'ABCDEF' instead of 'abcdef' */
#define SMALL  	64		/* use 'abcdef' instead of 'ABCDEF' */
#define PBSIZE 128 /* Print Buffer Size  */

void myprintf_init()
{
    init_rs232();
    enable_rs232_interrupts();
    enable_rs232();
}

void usart_send_bytes(uint8_t data)
{
    while(!USART_GetFlagStatus(USART2 ,USART_FLAG_TXE)) {
    }
    USART_SendData( USART2, data);
}


signed int mystrnlen (const char * s, signed int count)
{
    const char *sc;

    for (sc = s; count-- && *sc != '\0'; ++sc) {
        /* nothing */
    }
    return sc - s;
}

static int myskip_atoi (const char **s)
{
    int i = 0;

    while (isdigit(**s) != 0) {
        i = i * 10 + *((*s)++) - '0';
    }
    return i;
}

static char * mynumber (char * str, long num, int base, int size, int precision ,int type)
{
    char c = 0;
    char sign = 0;
    char tmp[66] = {0};
    int  i = 0;
    const char *digits = "0123456789abcdefghijklmnopqrstuvwxyz";

    if ((type & LARGE) != 0) {
        digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    }
    if ((type & LEFT) != 0) {
        type &= ~ZEROPAD;
    }
    if ((base < 2) || (base > 36)) {
        return 0;
    }

    c = (type & ZEROPAD) ? '0' : ' ';
    sign = 0;

    if ((type & SIGN) != 0) {
        if (num < 0) {
            sign = '-';
            num = -num;
            size--;
        } else if ((type & PLUS) != 0) {
            sign = '+';
            size--;
        } else if ((type & SPACE) != 0) {
            sign = ' ';
            size--;
        }
    }
    if ((type & SPECIAL) != 0) {
        if (16 == base) {
            size -= 2;
        } else if (8 == base) {
            size--;
        }
    }
    i = 0;
    if (0 == num) {
        tmp[i++] = '0';
    } else {
        while (num != 0) {
            tmp[i++] = digits[mydo_div(num,base)];
        }
    }
    if (i > precision) {
        precision = i;
    }
    size -= precision;
    if (0 == (type & (ZEROPAD + LEFT))) {
        while (size-- > 0) {
            *str++ = ' ';
        }
    }
    if (sign != 0) {
        *str++ = sign;
    }
    if ((type & SPECIAL) != 0) {
        if (8 == base) {
            *str++ = '0';
        } else if (16 == base) {
            *str++ = '0';
            *str++ = digits[33];
        }
    }
    if (0 == (type & LEFT)) {
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

/**
 * Format a string and place it in a buffer (va_list version)
 *
 * @param buf	The buffer to place the result into
 * @param fmt	The format string to use
 * @param args	Arguments for the format string
 *
 * The function returns the number of characters written
 * into @buf. Use vsnprintf() or vscnprintf() in order to avoid
 * buffer overflows.
 *
 * If you're not already dealing with a va_list consider using sprintf().
 */

int myvsprintf (char *buf, const char *fmt, va_list args)
{
    int len;
    int i;
    int flags;		    // flags to number()
    int base;
    int field_width;	//width of output field
    int precision;		//min. # of digits for integers; max number of chars for from string
    int qualifier;		//'h', 'l', or 'q' for integer fields
    char * str;
    const char *s;
    unsigned long num;

    for (str = buf; *fmt; ++fmt) {
        if (*fmt != '%') {
            *str++ = *fmt;
            continue;
        }

        flags = 0;     //process flags

repeat:
        ++fmt;		   //this also skips first '%'
        switch (*fmt) {
        case '-':
            flags |= LEFT;
            goto repeat;
        case '+':
            flags |= PLUS;
            goto repeat;
        case ' ':
            flags |= SPACE;
            goto repeat;
        case '#':
            flags |= SPECIAL;
            goto repeat;
        case '0':
            flags |= ZEROPAD;
            goto repeat;
        }

        field_width = -1;  //get field width
        if (isdigit(*fmt) != 0) {

            field_width = myskip_atoi(&fmt);
        } else if ('*' == *fmt) {
            ++fmt;
            field_width = va_arg(args, int); //it's the next argument
            if (field_width < 0) {
                field_width = -field_width;
                flags |= LEFT;
            }
        }

        precision = -1;      //get the precision
        if ('.' == *fmt) {
            ++fmt;
            if (isdigit(*fmt)) {
                precision = myskip_atoi(&fmt);
            } else if ('*' == *fmt) {
                ++fmt;
                precision = va_arg(args, int); //it's the next argument
            }
            if (precision < 0) {
                precision = 0;
            }
        }

        qualifier = -1;         //get the conversion qualifier
        if (('h' == *fmt) || ('l' == *fmt) || ('q' == *fmt)) {
            qualifier = *fmt;
            ++fmt;
        }

        base = 10;          //default base
        switch (*fmt) {
        case 'c': {
            if (0 == (flags & LEFT)) {
                while (--field_width > 0) {
                    *str++ = ' ';
                }
            }
            *str++ = (unsigned char) va_arg(args, int);
            while (--field_width > 0) {
                *str++ = ' ';
            }
            continue;
        }
        case 's': {
            s = va_arg(args, char *);
            if (!s) {
                s = "<NULL>";
            }
            len = mystrnlen(s, precision);
            if (0 == (flags & LEFT)) {
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
            continue;
        }
        case 'p': {
            if (-1 == field_width) {
                field_width = 2 * sizeof(void *);
                flags |= ZEROPAD;
            }
            str = mynumber(str, (unsigned long)va_arg(args, void *), 16, field_width, precision, flags);
            continue;
        }
        case 'n': {
            if ('l' == qualifier) {
                long * ip = va_arg(args, long *);
                *ip = (str - buf);
            } else {
                int * ip = va_arg(args, int *);
                *ip = (str - buf);
            }
            continue;
        }
        case '%': {
            *str++ = '%';
            continue;
        }

        case 'o': { //integer number formats - set up the flags and "break"
            base = 8;
            break;
        }

        case 'X': {
            flags |= LARGE;
        }
        case 'x': {
            base = 16;
            break;
        }
        case 'd':
        case 'i': {
            flags |= SIGN;
        }
        case 'u': {
            break;
        }
        default: {
            *str++ = '%';
            if (0 == *fmt) {
                *str++ = *fmt;
            } else {
                --fmt;
            }
            continue;
        }
        }

        if ('l' == qualifier) {
            num = va_arg(args, unsigned long);
        } else if ('h' == qualifier) {
            num = (unsigned short) va_arg(args, int);
            if (0 == (flags & SIGN)) {
                num = (short) num;
            }
        } else if (0 == (flags & SIGN)) {
            num = va_arg(args, int);
        } else {
            num = va_arg(args, unsigned int);
        }

        str = mynumber(str, num, base, field_width, precision, flags);

    }
    *str = '\0';

    return str-buf;
}

void myputc (const char c)
{
    if (c == '\n')
        usart_send_bytes('\r');
    usart_send_bytes(c);
}

void myputs (const unsigned char *s)
{
    while (*s) {
        myputc (*s++);
    }
}

void myprintf (const char *fmt,...)
{
    va_list args;
    char printbuffer[PBSIZE];

    va_start(args, fmt);
    myvsprintf(printbuffer, fmt, args);
    va_end(args);

    myputs((const unsigned char *)printbuffer);  // Print the string
}

