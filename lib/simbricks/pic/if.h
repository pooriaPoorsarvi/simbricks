#ifndef SIMBRICKS_PIC_IF_H_
#define SIMBRICKS_PIC_IF_H_

#include <stddef.h>
#include <stdint.h>

#include <simbricks/base/generic.h>
#include <simbricks/base/if.h>
#include <simbricks/pic/proto.h>

void SimbricksPicIfDefaultParams(struct SimbricksBaseIfParams *params);

struct SimbricksPicIf {
  struct SimbricksBaseIf base;
};

/** Generate queue access functions for both directions */
SIMBRICKS_BASEIF_GENERIC(SimbricksPicIfS2R, SimbricksProtoPicS2R,
                         SimbricksPicIf);
SIMBRICKS_BASEIF_GENERIC(SimbricksPicIfR2S, SimbricksProtoPicR2S,
                         SimbricksPicIf);

#endif  // SIMBRICKS_PIC_IF_H_
