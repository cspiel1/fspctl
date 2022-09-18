#include <stdio.h>
#include <unistd.h>
#include <modbus/modbus.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/serial.h>
#include <asm/ioctls.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>

#include "futil.h"
#include "fconf.h"
#include "freg.h"

static void print_usage(const char *pname)
{
	printf("Usage:\n%s "
			"[--set value] [address]"
			"\n", pname);
}


int main(int argc, char *argv[])
{
	int ret;
	modbus_t *ctx;
	int addr = -1;
	uint16_t dest;
	struct fconf conf;
	char fname[255];
	bool set = false;
	uint16_t setval;
	int err;

	memset(&conf, 0, sizeof(conf));
	sprintf(fname, "%s/.config/.fspctl", getenv("HOME"));
	err = read_fconfig(&conf, fname);
	if (err)
		return err;

	while (1) {
		int c;
		static struct option long_options[] = {
			{"set",  1, 0, 's'},
			{0,      0, 0,   0}
		};

		c = getopt_long(argc, argv, "", long_options, NULL);
		if (c == -1)
			break;

		switch (c) {
			case 's':
				set = true;
				setval = (uint16_t) atoi(optarg);
				break;

			default:
				print_usage(argv[0]);
				return EINVAL;
		}
	}


	printf("Connecting to %s modbus slave %d\n",
	       conf.ttydev, conf.slaveid);

	ctx = modbus_new_rtu(conf.ttydev, 19200, 'N', 8, conf.stopbits);
	if (ctx == NULL) {
		perror("Unable to create the libmodbus context\n");
		return -1;
	}

/*        ret = modbus_set_debug(ctx, TRUE);*/

	ret = modbus_rtu_set_serial_mode(ctx, MODBUS_RTU_RS232);
	if(ret < 0){
		perror("modbus_rtu_set_serial_mode error\n");
		return -1;
	}

	ret = modbus_connect(ctx);
	if(ret < 0){
		perror("modbus_connect error\n");
		return -1;
	}

	ret = modbus_set_slave(ctx, conf.slaveid);
	if(ret < 0) {
		perror("modbus_set_slave error\n");
		return -1;
	}

	if (addr == -1 )
		ret = print_all_registers(ctx);
	else
		ret = modbus_read_registers(ctx, addr, 1, &dest);

	if(ret < 0) {
		perror("modbus_read_regs error\n");
		return -1;
	}

	printf("val = %04x = %u = ", dest, dest);
	bin_u16(dest);
	ascii_u16(dest);

	modbus_close(ctx);
	modbus_free(ctx);

	free_fconfig(&conf);
}
