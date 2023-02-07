#ifndef FUTIL_H
#define FUTIL_H
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define ARRAY_SIZE(a) ((sizeof(a))/sizeof((a)[0]))

void bin_u16(uint16_t v);
void ascii_u16(uint16_t v);
uint16_t rand_u16(void);
uint32_t rand_u32(void);
uint64_t tmr_jiffies(void);
bool tmr_slot(uint64_t *slotp, uint64_t inc);
#endif
