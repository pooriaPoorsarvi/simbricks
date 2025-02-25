#ifndef SIMBRICKS_PIC_PROTO_H_
#define SIMBRICKS_PIC_PROTO_H_

#include <assert.h>
#include <stdint.h>

#include <simbricks/base/proto.h>

typedef __uint128_t uint128_t;

/******************************************************************************/
/* Initialization messages on Unix socket */

/** welcome message sent by memory to host. */
struct SimbricksProtoPicSIntro {
  uint32_t dummy; /* not used, but need to avoid empty struct for standard C */
} __attribute__((packed));

/** welcome message sent by host to memory */
struct SimbricksProtoPicRIntro {
  uint32_t dummy; /* not used, but need to avoid empty struct for standard C */
} __attribute__((packed));

/******************************************************************************/
/* Messages on memory to host channel */

/** Mask for type value in own_type field */
#define SIMBRICKS_PROTO_PIC_R2S_MSG_READCOMP 0x40
#define SIMBRICKS_PROTO_PIC_R2S_MSG_WRITECOMP 0x41

struct SimbricksProtoPicR2SReadcomp {
  uint64_t req_id;
  uint8_t pad[40];
  uint64_t timestamp;
  uint8_t pad_[7];
  uint8_t own_type;
  uint8_t data[];
} __attribute__((packed));
SIMBRICKS_PROTO_MSG_SZCHECK(struct SimbricksProtoPicR2SReadcomp);

struct SimbricksProtoPicR2SWritecomp {
  uint64_t req_id;
  uint8_t pad[40];
  uint64_t timestamp;
  uint8_t pad_[7];
  uint8_t own_type;
} __attribute__((packed));
SIMBRICKS_PROTO_MSG_SZCHECK(struct SimbricksProtoPicR2SWritecomp);

union SimbricksProtoPicR2S {
  union SimbricksProtoBaseMsg base;
  struct SimbricksProtoPicR2SReadcomp readcomp;
  struct SimbricksProtoPicR2SWritecomp writecomp;
} __attribute__((packed));
SIMBRICKS_PROTO_MSG_SZCHECK(union SimbricksProtoPicR2S);

/******************************************************************************/
/* Messages on host to memory channel */

#define SIMBRICKS_PROTO_PIC_S2R_MSG_READ 0x60
#define SIMBRICKS_PROTO_PIC_S2R_MSG_WRITE 0x61
// treat write as posted so don't send a completion message
#define SIMBRICKS_PROTO_PIC_S2R_MSG_WRITE_POSTED 0x62

struct SimbricksProtoPicS2RRead {
  uint64_t req_id;
  uint64_t as_id;
  uint128_t addr;
  uint16_t len;
  uint8_t pad[14];
  uint64_t timestamp;
  uint8_t pad_[7];
  uint8_t own_type;
} __attribute__((packed));
SIMBRICKS_PROTO_MSG_SZCHECK(struct SimbricksProtoPicS2RRead);

struct SimbricksProtoPicS2RWrite {
  uint64_t req_id;
  uint64_t as_id;
  uint128_t addr;
  uint16_t len;
  uint8_t pad[14];
  uint64_t timestamp;
  uint8_t pad_[7];
  uint8_t own_type;
  uint8_t data[];
} __attribute__((packed));
SIMBRICKS_PROTO_MSG_SZCHECK(struct SimbricksProtoPicS2RWrite);

union SimbricksProtoPicS2R {
  union SimbricksProtoBaseMsg base;
  struct SimbricksProtoPicS2RRead read;
  struct SimbricksProtoPicS2RWrite write;
} __attribute__((packed));
SIMBRICKS_PROTO_MSG_SZCHECK(union SimbricksProtoPicS2R);

#endif  // SIMBRICKS_MEM_PROTO_H_
