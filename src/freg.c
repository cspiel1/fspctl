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
	REG_NUM,
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

struct p17_reg_group {
	const char *name;
	uint16_t address;
};


struct p17_register registers[] = {

/* 1. Warning items */
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

/* 2. Enable/Disable items */
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

/* 3. Setting Energy priority */
{789, 0, 4, REG_HEX, "Solar energy distribution of priority", NULL, 0, false,
	false},

{26, 10, 1, REG_BOOL, "command for fault recovery", NULL, 0, false, false},

{59, 15, 1, REG_BOOL, "Setting control parameter to default value", NULL, 0,
	false, true},

{379, 15, 1, REG_BOOL, "Li-Fe battery self-test by charged at a time", NULL, 0,
	false, true},

/* 5. Working mode */
{1010, 0, 16, REG_NUM, "Battery piece number", NULL, 0, true, false},
{1011, 0, 16, REG_NUM, "Battery standard voltage per unit", "V", 10, true,
	false},
{1012, 0, 16, REG_NUM, "AC input phase number", NULL, 0, true, false},
{1013, 0, 16, REG_NUM, "AC output phase number", NULL, 0, true, false},
{1014, 0, 16, REG_NUM, "Nominal AC input voltage",  "V", 10, true, false},
{1015, 0, 16, REG_NUM, "Nominal AC output voltage", "V", 10, true, false},
{1016, 0, 16, REG_NUM, "Output power factor", NULL , 0, true, false},
{1017, 0, 32, REG_NUM, "Output rated VA", "VA", 0, true, false},
{1019, 0, 32, REG_ASCII, "Machine number", NULL, 0, true, false},

{208, 0, 4, REG_HEX, "Working mode", NULL, 0, true, false},

/* 6. Working status */
{211, 0, 32, REG_NUM, "AC input active power R", "W", 0, true, false},
{257, 0, 32, REG_NUM, "AC input active power S", "W", 0, true, false},
{259, 0, 32, REG_NUM, "AC input active power T", "W", 0, true, false},
{377, 0, 32, REG_NUM, "AC input total active power", "W", 0, true, false},
{798, 0, 16, REG_NUM, "AC input voltage R", "V", 10, true, false},
{799, 0, 16, REG_NUM, "AC input voltage S", "V", 10, true, false},
{800, 0, 16, REG_NUM, "AC input voltage T", "V", 10, true, false},
{801, 0, 16, REG_NUM, "AC input frequency", "Hz", 100, true, false},

{614, 0, 16, REG_NUM, "AC input current R", "A", 10, true, false},
{616, 0, 16, REG_NUM, "AC input current S", "A", 10, true, false},
{618, 0, 16, REG_NUM, "AC input current T", "A", 10, true, false},

{903, 0, 8, REG_ASCII, "AC output connect status", NULL, 0, true, false},
{217, 0, 32, REG_NUM, "AC output active power R", "W", 0, true, false},
{241, 0, 32, REG_NUM, "AC output active power S", "W", 0, true, false},
{243, 0, 32, REG_NUM, "AC output active power T", "W", 0, true, false},
{245, 0, 32, REG_NUM, "AC output total active power", "W", 0, true, false},
{864, 0, 16, REG_NUM, "AC output power percentage", "%", 0, true, false},
{781, 0, 32, REG_NUM, "AC output apparent power R", "VA", 0, true, false},
{894, 0, 32, REG_NUM, "AC output apparent power S", "VA", 0, true, false},
{1241, 0, 32, REG_NUM, "AC output apparent power T", "VA", 0, true, false},
{984, 0, 32, REG_NUM, "AC output total power", "VA", 0, true, false},

{216, 0, 16, REG_NUM, "AC output voltage R", "V", 10, true, false},
{247, 0, 16, REG_NUM, "AC output voltage S", "V", 10, true, false},
{248, 0, 16, REG_NUM, "AC output voltage T", "V", 10, true, false},

{219, 0, 16, REG_NUM, "AC output frequency", "Hz", 100, true, false},
{220, 0, 16, REG_NUM, "AC output current R", "A", 10, true, false},
{239, 0, 16, REG_NUM, "AC output current S", "A", 10, true, false},
{240, 0, 16, REG_NUM, "AC output current T", "A", 10, true, false},

{904, 8, 8, REG_ASCII,  "Battery power direction", NULL, 0, true, false},
{226, 0, 16, REG_NUM, "Battery capacity", "%", 0, true, false},
{230, 0, 32, REG_NUM, "Battery current", "A", 10, true, false},
{188, 0, 16, REG_NUM, "Battery voltage", "V", 10, true, false},
{1196, 0, 32, REG_NUM, "Battery power", "W", 0, true, false},
{204, 0, 16, REG_NUM, "External battery temperature", NULL, 0, true, false},

{903, 8, 8, REG_ASCII, "Solar input 1 work status", NULL, 0, true, false},
{904, 0, 8, REG_ASCII, "Solar input 2 work status", NULL, 0, true, false},
{228, 0, 32, REG_NUM, "Solar input power 1", "W", 0, true, false},
{232, 0, 32, REG_NUM, "Solar input power 2", "W", 0, true, false},
{234, 0, 16, REG_NUM, "Solar input voltage 1", "V", 10, true, false},
{235, 0, 16, REG_NUM, "Solar input voltage 2", "V", 10, true, false},

{696, 0, 16, REG_NUM, "Solar input current 1", "A", 10, true, false},
{698, 0, 16, REG_NUM, "Solar input current 2", "A", 10, true, false},

{905, 0, 8, REG_ASCII,  "DC/AC power direction", NULL, 0, true, false},
{905, 8, 8, REG_ASCII,  "Line power direction", NULL, 0, true, false},

{906, 0, 1, REG_BOOL, "Setting change bit flag", NULL, 0, false, false},

{892, 0, 16, REG_NUM, "Inner temperature", NULL, 0, true, false},
{237, 0, 16, REG_NUM, "Component max temperature", NULL, 0, true, false},

/* 7. Time information */
{275, 0, 112, REG_ASCII, "Time", NULL, 0, false, false},

/* 11. CPU information */
{992, 0, 16, REG_ASCII, "Protocol ID Inquiry", NULL, 0, true, false},
{993, 0, 64, REG_ASCII, "Main CPU Firmware version", NULL, 0, true, false},
{1180, 0, 64, REG_ASCII, "Secondary CPU Firmware version", NULL, 0, true,
	false},
{1042, 0, 112, REG_ASCII, "DSP Firmware Build date", NULL, 0, true, false},
{1049, 0, 112, REG_ASCII, "MCU Firmware Build date", NULL, 0, true, false},

/* 12. Output power */
{1250, 0, 16, REG_NUM, "R phase Feeding grid calibration power",
	NULL, 0, false, false},
{1251, 0, 16, REG_NUM, "S phase Feeding grid calibration power",
	NULL, 0, false, false},
{1252, 0, 16, REG_NUM, "T phase Feeding grid calibration power",
	NULL, 0, false, false},
{1253, 0, 16, REG_NUM, "The max power limit to feed to grid",
	NULL, 0, false, false},
{1459, 0, 16, REG_NUM, "Feeding grid calibration power", NULL, 0, false,
	false},
{861, 0, 16, REG_NUM, "Feed- in power factor", NULL, 0, false, false},

/* 13. LCD sleep time */
{1041, 0, 16, REG_NUM, "The LCD sleep time inquiry or set", NULL, 0, false,
	false},

/* 14. Battery information */
{249, 0, 16, REG_NUM, "Battery stop charger current level in floating "
	"charging", "A", 10, false, false},
{250, 0, 16, REG_NUM, "Keep charged time of battery catch stopped charging "
	"current level", "Min", 0, false, false},
{251, 0, 16, REG_NUM, "Battery voltage of recover to charge when battery stop "
	"charger in floating charging", "V", 10, false, false},
{252, 0, 16, REG_NUM, "Battery under voltage", "V", 10, false, false},
{253, 0, 16, REG_NUM, "Battery under back voltage", "V", 10, false, false},
{254, 0, 16, REG_NUM, "Battery weak voltage in hybrid mode", "V", 10, false,
	false},
{282, 0, 16, REG_NUM, "Battery weak back voltage in hybrid mode", "V", 10,
	false, false},
{283, 0, 16, REG_NUM, "Battery max. discharge current in hybrid mode", "A", 10,
	true, false},
{285, 0, 16, REG_NUM, "AC charger keep battery voltage", "V", 10, false,
	false},
{286, 0, 16, REG_NUM, "Battery temperature sensor compensation", "mV", 10,
	false, false},
{287, 0, 16, REG_NUM, "Max. AC charging current", "A", 10, false, false},
{606, 0, 112, REG_ASCII, "Battery install time", NULL, 0, false, false},
{623, 0, 16, REG_NUM, "Battery constant charge voltage(C.V.)", "V", 10,
	false, false},
{624, 0, 16, REG_NUM, "Battery floating charge voltage", "V", 10,
	false, false},
{1240, 0, 16, REG_NUM, "Battery discharge max current in hybrid mode", "A", 0,
	false, false},
{1457, 0, 16, REG_NUM, "Battery maximum charge current", "A", 10, false,
	false},
{1478, 0, 16, REG_NUM, "Battery type", NULL, 0, false, false},

/* 15. MPPT information */
{1485, 0, 16, REG_NUM, "Solar input MPPT highest voltage", "V", 10,
	false, false},
{1486, 0, 16, REG_NUM, "Solar input MPPT lowest voltage", "V", 10,
	false, false},

/* 16. Default information */
{183, 0, 16, REG_NUM, "Default: Battery weak back voltage in hybrid mode", "V", 10,
	true, false},
{184, 0, 16, REG_NUM, "Default: Battery stop charger current level in floating "
	"charging", "A", 10, true, false},
{192, 0, 16, REG_NUM, "Default: Battery voltage of recover to charge when battery stop "
	"charger in floating charging", "V", 10, true, false},
{193, 0, 16, REG_NUM, "Default: Battery under back voltage", "V", 10, true, false},
{194, 0, 16, REG_NUM, "Default: Battery float charge voltage", "V", 10, true, false},
{195, 0, 16, REG_NUM, "Default: Battery weak voltage in hybrid mode", "V", 10, true,
	false},
{197, 0, 16, REG_NUM, "Default: Keep charged time of battery catch stop charger "
	"current level", "Min", 0, true, false},
{854, 0, 16, REG_NUM, "Default: The wait time for feed power", "Sec", 0, true, false},
{866, 0, 16, REG_NUM, "Default: AC input long-time highest average voltage", "V", 10,
	true, false},
{1039, 0, 16, REG_NUM, "Default: Solar input highest voltage", "V", 10, true, false},
{1040, 0, 16, REG_NUM, "Default: Solar input lowest voltage", "V", 10, true, false},
{1194, 0, 16, REG_NUM, "Default: Solar input highest MPPT voltage", "V", 10, true,
	false},
{1195, 0, 16, REG_NUM, "Default: Solar input lowest MPPT voltage", "V", 10, true,
	false},
{1199, 0, 16, REG_NUM, "Default: LCD sleep wait time", NULL, 0, true, false},
{1256, 0, 32, REG_ASCII, "Default: Start time for support loads", NULL, 0, true, false},
{1258, 0, 32, REG_ASCII, "Default: Ending time for support loads", NULL, 0, true,
	false},
{1260, 0, 32, REG_ASCII, "Default: Start time for AC charger", NULL, 0, true, false},
{1262, 0, 32, REG_ASCII, "Default: Ending time for AC charger", NULL, 0, true, false},
{1460, 0, 16, REG_NUM, "Default: Battery maximum charge current", "A", 10,
	true, false},
{1477, 0, 16, REG_NUM, "Default: Battery under voltage", "V", 10, true, false},
{1487, 0, 16, REG_NUM, "Default: AC input highest voltage for feed power", "V",
	10, true, false},
{1488, 0, 16, REG_NUM, "Default: AC input lowest voltage for feed power", "V",
	10, true, false},
{1489, 0, 16, REG_NUM, "Default: AC input highest frequency for feed power",
	"Hz", 100, true, false},
{1490, 0, 16, REG_NUM, "Default: AC input lowest frequency for feed power",
	"Hz", 100, true, false},
{1496, 0, 16, REG_NUM, "Default: Battery constant charge voltage(C.V.)",
	"V", 10, true, false},

/* 17. Feeding wait time */
{288, 0, 16, REG_NUM, "The feeding wait time", "Sec", 0, false, false},


/* 18. Getting range information */
{198, 0, 16, REG_NUM, "The upper limit of AC input highest voltage for feed "
	"power", "V", 10, true, false},
{199, 0, 16, REG_NUM, "The lower limit of AC input highest voltage for feed "
	"power", "V", 10, true, false},
{200, 0, 16, REG_NUM, "The upper limit of AC input lowest voltage for feed "
	"power", "V", 10, true, false},
{201, 0, 16, REG_NUM, "The lower limit of AC input lowest voltage for feed "
	"power", "V", 10, true, false},
{202, 0, 16, REG_NUM, "The upper limit of AC input highest frequency for feed "
	"power", "Hz", 100, true, false},
{203, 0, 16, REG_NUM, "The lower limit of AC input highest frequency for feed "
	"power", "Hz", 100, true, false},
{263, 0, 16, REG_NUM, "The upper limit of battery charged voltage", "V", 10,
	true, false},
{264, 0, 16, REG_NUM, "The lower limit of battery charged voltage", "V", 10,
	true, false},
{265, 0, 16, REG_NUM, "The upper limit of battery Max. charged current", "A",
	10, true, false},
{266, 0, 16, REG_NUM, "The lower limit of battery Max. charged current", "A",
	10, true, false},
{267, 0, 16, REG_NUM, "The lower limit of solar minimum input voltage", "V",
	10, true, false},
{268, 0, 16, REG_NUM, "The upper limit of AC input lowest frequency for feed "
	"power", "Hz", 100, true, false},
{269, 0, 16, REG_NUM, "The lower limit of AC input lowest frequency for feed "
	"power", "Hz", 100, true, false},
{270, 0, 16, REG_NUM, "The upper limit of wait time for feed power", "Sec", 0,
	true, false},
{271, 0, 16, REG_NUM, "The lower limit of wait time for feed power", "Sec", 0,
	true, false},
{272, 0, 16, REG_NUM, "The upper limit of solar maximum input voltage", "V",
	10, true, false},
{273, 0, 16, REG_NUM, "The lower limit of solar maximum input voltage", "V",
	10, true, false},
{274, 0, 16, REG_NUM, "The upper limit of solar minimum input voltage", "V",
	10, true, false},
{365, 0, 32, REG_NUM, "The upper limit of maximum feeding power", "W", 0, true,
	false},
{367, 0, 32, REG_NUM, "The lower limit of maximum feeding power", "W", 0, true,
	false},
{1235, 0, 16, REG_NUM, "The upper limit of solar maximum MPPT voltage", "V",
	10, true, false},
{1236, 0, 16, REG_NUM, "The lower limit of solar maximum MPPT voltage", "V",
	10, true, false},
{1237, 0, 16, REG_NUM, "The upper limit of solar minimum MPPT voltage", "V",
	10, true, false},
{1238, 0, 16, REG_NUM, "The lower limit of solar minimum MPPT voltage", "V",
	10, true, false},

/* 19. PV model and rating information */

{1003, 0, 64, REG_ASCII, "Series number", NULL, 0, true, false},
{293, 0, 16, REG_NUM, "AC input rated frequency", "Hz", 10, false, false},
{295, 0, 16, REG_NUM, "AC input rated current", "A", 10, true, false},
{1162, 0, 16, REG_NUM, "Battery rated voltage", "V", 10, true, false},
{1171, 0, 16, REG_NUM, "AC output rated voltage", "V", 10, false, false},
{1172, 0, 16, REG_NUM, "AC output rated current", "A", 10, true, false},
{1175, 0, 16, REG_NUM, "MPPT rated current per string", "A", 10, true, false},
{1177, 0, 16, REG_NUM, "MPPT track number", NULL, 0, true, false},
{1178, 0, 16, REG_ASCII, "Machine type", NULL, 0, true, false},
{1179, 0, 16, REG_NUM, "Topology", NULL, 0, true, false},
{1192, 0, 16, REG_NUM, "AC input rated voltage", "V", 10, true, false},
{1200, 0, 16, REG_NUM, "Start power percentage of auto-adjusting", "%", 0,
	false, false},
{1201, 0, 16, REG_NUM, "Minmum PF value when power percentage reach 100%", "V",
	100, false, false},

};


