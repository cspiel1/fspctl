#include <stdint.h>
#include <stdio.h>

#include "futil.h"


static void bin(uint16_t n, uint16_t i)
{
	if (i > 1)
		bin(n >> 1, --i);

	printf("%d", n & 1);
}


void bin_u16(uint16_t v)
{
	bin(v >> 8, 8);
	printf(" ");
	bin(v, 8);
	printf("\n");
}


void ascii_u16(uint16_t v)
{
	printf("%c%c", (char) (v >> 8), (char) v);
	printf("\n");
}
