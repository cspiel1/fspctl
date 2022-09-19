#ifndef FREG_H
#define FREG_H

typedef struct _modbus modbus_t;

int print_all_registers(modbus_t *ctx);
int print_register(modbus_t *ctx, int addr);

#endif
