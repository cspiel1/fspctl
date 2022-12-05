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
#include "fmqtt.h"

static bool run = false;

static void signal_handler(int sig)
{
	run = false;
}


static void print_usage(const char *pname)
{
	printf("Usage:\n%s [options] [address]\n"
		"Options:\n"
		"-s, --set=value\n"
		"       The given value will be written to the address, which "
		"       is mandatory in this case.\n"
		"\n"
		"-b, --bit=pos\n"
		"       The bit position for boolean set.\n"
		"\n"
		"-d, --daemon\n"
		"       Start daemon that polls configured registers and "
			"sends to MQTT broker.\n"
		"       If broker is not configured the values are printed to "
			"stdout.\n"
		"\n"
		"-n, --nomodbus\n"
		"       For debugging/valgrind. No modbus connection is "
			"opened. \n"
		"\n"
		"-g, --group id\n"
		"       The given group will be queried and printed. Can't be "
		"       combined with option --set.\n"
		"-D, --dev device\n"
		"-i, --id slave\n"
		, pname);
}


int main(int argc, char *argv[])
{
	int ret;
	modbus_t *ctx = NULL;
	int addr = -1;
	struct fconf conf;
	char fname[255];
	bool set = false;
	int8_t bit = -1;
	char setval[20];
	int group = -1;
	const char *dev = NULL;
	int slave = 0;
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
			{"daemon",    0, 0, 'd'},
			{"dev",       1, 0, 'D'},
			{"id",        1, 0, 'i'},
			{0,           0, 0,   0}
		};

		c = getopt_long(argc, argv, "s:ng:b:dD:i:", long_options, NULL);
		if (c == -1)
			break;

		switch (c) {
			case 's':
				set = true;
				strncpy(setval, optarg, sizeof(setval));
				break;

			case 'd':
				run = true;
				break;

			case 'n':
				nomodbus = true;
				break;

			case 'g':
				group = atoi(optarg);
				break;

			case 'b':
				bit = (int8_t) atoi(optarg);
				break;

			case 'i':
				slave = atoi(optarg);
				break;

			case 'D':
				dev = optarg;
				break;
			default:
				print_usage(argv[0]);
				return EINVAL;
		}
	}

	if (optind < argc)
		addr = atoi(argv[optind]);

	if (!dev)
		dev = conf.ttydev;

	if (!slave)
		slave = conf.slaveid;

	printf("Connecting to %s modbus slave %d stopbits %d\n",
	       dev, slave, conf.stopbits);

	if (!nomodbus) {
		ctx = modbus_new_rtu(dev, 19200, 'N', 8, conf.stopbits);
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

		ret = modbus_set_slave(ctx, slave);
		if(ret < 0) {
			perror("modbus_set_slave error\n");
			goto out;
		}
	}

	(void)signal(SIGINT, signal_handler);
	(void)signal(SIGALRM, signal_handler);
	(void)signal(SIGTERM, signal_handler);

	if (group >= 0) {
		err = print_group(ctx, group);
	}
	else if (set && addr >= 0) {
		if (bit == -1)
			bit = 0;

		err = register_write(ctx, addr, bit, setval);
	}
	else if (run) {
		bool mqtt = conf.mqtthost != NULL;
		uint64_t ms = tmr_jiffies();

		if (mqtt) {
			err = fmqtt_init(&conf);
			if (err)
				goto out;
		}

		while (run) {
			if (mqtt)
				err = fmqtt_loop();

			if (err)
				break;

			if (tmr_jiffies() > ms) {
				ms += 5000;

				/* overrun? */
				if (ms < tmr_jiffies())
					ms = tmr_jiffies() + 5000;

				err = publish_registers(ctx, &conf);
			}

			if (err)
				break;

			if (usleep(10000)==-1)
				break;
		}

		run = false;
		if (mqtt)
			fmqtt_close();
	}
	else if (addr >= 0) {
		err = print_register(ctx, addr, bit);
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
