
#include <stdlib.h>
#include <stdio.h>


/*
 * Because this location of this RAM spans the bit 16 boundary
 * it is easiser to just allocate the full 64K.  Spares doing
 * math later on.
 *
 * Likewise, the data pins are NOT sequential due to gaps on the
 * pico.
 *
 * Could do bunches of bit shifting, or we could just treat them
 * all as 16-bit bytes.
 *
 * Go for the speedy option.
 *
 * Besides, what else would we use the memory for?
 */


uint16_t rom_contents[32768*2] = {};
