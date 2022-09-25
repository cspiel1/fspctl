#include <stdint.h>
#include <stdio.h>
#include <sys/time.h>
#include <errno.h>
#include <string.h>

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


/**
 * Get the timer jiffies in milliseconds
 *
 * @return Jiffies in [ms]
 */
uint64_t tmr_jiffies(void)
{
	uint64_t jfs;

	struct timeval now;

	if (0 != gettimeofday(&now, NULL)) {
		fprintf(stderr, "gettimeofday() failed (%s)\n",
				strerror(errno));
		return 0;
	}

	jfs  = (long)now.tv_sec * (uint64_t)1000;
	jfs += now.tv_usec / 1000;

	return jfs;
}
