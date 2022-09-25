#ifndef FMQTT_H
#define FMQTT_H

#include <stdbool.h>

struct fconf;

#ifdef USE_MQTT
int fmqtt_init(const struct fconf *conf);
int fmqtt_loop(void);
int fmqtt_publish(const char *topic, const char *fmt, ...);
void fmqtt_close(void);
#else
int fmqtt_init(const struct fconf *conf) { return 0; }
int fmqtt_loop(void) { return 0; }
int fmqtt_publish(const char *topic, const char *fmt, ...)
{
	(void)field;
	(void)category;
	(void)fmt;
	return 0;
}

void fmqtt_close(void) { }
#endif

#endif
