#include "lib/simbricks/pic/if.h"

void SimbricksPicIfDefaultParams(struct SimbricksBaseIfParams *params) {
  SimbricksBaseIfDefaultParams(params);
  params->upper_layer_proto = SIMBRICKS_PROTO_ID_PIC;
  // fit DMA writes with size 8192
  params->in_entries_size = params->out_entries_size =
      8192 + sizeof(union SimbricksProtoPicS2R);
}
