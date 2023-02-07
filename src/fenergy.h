#ifndef FENERGY_H
#define FENERGY_H

typedef struct _modbus modbus_t;
int fenergy_publish(modbus_t *ctx, struct fconf *conf);

#endif
