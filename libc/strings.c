#include <strings.h>
#include <stdint.h>

uint32_t strcmp(char *str1, char *str2)
{
	uint32_t ret = 0;
	while(str1[ret] != '\0')
	{
		if(str1[ret] != str2[ret])
			return ret+1;
		ret++;
	}
	if(str2[ret] != '\0')
		return ret+1;
	return 0;
}

void strcopy(char *dst, char *src)
{
	uint32_t i = 0;
	while(src[i])
	{
		dst[i] = src[i];
		i++;
	}
	dst[i] = '\0';
}

uint32_t strlen(char *str)
{
	uint32_t i;
	for(i=0; str[i]; i++);
	return i;
}

int num2str(uint32_t num, uint32_t base, char *buf)
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