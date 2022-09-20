#include <stdio.h>
#include <unistd.h>
#include <modbus.h>
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
	printf("Usage:\n%s [options] [address]\n"
		"Options:\n"
		"--set value      The given value will be written to the \n"
		"                 address, which is mandatory in this case.\n"
		"\n"
		"--nomodbus       For debugging/valgrind. No modbus connection"
		"\n"
		"                 is opened. \n"
		"\n"
		"--group id       The given group will be queried and printed."
		"\n"
		"                 Can't be combined with option --set."
		"\n", pname);
}


int main(int argc, char *argv[])
{
	int ret;
	modbus_t *ctx = NULL;
	int addr = -1;
	struct fconf conf;
	char fname[255];
	bool set = false;
	uint16_t setval;
	int group = -1;
	bool nomodbus = false;
	int err;

	memset(&conf, 0, sizeof(conf));
	sprintf(fname, "%s/.config/.fspctl", getenv("HOME"));
	err = read_fconfig(&conf, fname);
	if (err)
		return err;

	while (1) {
		int c;
		static struct option long_options[] = {
			{"set",       1, 0, 's'},
			{"nomodbus",  0, 0, 'n'},
			{"group",     1, 0, 'g'},
			{0,           0, 0,   0}
		};

		c = getopt_long(argc, argv, "sng:", long_options, NULL);
		if (c == -1)
			break;

		switch (c) {
			case 's':
				set = true;
				setval = (uint16_t) atoi(optarg);
				break;

			case 'n':
				nomodbus = true;
				break;

			case 'g':
				group = atoi(optarg);
				break;

			default:
				print_usage(argv[0]);
				return EINVAL;
		}
	}

	if (optind < argc)
		addr = atoi(argv[optind]);

	printf("Connecting to %s modbus slave %d\n",
	       conf.ttydev, conf.slaveid);

	if (!nomodbus) {
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
	}

	if (group >= 0)
		ret = print_group(ctx, group);
	else if (addr >= 0)
		ret = print_register(ctx, addr);
	else
		ret = print_all_registers(ctx);

	if(ret < 0) {
		perror("modbus_read_regs error\n");
		return -1;
	}

	modbus_close(ctx);
	modbus_free(ctx);

	free_fconfig(&conf);
}
