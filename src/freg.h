#ifndef FREG_H
#define FREG_H

#include <stdint.h>

typedef struct _modbus modbus_t;

int print_all_registers(modbus_t *ctx);
int print_register(modbus_t *ctx, int addr);
int print_group(modbus_t *ctx, int group);

int register_write(modbus_t *ctx, int addr, uint8_t bit, const char *val);

#endif
