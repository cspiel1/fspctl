#include <stdio.h>
#include <unistd.h>
#include <modbus/modbus.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/serial.h>
#include <asm/ioctls.h>
#include <stdlib.h>

#include "futil.h"
#include "fconf.h"

#define NB_REGS 1


int main(int argc, char *argv[])
{
	int ret;
	modbus_t *ctx;
	int addr = 0;
	uint16_t dest [NB_REGS];
	struct fconf conf;
	char fname[255];
	int err;


	memset(&conf, 0, sizeof(conf));
	sprintf(fname, "%s/.config/.fspctl", getenv("HOME"));
	err = read_fconfig(&conf, fname);
	if (err)
		return err;

	if (argc > 1)
		addr = atoi(argv[1]);

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

	ret = modbus_read_registers(ctx, addr, 1, dest);
	if(ret < 0) {
		perror("modbus_read_regs error\n");
		return -1;
	}

	printf("val = %04x = %u = ", dest[0], dest[0]);
	bin_u16(dest[0]);
	ascii_u16(dest[0]);

	modbus_close(ctx);
	modbus_free(ctx);

	free_fconfig(&conf);
}
