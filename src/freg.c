#include <asm-generic/errno.h>
#include <errno.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <modbus.h>

#include "futil.h"
#include "freg.h"

enum {
	VALSIZE    = 32,
	VALPOS     = 70,
};

enum reg_type {
	REG_BOOL,
	REG_INT,
	REG_HEX,
	REG_ASCII
};

struct p17_register {
	uint16_t address;     /* modbus address                              */
	uint8_t bit;          /* first bit of register                       */
	uint8_t size;         /* number of bits                              */
	enum reg_type typ;    /* type of the value                           */
	const char *desc;     /* description                                 */
	const char *unit;     /* unit of the value                           */
	uint8_t div;          /* divisor                                     */
	bool ro;              /* read only flag                              */
	bool wo;              /* write only flag                             */
};


struct p17_register registers[] = {
{0,  0, 1, REG_BOOL, "AC input frequency loss", NULL, 0, true, false},
{0,  1, 1, REG_BOOL, "AC input voltage loss", NULL, 0, true, false},
{0,  2, 1, REG_BOOL, "AC input long-time average voltage over", NULL, 0, true,
	false},
{0,  3, 1, REG_BOOL, "Grid frequency low loss", NULL, 0, true, false},
{0,  4, 1, REG_BOOL, "Grid frequency high loss", NULL, 0, true, false},
{0,  5, 1, REG_BOOL, "Grid voltage low loss", NULL, 0, true, false},
{0,  6, 1, REG_BOOL, "Grid voltage high loss", NULL, 0, true, false},
{0,  7, 1, REG_BOOL, "Battery low in hybrid mode", NULL, 0, true, false},
{0,  8, 1, REG_BOOL, "Battery voltage too high", NULL, 0, true, false},
{0,  9, 1, REG_BOOL, "Battery open", NULL, 0, true, false},
{0, 10, 1, REG_BOOL, "Battery low", NULL, 0, true, false},
{0, 11, 1, REG_BOOL, "Battery under", NULL, 0, true, false},
{0, 12, 1, REG_BOOL, "Solar input 2 voltage too high", NULL, 0, true, false},
{0, 13, 1, REG_BOOL, "Solar input 1 voltage too high", NULL, 0, true, false},
{0, 14, 1, REG_BOOL, "Solar input 2 loss", NULL, 0, true, false},
{0, 15, 1, REG_BOOL, "Solar input 1 loss", NULL, 0, true, false},

{1,  4, 1, REG_BOOL, "Bat SCR temperature", NULL, 0, true, false},
{1,  5, 1, REG_BOOL, "Inv1 over temperature", NULL, 0, true, false},
{1,  6, 1, REG_BOOL, "Transfer OT temperature", NULL, 0, true, false},
{1,  7, 1, REG_BOOL, "SCR temperature Over", NULL, 0, true, false},
{1,  8, 1, REG_BOOL, "Inv0 Over temperature", NULL, 0, true, false},
{1,  9, 1, REG_BOOL, "DCDC Over temperature", NULL, 0, true, false},
{1, 10, 1, REG_BOOL, "AC input wave loss", NULL, 0, true, false},
{1, 11, 1, REG_BOOL, "EPO active", NULL, 0, true, false},
{1, 12, 1, REG_BOOL, "Over load", NULL, 0, true, false},
{1, 13, 1, REG_BOOL, "Over temperature", NULL, 0, true, false},
{1, 14, 1, REG_BOOL, "AC input phase dislocation", NULL, 0, true, false},
{1, 15, 1, REG_BOOL, "AC input island", NULL, 0, true, false},

{2,  8, 1, REG_BOOL, "Feeding Power over frequency de-rating", NULL, 0, false,
	false},
{2,  9, 1, REG_BOOL, "Feeding Power over voltage de-rating", NULL, 0, false,
	false},
{2, 10, 1, REG_BOOL, "Output Neutral line grounding in battery mode", NULL, 0,
	false, false},
{2, 11, 1, REG_BOOL, "Wide AC input range", NULL, 0, false, false},
{2, 12, 1, REG_BOOL, "Generator as AC input", NULL, 0, false, false},
{2, 13, 1, REG_BOOL, "Mute buzzer beep only on battery discharged status",
	NULL, 0, false, false},
{2, 14, 1, REG_BOOL, "Mute buzzer beep in standby mode", NULL, 0, false,
	false},
{2, 15, 1, REG_BOOL, "Mute buzzer beep", NULL, 0, false, false},

{7,  7, 1, REG_BOOL, "Enable/Disable Parallel for output", NULL, 0, false,
	false},
{7,  8, 1, REG_BOOL, "Enable/disable auto adjust PF "
	"according to Feed power", NULL, 0, false, false},
{7,  9, 1, REG_BOOL, "Enable/disable battery discharge to "
	"feed power to utility when solar input lost", NULL, 0, false, false},
{7, 10, 1, REG_BOOL, "Enable/disable battery discharge to feed power to "
	"utility when solar input normal", NULL, 0, false, false},
{7, 11, 1, REG_BOOL, "Enable/disable battery discharge to "
	"loads when solar input loss", NULL, 0, false, false},
{7, 12, 1, REG_BOOL, "Enable/disable battery discharge to "
	"loads when solar input normal", NULL, 0, false, false},
{7, 13, 1, REG_BOOL, "Enable/disable feed power to utility", NULL, 0, false,
	false},
{7, 14, 1, REG_BOOL, "Enable/disable AC charge battery", NULL, 0, false,
	false},
{7, 15, 1, REG_BOOL, "Enable/disable charge battery", NULL, 0, false, false},

{291, 0, 1, REG_BOOL, "Set enable/disable machine supply power to the loads",
	NULL, 0, false, true},

{292, 0, 1, REG_BOOL, "Enable/disable auto adjust PF according to Feed power",
	NULL, 0, false, false},

{284, 0, 1, REG_BOOL, "Enable/disable AC charger keep battery voltage "
	"function", NULL, 0, false, false},

{789, 0, 4, REG_HEX, "Solar energy distribution of priority", NULL, 0, false,
	false},
};