struct p17_reg_group groups[] = {
	{"1. Warning items",            0x0000},
	{"2. Enable/Disable items",     0x0002},
	{"3. Setting Energy priority",  0x0315},
	{"4. Control Items",            0x001A},
	{"5. Working mode",             0x03f2},
	{"6. Working status",           0x00d3},
	{"7. Time information",         0x0113},
	{"11. CPU information",         0x03E0},
	{"12. Output power",            0x04E2},
	{"13. LCD sleep time",          0x0411},
	{"14. Battery information",     0x00f9},
	{"15. MPPT information",        0x05cd},
	{"16. Default information",     0x00b7},
	{"17. Feeding wait time",       0x0120},
	{"18. Getting range information",       0x00c6},
	{"19. PV model and rating information", 0x03eb},
};


static void print_title(uint32_t address)
{
	size_t n = ARRAY_SIZE(groups);

	for (size_t i = 0; i < n; ++i) {
		struct p17_reg_group *g = &groups[i];
		if (g->address == address) {
			printf("\n%s\n", g->name);
			for (int j = 0; j < 80; ++j)
				printf("%c", '=');

			printf("\n");
		}
	}
}


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


static void convert_num(char *txt,
			const struct p17_register *reg, uint16_t *val)
{
	int nb;
	uint32_t v0 = val[0];
	uint32_t v;

	nb = reg->size / 16;
	if (nb > VALSIZE) nb = VALSIZE;
	else if (!nb) nb = 1;

	v = nb == 1 ? v0 : (v0 << 16) + val[1];

	if (reg->div) {
		int r = ((int) v) / reg->div;
		int c = ((int) v) % reg->div;
		if (c < 0)
			c *= -1;
		sprintf(txt, "%d.%d", r, c);
	}
	else {
		sprintf(txt, "%d", (int) v);
	}
}


