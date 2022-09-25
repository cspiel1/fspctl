#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <openssl/rand.h>
#include <mosquitto.h>
#include <errno.h>

#include "futil.h"
#include "fconf.h"
#include "fmqtt.h"

#define GUUID_LEN  36

struct fmqtt {
	struct mosquitto *m;
	int qos;

	char guuid[GUUID_LEN + 1];
};


static struct fmqtt d;


uint32_t rand_u32(void)
{
	uint32_t v;

	v = 0;
	if (RAND_bytes((unsigned char *)&v, sizeof(v)) < 0)
		printf("WRN - could not get random number\n");

	return v;
}


uint16_t rand_u16(void)
{
	return rand_u32() >> 16;
}


static int guuid_make(char *buf, size_t buflen)
{
	int n = snprintf(buf, buflen, "%08x-%04x-%04x-%04x-%08x%04x",
		       rand_u32(), rand_u16(), rand_u16(), rand_u16(),
		       rand_u32(), rand_u16());

	if (n != GUUID_LEN) {
		printf("WRN - could not generate GUUID\n");
		return EINVAL;
	}

	return 0;
}


int fmqtt_init(const struct fconf *conf)
{
	int err;
	int merr;

	if (!conf || !conf->mqtthost)
		return EINVAL;

	memset(&d, 0, sizeof(d));
	d.qos = 1;
	merr = mosquitto_lib_init();
	if (merr)
		goto out;

	err = guuid_make(d.guuid, sizeof(d.guuid));
	if (err)
		goto out;

	d.m = mosquitto_new(d.guuid, false, &d);
	if (!d.m) {
		err = ENOMEM;
		goto out;
	}

/*        merr = mosquitto_threaded_set(d.m, false);*/
/*        if (merr)*/
/*                goto out;*/

	merr = mosquitto_username_pw_set(d.m, conf->mqttuser, conf->mqttpass);
	if (merr)
		goto out;

	if (conf->cafile || conf->capath) {
		merr = mosquitto_tls_set(d.m, conf->cafile, conf->capath,
					 NULL, NULL, NULL);
		if (merr)
			goto out;
	}

	merr = mosquitto_connect(d.m, conf->mqtthost, conf->mqttport, 30);
	if (merr)
		goto out;

/*        merr = mosquitto_loop_start(d.m);*/
	fmqtt_loop();

out:
	if (merr) {
		printf("ERR - Could not connect to MQTT broker\n");
		printf("   %s\n", mosquitto_strerror(merr));
		err = EIO;
	}

	if (err) {
		mosquitto_disconnect(d.m);
		mosquitto_destroy(d.m);
	}

	if (!err) {
		printf("INF - successfully connected to %s:%d \n",
		       conf->mqtthost, conf->mqttport);
	}

	return err;
}


int fmqtt_loop(void)
{
	int merr;

	merr = mosquitto_loop(d.m, -1, 1);
	if (merr) {
		printf("ERR - mqtt loop failed\n");
		printf("      %s\n", mosquitto_strerror(merr));
		if (merr == MOSQ_ERR_NO_CONN) {
			merr = mosquitto_reconnect(d.m);
			printf("ERR - mqtt reconnect failed\n");
			printf("      %s\n", mosquitto_strerror(merr));
		}
	}

	return merr ? EIO : 0;
}


void fmqtt_close(void)
{
	mosquitto_disconnect(d.m);
/*        mosquitto_loop_stop(d.m, false);*/
	mosquitto_destroy(d.m);
	mosquitto_lib_cleanup();
}


int fmqtt_publish(const char *topic, const char *fmt, ...)
{
	char payload[65];
	va_list va;
	int err = 0;
	int merr;

	va_start(va, fmt);
	vsnprintf(payload, sizeof(payload), fmt, va);
	va_end(va);
	merr = mosquitto_publish(d.m, NULL, topic, strlen(payload), payload, 1,
				 false);

	if (merr) {
		printf("ERR - Could not publish to MQTT broker\n");
		printf("   %s\n", mosquitto_strerror(merr));
		err = EIO;
	}

	return err;
}
