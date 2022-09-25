#ifndef FCONF_H
#define FCONF_H

#include <stdint.h>


struct fconf {
	char *ttydev;
	int stopbits;
	int slaveid;
	char *mqtthost;
	int   mqttport;
	char *mqttuser;
	char *mqttpass;
	char *capath;
	char *cafile;
	char *publish[10];
};


int read_fconfig(struct fconf *conf, const char *fname);
int write_conf_file(struct fconf *conf, const char *fname);
void free_fconfig(struct fconf *conf);

#endif