static const char *print_subline(const char *txt)
{
	size_t sz = VALPOS - 3;
	const char *p = txt + sz;

	for (; p > txt && p[0]!=' '; --p)
		--sz;

	printf("%.*s\n", sz, txt);
	sz = strlen(p);
	if (sz > 1024) {
		printf("ERR - description to long\n");
		return NULL;
	}

	if (sz > VALPOS - 3)
		return print_subline(p);
	else
		return p;
}


static void print_reg(struct p17_register *reg, uint16_t *val)
{
	char fill[VALPOS];
	char vtxt[VALSIZE * 2 + 1];
	int nb = reg->size / 16;
	const char *p;
	size_t sz;

	if (nb > VALSIZE)
		nb = VALSIZE;

	if (!nb)
		nb = 1;

	switch (reg->typ) {
	case REG_BOOL:
		sprintf(vtxt, "%s", (((uint16_t) 1) << reg->bit) & *val ?
			"YES": "NO");
		break;
	case REG_INT:
	case REG_HEX: {
		uint32_t v0 = val[0];
		uint32_t v;
		v = nb == 1 ? v0 : (v0 << 16) + val[1];
		if (reg->typ == REG_INT) {
			sprintf(vtxt, "%d", (int) v);
		}
		else {
			nb = reg->size / 4;
			if (nb > VALSIZE)
				nb = 4;
			if (!nb)
				nb = 1;
			sprintf(vtxt, "0x%0*x", nb, v);
		}
	        }
		break;
	case REG_ASCII:
		for (int i = 0; i < nb; ++i)
			memcpy(vtxt + 2 * i, val + i, 2);

		vtxt[2 * nb] = 0;
		break;
	default:
		printf("ERR - wrong register type %u\n", reg->typ);
		return;
		break;
	}

	p = reg->desc;
	sz = strlen(p);
	if (sz > VALPOS - 3) {
		p = print_subline(reg->desc);
		sz = strlen(p);
	}

	memset(fill, 0, sizeof(fill));
	memset(fill, ' ', VALPOS - sz);
	printf("%s%s%s\n", p, fill, vtxt);
}


int print_all_registers(modbus_t *ctx)
{
	size_t n = ARRAY_SIZE(registers);
	uint32_t addr = UINT16_MAX;
	uint16_t val[VALSIZE];
	int err = 0;

	for (size_t i = 0; i < n; ++i) {
		struct p17_register *reg = &registers[i];
		int nb = reg->size / 16;
		int mret = 0;

		if (!nb)
			nb = 1;

		if (nb > sizeof(val)) {
			printf("ERR - register size %u to big for buffer\n",
			       reg->size);
			return EINVAL;
		}

		if (reg->address != addr) {
			if (reg->wo)
				continue;

			addr = reg->address;
			memset(val, 0, sizeof(val));
			printf("\nQuery register 0x%04x\n", addr);
			for (int j = 0; j <= 80; ++j)
				printf("%c", j < 80 ? '=' : '\n');

			mret = modbus_read_registers(ctx, addr, nb, val);
			if (ctx && mret == -1) {
				perror("ERR - modbus read error\n");
				err = EPROTO;
				break;
			}
		}

		print_reg(reg, val);
	}

	return err;
}


int print_register(modbus_t *ctx, int addr)
{
	size_t n = ARRAY_SIZE(registers);
	uint16_t val[VALSIZE];
	int err = 0;
	for (size_t i = 0; i < n; ++i) {
		struct p17_register *reg = &registers[i];
		int nb = reg->size / 16;
		int mret;
		if (reg->address != addr)
			continue;

		if (reg->wo) {
			printf("ERR - Register 0x%04x is write only\n", addr);
			return EINVAL;
		}

		mret = modbus_read_registers(ctx, addr, nb, val);
		if (ctx && mret == -1) {
			perror("ERR - modbus read error\n");
			err = EPROTO;
			break;
		}

		print_reg(reg, val);
		break;
	}

	return err;
}