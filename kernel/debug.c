#include <stdint.h>
#include <k_debug.h>
#include <pmm.h>
#include <stdarg.h>
#include <ctype.h>

uint16_t *vidmem = (uint16_t *)VIDMEM;

uint32_t scrn_x, scrn_y;

uint8_t colors[] = {0x7, 0x8, 0x9, 0xA, 0xB};

uint8_t text_style = 0x7;

void init_debug()
{
	k_memset((uint8_t*)vidmem, 0x0, SCRN_W*SCRN_H*2);
	scrn_x = scrn_y = 0;
}

void setColor(uint32_t clr)
{
	text_style = colors[clr];
}

void scroll()
{
	while (scrn_y > SCRN_H-1)
	{
		uint32_t i;
		for(i=0; i<SCRN_W*(SCRN_H-1); i++)
			vidmem[i] = vidmem[i+SCRN_W];
		for(i=SCRN_W*(SCRN_H-1); i<SCRN_W*SCRN_H; i++)
			vidmem[i] = 0x0;
		scrn_y --;
	}
}

void k_putch(char c, uint8_t style, uint32_t pos)
{
	vidmem[pos] = c | (style << 0x8);
}

void k_printf(char *str, ...)
{
	va_list args;
	va_start(args, str);
	char buf[255];
	uint32_t num;
	while(*str)
	{
		if(*str == '\n')
		{
			scrn_x = 0;
			scrn_y++;
		} 
		else if(*str == '%')
		{
			str++;
			switch(*str)
			{
				case 'x':
					num = va_arg(args, uint32_t);
					k_num2str(num,16,buf);
					k_printf("0x");
					k_printf(buf);
					break;
				case 'd':
					num = va_arg(args, uint32_t);
					k_num2str(num,10,buf);
					k_printf(buf);
					break;
				case '%':
					k_putch(*str,text_style,scrn_y*80+scrn_x);
					scrn_x++;
					break;
				default:
					str--;
					k_putch(*str, text_style, scrn_y*SCRN_W+scrn_x);
					scrn_x++;
			}
		}
		else if (isprint(*str))
		{
			k_putch(*str, text_style, scrn_y*SCRN_W+scrn_x);
			scrn_x++;
		} else {
			;
		}
		if(scrn_x >= SCRN_W)
		{
			scrn_x = 0;
			scrn_y++;
		}
		scroll();
		str++;
	}
	va_end();
}

int k_num2str(uint32_t num, uint32_t base, char *buf)
{
	if(num == 0)
	{
		buf[0] = '0';
		buf[1] = '\0';
		return;
	}
	
	uint32_t i = 0, j=0;
 	char c;
	
	while(num > 0)
	{
		if(num%base < 10)
			buf[i++] = (char)((uint32_t)'0' + num%base);
		else
			buf[i++] = (char)((uint32_t)'A' + num%base-10);
		num /= base;
	}
	
	for(i--, j=0; j<i; j++,i--)
	{
		char tmp = buf[i];
		buf[i] = buf[j];
		buf[j] = tmp;
	}
	buf[i+j+1] = '\0';
	
	return i+j+1;
}
