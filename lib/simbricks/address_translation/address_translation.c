#include "address_translation.h"

uint128_t translate_local_to_global(uint64_t local){
    return local;
}

uint64_t translate_global_to_local(uint128_t global){
    uint128_t bitmask = 1;
    bitmask = (bitmask << 64) - 1;
    return global & bitmask;
}

