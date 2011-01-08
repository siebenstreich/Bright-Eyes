/*
 *	transitional functions for version 3.02_de
 *
 *	Here are function, which should make the transition from
 *	DOXBox emulation to native easier.
 *	These functions will only work with the version mentioned above.
 */

#include "schick.h"

#define TEXT_LTX	(0xc3b5)

static inline Bit8u *get_ltx(unsigned short off) {
	return MemBase + Real2Phys(mem_readd(Real2Phys(ds_readd(TEXT_LTX) + off)));
}