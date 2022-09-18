#ifndef FUTIL_H
#define FUTIL_H
#include <stdint.h>

#define ARRAY_SIZE(a) ((sizeof(a))/(sizeof((a)[0])))

void bin_u16(uint16_t v);
void ascii_u16(uint16_t v);
#endif
