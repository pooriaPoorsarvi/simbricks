/*
 * Copyright 2022 Max Planck Institute for Software Systems, and
 * National University of Singapore
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 * Modified by Sidharth Sundar and Pooria Poorsarvi Tehrani
 */

#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "mem_pic.h"

#define MEM_PIC_DEBUG 1

typedef __uint128_t uint128_t;

static int exiting = 0;
static uint64_t cur_ts = 0;

static void sigint_handler(int dummy) {
  exiting = 1;
}

static void sigusr1_handler(int dummy) {
  fprintf(stderr,  "main_time = %lu\n", cur_ts);
}


bool PicIfInitListen(struct SimbricksPicIf *picif, const char *shm_path, struct SimbricksBaseIfParams *picParams){

  struct SimbricksBaseIf *picbase = &picif->base;
  struct SimbricksBaseIfSHMPool pool_;
  memset(&pool_, 0, sizeof(pool_));

  struct SimBricksBaseIfEstablishData ests[1];
  struct SimbricksProtoPicSIntro s_intro;
  struct SimbricksProtoPicRIntro r_intro;
  unsigned n_bifs = 0;

  memset(&r_intro, 0, sizeof(r_intro));
  ests[n_bifs].base_if = picbase;
  ests[n_bifs].tx_intro = &r_intro;
  ests[n_bifs].tx_intro_len = sizeof(r_intro);
  ests[n_bifs].rx_intro = &s_intro;
  ests[n_bifs].rx_intro_len = sizeof(s_intro);
  n_bifs++;

  
  if (SimbricksBaseIfInit(picbase, picParams)) {
    fprintf(stderr, "Init: SimbricksBaseIfInit failed");
  }

  if (SimbricksBaseIfSHMPoolCreate(
          &pool_, shm_path, SimbricksBaseIfSHMSize(&picbase->params)) != 0) {
    fprintf(stderr, "MemifInit: SimbricksBaseIfSHMPoolCreate failed");
    return false;
  }

  if (SimbricksBaseIfListen(picbase, &pool_) != 0) {
    fprintf(stderr, "MemifInit: SimbricksBaseIfListen failed");
    return false;
  }

  if (SimBricksBaseIfEstablish(ests, n_bifs)) {
    fprintf(stderr, "SimBricksBaseIfEstablish failed\n");
    return false;
  }

  printf("done connecting to CPU PIC\n");
  return true;
}

bool PicIfInit(struct SimbricksPicIf *picif, const char *shm_path, struct SimbricksBaseIfParams *picParams) {
  return PicIfInitListen(picif, shm_path, picParams);
}

bool MemIfInitConnect(struct SimbricksMemIf *memif, struct SimbricksBaseIfParams *memParams) {

    struct SimbricksBaseIf *membase = &memif->base;

    struct SimBricksBaseIfEstablishData ests[1];
    struct SimbricksProtoMemMemIntro m_intro;
    struct SimbricksProtoMemHostIntro h_intro;
    unsigned n_bifs = 0;

    memset(&m_intro, 0, sizeof(m_intro));
    ests[n_bifs].base_if = membase;
    ests[n_bifs].tx_intro = &h_intro;
    ests[n_bifs].tx_intro_len = sizeof(h_intro);
    ests[n_bifs].rx_intro = &m_intro;
    ests[n_bifs].rx_intro_len = sizeof(m_intro);
    n_bifs++;   

    if (SimbricksBaseIfInit(membase, memParams)) {
        fprintf(stderr, "SimbricksBaseIfInit failed\n");
        return false;
    }

    if (SimbricksBaseIfConnect(membase)) {
        fprintf(stderr, "SimbricksBaseIfConnect failed\n");
        return false;
    }

    if (SimbricksBaseIfConnected(membase)) {
        fprintf(stderr, "SimbricksBaseIfConnected indicates unconnected\n");
        return false;
    }

    if (SimBricksBaseIfEstablish(ests, n_bifs)) {
        fprintf(stderr, "SimBricksBaseIfEstablish failed\n");
        return false;
    }
    
    printf("done connecting to memory sim\n");
    return true;
}

bool MemIfInit(struct SimbricksMemIf *memif, const char *shm_path, struct SimbricksBaseIfParams *memParams) {
  return MemIfInitConnect(memif, memParams);
}

