#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

#include "futil.h"
#include "fconf.h"
#include "freg.h"
#include "fenergy.h"
#include "fmqtt.h"

static struct state_machine {
	struct tm tm;

	char hbody[1280];
	char dbody[1280];
	char mbody[1280];
	char ybody[1280];

	int h;
	int d;
	int m;
	int y;

	bool hfirst;
	bool dfirst;
	bool mfirst;
	bool yfirst;

	bool hrd;
	bool drd;
	bool mrd;
	bool yrd;
} sm;


struct energy_reg {
	int getter;
	int setter;
};

static struct energy_reg regs[] = {
	{409, 448}, /* hourly energy */
	{411, 453}, /* daily energy */
	{413, 457}, /* monthly energy */
	{415, 460}, /* yearly energy */
};


#ifdef USE_MQTT
static char *topics[] = {
	"pv/inverter/energy/hour",
	"pv/inverter/energy/day",
	"pv/inverter/energy/month",
	"pv/inverter/energy/year",
};
#endif


static int append_vtxth(const char *vtxt, void *arg)
{
	char *body = arg;

	if (!strcmp(vtxt, "-1"))
		return EINVAL;

	if (!strcmp(vtxt, "-16777217"))
		return EAGAIN;

	sprintf(body + strlen(body),
		"\"%s\"]", vtxt);
	return 0;
}


void fenergy_init(void) {
	memset(&sm, 0, sizeof(sm));
}


int fenergy_publish(modbus_t *ctx, struct fconf *conf)
{
	time_t t = time(NULL);
	struct tm tm;
	localtime_r(&t, &tm);
	char buf[11];
	int err;

	if (sm.tm.tm_hour != tm.tm_hour) {
		sm.h = -24;
		strcpy(sm.hbody, "{\"hour\": [");
		sm.tm.tm_hour = tm.tm_hour;
		sm.hfirst = true;
	}

	if (sm.tm.tm_mday != tm.tm_mday) {
		sm.d = -31;
		strcpy(sm.dbody, "{\"day\": [");
		sm.tm.tm_mday = tm.tm_mday;
		sm.dfirst = true;
	}

	if (sm.tm.tm_mon != tm.tm_mon) {
		sm.m = -12;
		strcpy(sm.mbody, "{\"month\": [");
		sm.tm.tm_mon = tm.tm_mon;
		sm.mfirst = true;
	}

	if (sm.tm.tm_year != tm.tm_year) {
		sm.y = -10;
		strcpy(sm.ybody, "{\"year\": [");
		sm.tm.tm_year = tm.tm_year;
		sm.yfirst = true;
	}

	if (!sm.hrd && sm.h <= 0) {
		t = time(NULL);
		t += sm.h*3600;
		localtime_r(&t, &tm);
		snprintf(buf, sizeof(buf), "%04d%02d%02d%02d",
			 tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
			 tm.tm_hour);
		err = register_write(ctx, regs[0].setter, 0, buf);
		sm.hrd = !err;
		if (!err) {
			if (!sm.hfirst)
				strcat(sm.hbody, ", ");

			sm.hfirst = false;
			sprintf(sm.hbody + strlen(sm.hbody),
				"[\"%lu\", ", (unsigned long int) t);
		}
	}
	else if (sm.hrd) {
		err = print_register_full(ctx, regs[0].getter, 0,
					  append_vtxth, sm.hbody);

		if (err) {
			sm.hrd = err == EAGAIN;
		}
		else {
			sm.hrd = false;
			++sm.h;
			if (sm.h > 0) {
				strcat(sm.hbody, "]}");
#ifdef USE_MQTT
				fmqtt_publish(topics[0], sm.hbody);
#else
				printf("%s\n", sm.hbody);
#endif
			}
		}
	}

	if (!sm.drd && sm.d <= 0) {
		t = time(NULL);
		t += sm.d*24*3600;
		localtime_r(&t, &tm);
		snprintf(buf, sizeof(buf), "%04d%02d%02d",
			 tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
		err = register_write(ctx, regs[1].setter, 0, buf);
		sm.drd = !err;
		if (!err) {
			if (!sm.dfirst)
				strcat(sm.dbody, ", ");

			sm.dfirst = false;
			sprintf(sm.dbody + strlen(sm.dbody),
				"[\"%lu\", ", (unsigned long int) t);
		}
	}
	else if (sm.drd) {
		err = print_register_full(ctx, regs[1].getter, 0,
					  append_vtxth, sm.dbody);

		if (err) {
			sm.drd = err == EAGAIN;
		}
		else {
			sm.drd = false;
			++sm.d;
			if (sm.d > 0) {
				strcat(sm.dbody, "]}");
#ifdef USE_MQTT
				fmqtt_publish(topics[1], sm.dbody);
#else
				printf("%s\n", sm.dbody);
#endif
			}
		}
	}

	if (!sm.mrd && sm.m <= 0) {
		t = time(NULL);
		localtime_r(&t, &tm);
		int mon = tm.tm_mon + 1 + sm.m;
		int y = tm.tm_year + 1900;
		if (mon < 1) {
			--y;
			mon += 12;
		}

		snprintf(buf, sizeof(buf), "%04d%02d", y, mon);
		err = register_write(ctx, regs[2].setter, 0, buf);
		sm.mrd = !err;
		if (!err) {
			if (!sm.mfirst)
				strcat(sm.mbody, ", ");

			sm.mfirst = false;
			sprintf(sm.mbody + strlen(sm.mbody),
				"[\"%lu\", ", (unsigned long int) t);
		}
	}
	else if (sm.mrd) {
		err = print_register_full(ctx, regs[2].getter, 0,
					  append_vtxth, sm.mbody);
		if (err) {
			sm.mrd = err == EAGAIN;
		}
		else {
			sm.mrd = false;
			++sm.m;
			if (sm.m > 0) {
				strcat(sm.mbody, "]}");
#ifdef USE_MQTT
				fmqtt_publish(topics[2], sm.mbody);
#else
				printf("%s\n", sm.mbody);
#endif
			}
		}
	}

	if (!sm.yrd && sm.y <= 0) {
		t = time(NULL);
		localtime_r(&t, &tm);
		int yr = tm.tm_year + 1900 + sm.y;

		snprintf(buf, sizeof(buf), "%04d", yr);
		err = register_write(ctx, regs[3].setter, 0, buf);
		sm.yrd = !err;
		if (!err) {
			if (!sm.yfirst)
				strcat(sm.ybody, ", ");

			sm.yfirst = false;
			sprintf(sm.ybody + strlen(sm.ybody),
				"[\"%lu\", ", (unsigned long int) t);
		}
	}
	else if (sm.yrd) {
		err = print_register_full(ctx, regs[3].getter, 0,
					  append_vtxth, sm.ybody);
		if (err) {
			sm.yrd = err == EAGAIN;
		}
		else {
			sm.yrd = false;
			++sm.y;
			if (sm.y > 0) {
				strcat(sm.ybody, "]}");
#ifdef USE_MQTT
				fmqtt_publish(topics[3], sm.ybody);
#else
				printf("%s\n", sm.ybody);
#endif
			}
		}
	}

	return 0;
}
