#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>

#include "fconf.h"

#define TTYDEV   "/dev/ttyUSB0"

typedef int (conf_valset_h)(struct fconf *conf, const char *val);
typedef const char* (conf_getstr_h)(struct fconf *conf);
typedef int (conf_getint_h)(struct fconf *conf);

enum conftyp {
	CONF_INT,
	CONF_STR
};

struct confkey {
	const char *key;
	enum conftyp typ;
	int vidx;
	const char *defstr;
	int defint;
};

struct fconf fconfd;


static int set_str(struct fconf *conf, int vidx, const char *val)
{
	char **p;

	if (!conf || !val)
		return EINVAL;

	p = (char **) (((uint8_t *) conf) + vidx);
	*p = strdup(val);
	if (!*p)
		return ENOMEM;

	return 0;
}


static void free_str(struct fconf *conf, int vidx)
{
	char **p;

	if (!conf)
		return;

	p = (char **) (((uint8_t *) conf) + vidx);
	if (*p)
		free(*p);
}


static int set_int(struct fconf *conf, int vidx, int v)
{
	int *p;

	if (!conf)
		return EINVAL;

	p = (int *) (((uint8_t *) conf) + vidx);
	*p = v;

	return 0;
}


static int set_val(struct fconf *conf, const struct confkey *ck,
		   const char *val)
{
	switch (ck->typ) {
		case CONF_INT:
			return set_int(conf, ck->vidx, atoi(val));
		case CONF_STR:
			return set_str(conf, ck->vidx, val);
		default:
			return EINVAL;
	}
}


static const char *get_str(const struct fconf *conf, int vidx)
{
	const char *p = *((char **) (((uint8_t *) conf) + vidx));

	return p;
}


static int get_int(const struct fconf *conf, int vidx)
{
	return *((int *) (((uint8_t *) conf) + vidx));
}


int fprint_conf(FILE *f, const struct fconf *conf, const struct confkey *ck)
{
	int n = 0;

	switch (ck->typ) {
		case CONF_INT:
			n = fprintf(f, "%s   %d\n", ck->key,
				get_int(conf, ck->vidx));
			break;
		case CONF_STR:
			n = fprintf(f, "%s   %s\n", ck->key,
				get_str(conf, ck->vidx));
			break;
		default:
			break;
	}

	return n;
}


#define FCONF_VIDX(fld) \
	(int) ((uint8_t *) &fconfd.fld - (uint8_t *) &fconfd)


struct confkey confkeys[] = {

{"ttydev",   CONF_STR, FCONF_VIDX(ttydev),   "/dev/ttyUSB0",    0},
{"stopbits", CONF_INT, FCONF_VIDX(stopbits),           NULL,    2},
{"slaveid",  CONF_INT, FCONF_VIDX(slaveid),            NULL,   10},

};


static char *find_begin(char *start)
{
	char *b;

	if (!start)
		return NULL;

	b = start;
	while (b[0] == ' ' && b[0] !=0)
		++b;

	return b;
}


static char *find_end(char *start)
{
	char *e;

	if (!start)
		return NULL;

	e = start;
	while (e[0] != ' ' && e[0] != 0 && e[0] != '\n' && e[0] != '\r')
		++e;

	e[0] = 0;
	if(e == start)
		return NULL;

	return e;
}


static int conf_proc_line(struct fconf *conf, const char *line)
{
	char *buf;
	char *key;
	char *val;
	char *e;
	size_t n = sizeof(confkeys)/sizeof(struct confkey);
	int err = 0;

	buf = strdup(line);
	if (!buf)
		return ENOMEM;

	key = find_begin(buf);
	e   = find_end(key);
	if (e==NULL)
		return 0;

	val = find_begin(++e);
	e   = find_end(val);
	if (e==NULL)
		return 0;

	for (size_t i = 0; i < n; i++) {
		const struct confkey *ck;

		ck = &confkeys[i];
		if (!strcmp(ck->key, key))
			err = set_val(conf, ck, val);

		if (err)
			break;
	}

	free(buf);
	return err;
}


int write_conf_file(struct fconf *conf, const char *fname)
{
	FILE *f;
	size_t n = sizeof(confkeys)/sizeof(struct confkey);
	int err = 0;

	if (!conf || !fname)
		return EINVAL;

	f = fopen(fname, "w");
	if (!f) {
		printf("ERR - Could not write config file %s\n", fname);
		return EIO;
	}

	for (size_t i = 0; i < n; i++) {
		const struct confkey *ck;

		ck = &confkeys[i];
		if (fprint_conf(f, conf, ck) < 3) {
			err = EIO;
			break;
		}
	}

	fclose(f);
	return err;
}


static int write_template(struct fconf *conf, const char *fname)
{
	size_t n = sizeof(confkeys)/sizeof(struct confkey);
	int err;

	/* set default config */
	for (size_t i = 0; i < n; ++i) {
		const struct confkey *ck;

		ck = &confkeys[i];
		switch (ck->typ) {
			case CONF_INT:
				err = set_int(conf, ck->vidx, ck->defint);
			break;
			case CONF_STR:
				err = set_str(conf, ck->vidx, ck->defstr);
			break;
			default:
				return EINVAL;
		}

		if (err)
			return err;
	}

	/* write default config */
	err = write_conf_file(conf, fname);
	if (err)
		free_fconfig(conf);

	return err;
}


int read_fconfig(struct fconf *conf, const char *fname)
{
	FILE *f;
	char *line = NULL;
	size_t len = 0;
	int err;

	if (!conf || !fname)
		return EINVAL;

	f = fopen(fname, "r");
	if (!f) {
		printf("INF - Generating new config file %s\n", fname);
		err = write_template(conf, fname);
		return err;
	}

	while (getline(&line, &len, f) != -1) {
		err = conf_proc_line(conf, line);
		if (err)
			goto out;
	}
	
out:
	free(line);
	fclose(f);
	return 0;
}


void free_fconfig(struct fconf *conf)
{
	size_t n = sizeof(confkeys)/sizeof(struct confkey);
	if (!conf)
		return;

	for (size_t i = 0; i < n; ++i) {
		const struct confkey *ck;

		ck = &confkeys[i];
		if (ck->typ == CONF_STR)
				free_str(conf, ck->vidx);
	}
}