volatile union SimbricksProtoMemH2M *H2MAlloc(struct SimbricksMemIf *memif) {
  volatile union SimbricksProtoMemH2M *msg_to;
  bool first = true;
  while ((msg_to = SimbricksMemIfH2MOutAlloc(memif, cur_ts)) == NULL) {
    if (first) {
      fprintf(stderr,  "H2MAlloc: warning waiting for entry (%zu)\n",
              memif->base.out_pos);
      first = false;
    }
  }

  if (!first) {
    fprintf(stderr,  "H2MAlloc: entry successfully allocated\n");
  }

  return msg_to;
}

volatile union SimbricksProtoPicR2S *PICR2SAlloc(struct SimbricksPicIf *picif) {
  volatile union SimbricksProtoPicR2S *msg_to;
  bool first = true;
  while ((msg_to = SimbricksPicIfR2SOutAlloc(picif, cur_ts)) == NULL) {
    if (first) {
      fprintf(stderr,  "PICR2SAlloc: warning waiting for entry (%zu)\n",
              picif->base.out_pos);
      first = false;
    }
  }

  if (!first) {
    fprintf(stderr,  "PICR2SAlloc: entry successfully allocated\n");
  }

  return msg_to;
}

void HandleSimbricksPicS2RReadRequestMsg(struct SimbricksMemIf *memif, struct SimbricksPicIf *picif, volatile union SimbricksProtoPicS2R *cpu_pic_to_mem_pic_msg) {
  uint64_t len;
  uint128_t global_addr;
  volatile union SimbricksProtoMemH2M *mem_pic_to_mem_msg;
  volatile union SimbricksProtoMemM2H *mem_to_mem_pic_resp;
  volatile union SimbricksProtoPicR2S *mem_pic_to_cpu_pic_resp;
  
  global_addr = cpu_pic_to_mem_pic_msg->read.addr;
  len = cpu_pic_to_mem_pic_msg->read.len;

  mem_pic_to_mem_msg = H2MAlloc(memif);
  volatile struct SimbricksProtoPicS2RRead *read_in = &cpu_pic_to_mem_pic_msg->read;
  volatile struct SimbricksProtoMemH2MRead *read_out = &mem_pic_to_mem_msg->read;

  read_out->req_id = read_in->req_id;
  read_out->addr = translate_global_to_local(global_addr);
  read_out->len = read_in->len;
  read_out->timestamp = cur_ts;

  SimbricksMemIfH2MOutSend(memif, mem_pic_to_mem_msg, SIMBRICKS_PROTO_MEM_H2M_MSG_READ);

  mem_to_mem_pic_resp = PollM2H(memif);
  volatile struct SimbricksProtoMemM2HReadcomp *mem_read_resp = &mem_to_mem_pic_resp->readcomp;
  
  mem_pic_to_cpu_pic_resp = PICR2SAlloc(picif);
  volatile struct SimbricksProtoPicR2SReadcomp *mem_pic_to_cpu_pic_read_resp = &mem_pic_to_cpu_pic_resp->readcomp;

  mem_pic_to_cpu_pic_read_resp->req_id = mem_read_resp->req_id;
  mem_pic_to_cpu_pic_read_resp->timestamp = cur_ts;
  mem_pic_to_cpu_pic_read_resp->own_type = SIMBRICKS_PROTO_PIC_R2S_MSG_READCOMP;
  memcpy((void*)mem_pic_to_cpu_pic_read_resp->data, (void*)mem_read_resp->data, len);

  SimbricksPicIfR2SOutSend(picif, mem_pic_to_cpu_pic_resp, SIMBRICKS_PROTO_PIC_R2S_MSG_READCOMP);


#if MEM_PIC_DEBUG
      uint128_t mask = 1;
      mask = (mask << 64) - 1;
      uint64_t lower = global_addr & mask;
      uint64_t upper = (global_addr - lower) >> 64;
      printf("received PIC read. global addr: 0x%lx%lx size: 0x%lx\n", upper, lower, len);
      for (int i = 0; i < (int)len; i++) {
        printf("%X ", mem_read_resp->data[i]);
      }
      printf("\n");
#endif

  SimbricksMemIfM2HInDone(memif, mem_to_mem_pic_resp);
}

