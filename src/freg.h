#ifndef FREG_H
#define FREG_H

#include <stdint.h>

typedef struct _modbus modbus_t;
struct fconf;

int print_all_registers(modbus_t *ctx);
int print_register(modbus_t *ctx, int addr, int8_t bit);
int print_group(modbus_t *ctx, int group);

int register_write(modbus_t *ctx, int addr, uint8_t bit, const char *val);

int publish_registers(modbus_t *ctx, struct fconf *conf);

#endif
