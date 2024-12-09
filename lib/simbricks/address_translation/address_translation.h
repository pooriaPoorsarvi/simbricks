#include <stdio.h>
#include <stdint.h>

typedef __uint128_t uint128_t;

uint128_t translate_local_to_global(uint64_t local);
uint64_t translate_global_to_local(uint128_t global);