void HandleSimbricksPicS2RWriteRequestMsg(struct SimbricksMemIf *memif, struct SimbricksPicIf *picif, volatile union SimbricksProtoPicS2R *cpu_pic_to_mem_pic_msg){
  uint64_t len;
  uint128_t global_addr;
  volatile union SimbricksProtoMemH2M *mem_pic_to_mem_msg;
  volatile union SimbricksProtoMemM2H *mem_to_mem_pic_resp;
  volatile union SimbricksProtoPicR2S *mem_pic_to_cpu_pic_resp;
  
  global_addr = cpu_pic_to_mem_pic_msg->write.addr;
  len = cpu_pic_to_mem_pic_msg->write.len;

  mem_pic_to_mem_msg = H2MAlloc(memif);
  volatile struct SimbricksProtoPicS2RWrite *write_in = &cpu_pic_to_mem_pic_msg->write;
  volatile struct SimbricksProtoMemH2MWrite *write_out = &mem_pic_to_mem_msg->write;

  write_out->req_id = write_in->req_id;
  write_out->addr = translate_global_to_local(global_addr);
  write_out->len = write_in->len;
  write_out->timestamp = cur_ts;

  memcpy((void*)write_out->data, (void*)write_in->data, len);

  SimbricksMemIfH2MOutSend(memif, mem_pic_to_mem_msg, SIMBRICKS_PROTO_MEM_H2M_MSG_WRITE);

  mem_to_mem_pic_resp = PollM2H(memif);
  volatile struct SimbricksProtoMemM2HWritecomp *mem_write_resp = &mem_to_mem_pic_resp->writecomp;
  
  mem_pic_to_cpu_pic_resp = PICR2SAlloc(picif);
  volatile struct SimbricksProtoPicR2SWritecomp *mem_pic_to_cpu_pic_write_resp = &mem_pic_to_cpu_pic_resp->writecomp;

  mem_pic_to_cpu_pic_write_resp->req_id = mem_write_resp->req_id;
  mem_pic_to_cpu_pic_write_resp->timestamp = cur_ts;
  mem_pic_to_cpu_pic_write_resp->own_type = SIMBRICKS_PROTO_PIC_R2S_MSG_WRITECOMP;

  SimbricksPicIfR2SOutSend(picif, mem_pic_to_cpu_pic_resp, SIMBRICKS_PROTO_PIC_R2S_MSG_WRITECOMP);


#if MEM_PIC_DEBUG
      uint128_t mask = 1;
      mask = (mask << 64) - 1;
      uint64_t lower = global_addr & mask;
      uint64_t upper = (global_addr - lower) >> 64;

      printf("received PIC write. addr: 0x%lx%lx size: 0x%lx\n", upper, lower, len);
      for (int i = 0; i < (int)len; i++) {
        printf("%X ", write_in->data[i]);
      }
      printf("\n");
#endif

  SimbricksMemIfM2HInDone(memif, mem_to_mem_pic_resp);
}

void HandleSimbricksPicS2RMessage(struct SimbricksMemIf *memif, struct SimbricksPicIf *picif, volatile union SimbricksProtoPicS2R *cpu_pic_to_mem_pic_msg) {


  uint8_t type;

  type = SimbricksPicIfS2RInType(picif, cpu_pic_to_mem_pic_msg);
  switch (type) {
    case SIMBRICKS_PROTO_PIC_S2R_MSG_READ:
      HandleSimbricksPicS2RReadRequestMsg(memif, picif, cpu_pic_to_mem_pic_msg);
      break;

    case SIMBRICKS_PROTO_PIC_S2R_MSG_WRITE:
      HandleSimbricksPicS2RWriteRequestMsg(memif, picif, cpu_pic_to_mem_pic_msg);
      break;

    case SIMBRICKS_PROTO_PIC_S2R_MSG_WRITE_POSTED:
      // TODO: handle posted writes
      break;

    case SIMBRICKS_PROTO_MSG_TYPE_SYNC:
      // TODO: update timestamp based on timestamp of sync messages
      break;

    case SIMBRICKS_PROTO_MSG_TYPE_TERMINATE:
      SimbricksBaseIfClose(&memif->base);
      exiting=1;
      break;
    default:
      fprintf(stderr,  "Unsupported message type=%u\n", type);
  }

  SimbricksPicIfS2RInDone(picif, cpu_pic_to_mem_pic_msg);
}