static void convert_hex(char *txt,
			const struct p17_register *reg, uint16_t *val)
{
	int nb;
	uint32_t v0 = val[0];
	uint32_t v;

	nb = reg->size / 16;
	if (nb > VALSIZE) nb = VALSIZE;
	else if (!nb) nb = 1;

	v = nb == 1 ? v0 : (v0 << 16) + val[1];

	nb = reg->size / 4;
	if (nb > VALSIZE) nb = 4;
	else if (!nb) nb = 1;

	sprintf(txt, "0x%0*x", nb, v);
}


static void convert_txt(char *txt,
			const struct p17_register *reg, uint16_t *val)
{
	int nb;

	nb = reg->size / 8;

	for (int i = 0; i < nb; ++i) {
		txt[i] = (i % 2 == 0) && (i + 1 < nb) ?
			(char) (val[i/2] >> 8) :
			(char) val[i/2];
	}

	txt[nb] = 0;
}


static void print_reg(struct p17_register *reg, uint16_t *val)
{
	char fill[VALPOS];
	char vtxt[VALSIZE * 2 + 1];
	const char *p;
	size_t sz;

	switch (reg->typ) {
	case REG_BOOL:
		sprintf(vtxt, "%s", (((uint16_t) 1) << reg->bit) & *val ?
			"YES": "NO");
		break;
	case REG_NUM:
		convert_num(vtxt, reg, val);
		break;
	case REG_HEX:
		convert_hex(vtxt, reg, val);
		break;
	case REG_ASCII:
		convert_txt(vtxt, reg, val);
		break;
	default:
		printf("ERR - wrong register type %u\n", reg->typ);
		return;
		break;
	}

	if (reg->unit)
		strcat(vtxt, reg->unit);

	p = reg->desc;
	sz = strlen(p);
	if (sz > VALPOS - 3) {
		p = print_subline(reg->desc);
		sz = strlen(p);
	}

	memset(fill, 0, sizeof(fill));
	memset(fill, ' ', VALPOS - sz);
	printf("%04u: %s%s%s\n", reg->address, p, fill, vtxt);
/*        printf("   %04x\n", val[0]);*/
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
			print_title(addr);

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
	bool read = false;
	int mret;
	int err = 0;

	for (size_t i = 0; i < n; ++i) {
		struct p17_register *reg = &registers[i];
		int nb;
		if (reg->address != addr)
			continue;

		nb = reg->size / 16;
		if (!nb)
			nb = 1;
		else if (nb > sizeof(val)) {
			printf("ERR - register size %u to big for buffer\n",
			       reg->size);
			return EINVAL;
		}

		if (reg->wo) {
			printf("ERR - Register 0x%04x is write only\n", addr);
			return EINVAL;
		}

		if (!read) {
			read = true;
			mret = modbus_read_registers(ctx, addr, nb, val);
			if (ctx && mret == -1) {
				perror("ERR - modbus read error\n");
				err = EPROTO;
				break;
			}
		}

		print_reg(reg, val);
	}

	if (!err && !read) {
		mret = modbus_read_registers(ctx, addr, 1, val);
		if (ctx && mret != -1) {
			printf("Unknown register %04x:    %04x\n",
			       addr, val[0]);
		}
	}

	return err;
}
