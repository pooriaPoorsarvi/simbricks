#include "pic.h"


bool MemIfInitListen(struct SimbricksMemIf *memif, const char *shm_path, struct SimbricksBaseIfParams *memParams);
bool PicIfInitConnect(struct SimbricksPicIf *picif, struct SimbricksBaseIfParams *picParams);

volatile union SimbricksProtoMemM2H *M2HAlloc(struct SimbricksMemIf *memif);
volatile union SimbricksProtoPicS2R *PICS2RAlloc(struct SimbricksPicIf *picif);

volatile union SimbricksProtoMemH2M *PollH2M(struct SimbricksMemIf *memif);
volatile union SimbricksProtoPicR2S *PollPicR2S(struct SimbricksPicIf *picif);

void HandleSimbricksH2MMsg(struct SimbricksMemIf *memif, struct SimbricksPicIf *picif, volatile union SimbricksProtoMemH2M *msg);
void HandleSimbricksH2MReadRequestMsg(struct SimbricksMemIf *memif, struct SimbricksPicIf *picif, volatile union SimbricksProtoMemH2M *msg);
void HandleSimbricksH2MWriteRequestMsg(struct SimbricksMemIf *memif, struct SimbricksPicIf *picif, volatile union SimbricksProtoMemH2M *msg);