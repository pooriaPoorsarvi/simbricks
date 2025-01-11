#include "pic.h"


bool MemIfInitConnect(struct SimbricksMemIf *memif, struct SimbricksBaseIfParams *memParams);
bool PicIfInitListen(struct SimbricksPicIf *picif, const char *shm_path, struct SimbricksBaseIfParams *picParams);

volatile union SimbricksProtoMemH2M *H2MAlloc(struct SimbricksMemIf *memif);
volatile union SimbricksProtoPicR2S *PICR2SAlloc(struct SimbricksPicIf *picif);

volatile union SimbricksProtoMemM2H *PollM2H(struct SimbricksMemIf *memif);
volatile union SimbricksProtoPicS2R *PollPicS2R(struct SimbricksPicIf *picif);

void HandleSimbricksPicS2RMsg(struct SimbricksMemIf *memif, struct SimbricksPicIf *picif, volatile union SimbricksProtoMemH2M *msg);
void HandleSimbricksPicS2RReadRequestMsg(struct SimbricksMemIf *memif, struct SimbricksPicIf *picif, volatile union SimbricksProtoPicS2R *cpu_pic_to_mem_pic_msg);
void HandleSimbricksPicS2RWriteRequestMsg(struct SimbricksMemIf *memif, struct SimbricksPicIf *picif, volatile union SimbricksProtoPicS2R *cpu_pic_to_mem_pic_msg);