void syncOnPicS2RSyncMessage(struct SimbricksPicIf *picif, volatile union SimbricksProtoPicS2R *msg){
    cur_ts = msg->base.header.timestamp;
    SimbricksPicIfS2RInDone(picif, msg);
}

void syncOnMemM2HSyncMessage(struct SimbricksMemIf *memif, volatile union SimbricksProtoMemM2H *msg){
    cur_ts = msg->base.header.timestamp;
    SimbricksMemIfM2HInDone(memif, msg);
}

volatile union SimbricksProtoMemM2H *PollM2H(struct SimbricksMemIf *memif) {
  volatile union SimbricksProtoMemM2H *msg;

  //TODO: the INT64_MAX is a hack to avoid timestamp management. fix this
  while (!(msg = SimbricksMemIfM2HInPoll(memif, __INT64_MAX__)) && !exiting) {
    if (!msg) {
      continue;
    }
    else if (SimbricksMemIfM2HInType(memif, msg) == SIMBRICKS_PROTO_MSG_TYPE_SYNC) { //TODO: handle timestamping with sync messages
      syncOnMemM2HSyncMessage(memif, msg);
      continue;
    }
    break;
  }
  return msg;
}

volatile union SimbricksProtoPicS2R *PollPicS2R(struct SimbricksPicIf *picif) {
  volatile union SimbricksProtoPicS2R *msg;

  while (!(msg = SimbricksPicIfS2RInPoll(picif, __INT64_MAX__)) && !exiting) {
    if (!msg){
      continue;
    }
    else if (SimbricksPicIfS2RInType(picif, msg) == SIMBRICKS_PROTO_MSG_TYPE_SYNC) {
        syncOnPicS2RSyncMessage(picif, msg);
        continue;
    }
    break;
  }
  return msg;
}

int main(int argc, char *argv[]) {
  signal(SIGINT, sigint_handler);
  signal(SIGUSR1, sigusr1_handler);

  if (argc != 9) {
    fprintf(stderr, 
            "Usage: mem_pic [ASID] [MEM-SOCKET] [MEM-LINK-LATENCY] " 
            "[PIC-SOCKET] [PIC-SHM] [PIC-LINK-LATENCY] [SYNC-MODE] [START-TICK] \n");
    return -1;
  } 

  int sync_mem = 1;
  uint64_t next_ts = 0;
  const char *shmPath;
  struct SimbricksBaseIfParams memParams;
  struct SimbricksMemIf memif;

  SimbricksMemIfDefaultParams(&memParams);

  memParams.sock_path = argv[2];
  memParams.link_latency = strtoull(argv[3], NULL, 0);
  memParams.sync_interval = memParams.link_latency;
  memParams.sync_mode = kSimbricksBaseIfSyncOptional; // TODO: handle sync mode properly
  memParams.blocking_conn = true;

  if (!MemIfInit(&memif, NULL, &memParams)) {
    return EXIT_FAILURE;
  }

  struct SimbricksBaseIfParams picParams;
  struct SimbricksPicIf picif;

  SimbricksPicIfDefaultParams(&picParams);

  picParams.sock_path = argv[4];
  shmPath = argv[5];
  picParams.link_latency = strtoull(argv[6], NULL, 0);
  picParams.sync_interval = picParams.link_latency;
  picParams.sync_mode = kSimbricksBaseIfSyncOptional;
  picParams.blocking_conn = true;

  if (!PicIfInit(&picif, shmPath, &picParams)) {
    return EXIT_FAILURE;
  }

  volatile union SimbricksProtoPicS2R *pic_msg;

  printf("start polling\n");
  while (!exiting) {
    do {
      pic_msg = PollPicS2R(&picif);
      HandleSimbricksPicS2RMessage(&memif, &picif, pic_msg);

      if (sync_mem) {
        next_ts = SimbricksPicIfS2RInTimestamp(&picif);
      }
    } while (!exiting && next_ts <= cur_ts);

    cur_ts = next_ts;
  }
  return 0;
}
