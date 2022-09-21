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
	uint8_t bit = 0;
	char setval[20];
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
			{"bit",       1, 0, 'b'},
			{"nomodbus",  0, 0, 'n'},
			{"group",     1, 0, 'g'},
			{0,           0, 0,   0}
		};

		c = getopt_long(argc, argv, "s:ng:b:", long_options, NULL);
		if (c == -1)
			break;

		switch (c) {
			case 's':
				set = true;
				strncpy(setval, optarg, sizeof(setval));
				break;

			case 'n':
				nomodbus = true;
				break;

			case 'g':
				group = atoi(optarg);
				break;

			case 'b':
				bit = (uint8_t) atoi(optarg);

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
			goto out;
		}

		/*        ret = modbus_set_debug(ctx, TRUE);*/

		ret = modbus_rtu_set_serial_mode(ctx, MODBUS_RTU_RS232);
		if(ret < 0){
			perror("modbus_rtu_set_serial_mode error\n");
			goto out;
		}

		ret = modbus_connect(ctx);
		if(ret < 0){
			perror("modbus_connect error\n");
			goto out;
		}

		ret = modbus_set_slave(ctx, conf.slaveid);
		if(ret < 0) {
			perror("modbus_set_slave error\n");
			goto out;
		}
	}

	if (group >= 0) {
		err = print_group(ctx, group);
	}
	else if (set && addr >= 0) {
		err = register_write(ctx, addr, bit, setval);
	}
	else if (addr >= 0) {
		err = print_register(ctx, addr);
	}
	else {
		err = print_all_registers(ctx);
	}

out:
	modbus_close(ctx);
	modbus_free(ctx);

	free_fconfig(&conf);

	if (!err && ret < 0)
		err = EIO;

	return err;
}
