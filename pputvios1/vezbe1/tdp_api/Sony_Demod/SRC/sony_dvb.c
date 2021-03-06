/*------------------------------------------------------------------------------

 <dev:header>
    Copyright(c) 2009 Sony Corporation.

    $Revision: 838 $
    $Author: sking $

</dev:header>

------------------------------------------------------------------------------*/

#include "sony_dvb.h"

/* Utility function */
#define MASKUPPER(n) ((n) == 0 ? 0 : 0xFFFFFFFFU << (32 - (n)))
#define MASKLOWER(n) ((n) == 0 ? 0 : 0xFFFFFFFFU >> (32 - (n)))
/* Convert N (<32) bit 2's complement value to 32 bit signed value */
int32_t dvb_Convert2SComplement (uint32_t value, uint32_t bitlen)
{
    if (bitlen == 0 || bitlen >= 32) {
        return (int32_t) value;
    }

    if (value & (1 << (bitlen - 1))) {
        /* minus value */
        return (int32_t) (MASKUPPER (32 - bitlen) | value);
    }
    else {
        /* plus value */
        return (int32_t) (MASKLOWER (bitlen) & value);
    }
}
