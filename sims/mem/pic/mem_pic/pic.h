#include <simbricks/mem/if.h>
#include <simbricks/mem/proto.h>
#include <simbricks/pic/if.h>
#include <simbricks/pic/proto.h>

#include <simbricks/address_translation/address_translation.h>


static void sigint_handler(int dummy);
static void sigusr1_handler(int dummy);

bool MemIfInit(struct SimbricksMemIf *memif, const char *shm_path, struct SimbricksBaseIfParams *memParams);
bool PicIfInit(struct SimbricksPicIf *picif, const char *shm_path, struct SimbricksBaseIfParams *picParams);
