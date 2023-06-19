#ifndef FMQTT_H
#define FMQTT_H

#include <stdbool.h>

struct fconf;

int fmqtt_init(const struct fconf *conf);
int fmqtt_loop(void);
int fmqtt_publish(const char *topic, bool retain, const char *fmt, ...);
void fmqtt_close(void);

#endif
