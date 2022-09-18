#ifndef FCONF_H
#define FCONF_H

struct fconf {
	char *ttydev;
	int stopbits;
	int slaveid;
};

int read_fconfig(struct fconf *conf, const char *fname);
int write_conf_file(struct fconf *conf, const char *fname);
void free_fconfig(struct fconf *conf);

#endif
