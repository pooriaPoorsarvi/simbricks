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

#include "cpu_pic.h"

#define CPU_PIC_DEBUG 1

static int exiting = 0;
static uint64_t cur_ts = 0;

static void sigint_handler(int dummy) {
  exiting = 1;
}

static void sigusr1_handler(int dummy) {
  fprintf(stderr, "main_time = %lu\n", cur_ts);
}


bool PicIfInitConnect(struct SimbricksPicIf *picif, struct SimbricksBaseIfParams *picParams){

  struct SimbricksBaseIf *picbase = &picif->base;

  struct SimBricksBaseIfEstablishData ests[1];
  struct SimbricksProtoPicSIntro s_intro;
  struct SimbricksProtoPicRIntro r_intro;
  unsigned n_bifs = 0;

  memset(&s_intro, 0, sizeof(s_intro));
  ests[n_bifs].base_if = picbase;
  ests[n_bifs].tx_intro = &s_intro;
  ests[n_bifs].tx_intro_len = sizeof(s_intro);
  ests[n_bifs].rx_intro = &r_intro;
  ests[n_bifs].rx_intro_len = sizeof(r_intro);
  n_bifs++;

  if (SimbricksBaseIfInit(picbase, picParams)) {
    fprintf(stderr, "PIC Init: SimbricksBaseIfInit failed");
  }

  if (SimbricksBaseIfConnect(picbase)) {
    fprintf(stderr, "PIC Connect: SimbricksBaseIfConnect failed\n");
    return false;
  }

  if (SimbricksBaseIfConnected(picbase)) {
    fprintf(stderr, "PIC Connection Error: SimbricksBaseIfConnected indicates unconnected\n");
    return false;
  }

  if (SimBricksBaseIfEstablish(ests, 1)) {
    fprintf(stderr, "PIC Init Error: SimBricksBaseIfEstablish failed\n");
    return false;
  }

  printf("done connecting to PIC\n");
  return true;
}

bool PicIfInit(struct SimbricksPicIf *picif, const char *shm_path, struct SimbricksBaseIfParams *picParams) {
  return PicIfInitConnect(picif, picParams);
}

bool MemIfInitListen(struct SimbricksMemIf *memif, const char *shm_path, struct SimbricksBaseIfParams *memParams) {
  
  struct SimbricksBaseIf *membase = &memif->base;
  struct SimbricksBaseIfSHMPool pool_;
  memset(&pool_, 0, sizeof(pool_));

  struct SimBricksBaseIfEstablishData ests[1];
  struct SimbricksProtoMemMemIntro m_intro;
  struct SimbricksProtoMemHostIntro h_intro;
  unsigned n_bifs = 0;

  memset(&m_intro, 0, sizeof(m_intro));
  ests[n_bifs].base_if = membase;
  ests[n_bifs].tx_intro = &m_intro;
  ests[n_bifs].tx_intro_len = sizeof(m_intro);
  ests[n_bifs].rx_intro = &h_intro;
  ests[n_bifs].rx_intro_len = sizeof(h_intro);
  n_bifs++;

  if (SimbricksBaseIfInit(membase, memParams)) {
    fprintf(stderr, "Init: SimbricksBaseIfInit failed");
  }

  if (SimbricksBaseIfSHMPoolCreate(
          &pool_, shm_path, SimbricksBaseIfSHMSize(&membase->params)) != 0) {
    fprintf(stderr, "MemIfInit: SimbricksBaseIfSHMPoolCreate failed");
    return false;
  }

  if (SimbricksBaseIfListen(membase, &pool_) != 0) {
    fprintf(stderr, "MemIfInit: SimbricksBaseIfListen failed");
    return false;
  }

  if (SimBricksBaseIfEstablish(ests, n_bifs)) {
    fprintf(stderr, "MemIfInit: SimBricksBaseIfEstablish failed\n");
    return false;
  }

  printf("done connecting to cpu sim\n");
  return true;
}

bool MemIfInit(struct SimbricksMemIf *memif, const char *shm_path, struct SimbricksBaseIfParams *memParams) {
  return MemIfInitListen(memif, shm_path, memParams);
}

volatile union SimbricksProtoMemM2H *M2HAlloc(struct SimbricksMemIf *memif) {
  volatile union SimbricksProtoMemM2H *msg_to;
  bool first = true;
  while ((msg_to = SimbricksMemIfM2HOutAlloc(memif, cur_ts)) == NULL) {
    if (first) {
      fprintf(stderr, "M2HAlloc: warning waiting for entry (%zu)\n",
              memif->base.out_pos);
      first = false;
    }
  }

  if (!first) {
    fprintf(stderr, "D2HAlloc: entry successfully allocated\n");
  }

  return msg_to;
}


volatile union SimbricksProtoPicS2R *PICS2RAlloc(struct SimbricksPicIf *picif) {
  volatile union SimbricksProtoPicS2R *msg_to;
  bool first = true;
  while ((msg_to = SimbricksPicIfS2ROutAlloc(picif, cur_ts)) == NULL) {
    if (first) {
      fprintf(stderr,  "PICS2RAlloc: warning waiting for entry (%zu)\n",
              picif->base.out_pos);
      first = false;
    }
  }

  if (!first) {
    fprintf(stderr,  "PICS2RAlloc: entry successfully allocated\n");
  }

  return msg_to;
}

void HandleSimbricksH2MReadRequestMsg(struct SimbricksMemIf *memif, struct SimbricksPicIf *picif, volatile union SimbricksProtoMemH2M *msg) {
  uint64_t local_addr, len;
  volatile union SimbricksProtoMemM2H *msg_resp;
  volatile union SimbricksProtoPicS2R *cpu_pic_to_mem_pic_msg;
  volatile union SimbricksProtoPicR2S *mem_pic_to_cpu_pic_resp;
  
  local_addr = msg->read.addr;
  len = msg->read.len;

  cpu_pic_to_mem_pic_msg = PICS2RAlloc(picif);
  volatile struct SimbricksProtoMemH2MRead *read_in = &msg->read;
  volatile struct SimbricksProtoPicS2RRead *read_out = &cpu_pic_to_mem_pic_msg->read;

  read_out->req_id = read_in->req_id;
  read_out->addr = translate_local_to_global(local_addr);
  read_out->len = read_in->len;
  read_out->timestamp = cur_ts;

  SimbricksPicIfS2ROutSend(picif, cpu_pic_to_mem_pic_msg, SIMBRICKS_PROTO_PIC_S2R_MSG_READ);

  mem_pic_to_cpu_pic_resp = PollPicR2S(picif);
  volatile struct SimbricksProtoPicR2SReadcomp *cpu_pic_read_resp = &mem_pic_to_cpu_pic_resp->readcomp;
  
  msg_resp = M2HAlloc(memif);
  volatile struct SimbricksProtoMemM2HReadcomp *msg_read_resp = &msg_resp->readcomp;

  msg_read_resp->req_id = cpu_pic_read_resp->req_id;
  msg_read_resp->timestamp = cur_ts;
  msg_read_resp->own_type = SIMBRICKS_PROTO_MEM_M2H_MSG_READCOMP;
  memcpy((void*)msg_read_resp->data, (void*)cpu_pic_read_resp->data, len);

  SimbricksMemIfM2HOutSend(memif, msg_resp, SIMBRICKS_PROTO_MEM_M2H_MSG_READCOMP);
  

#if CPU_PIC_DEBUG
      printf("received H2M read. addr: 0x%lx size: 0x%lx\n", local_addr, len);
      for (int i = 0; i < (int)len; i++) {
        printf("%X ", cpu_pic_read_resp->data[i]);
      }
      printf("\n");
#endif

  SimbricksPicIfR2SInDone(picif, mem_pic_to_cpu_pic_resp);
}

void HandleSimbricksH2MWriteRequestMsg(struct SimbricksMemIf *memif, struct SimbricksPicIf *picif, volatile union SimbricksProtoMemH2M *msg){
  uint64_t local_addr, len;
  volatile union SimbricksProtoMemM2H *msg_resp;
  volatile union SimbricksProtoPicS2R *cpu_pic_to_mem_pic_msg;
  volatile union SimbricksProtoPicR2S *mem_pic_to_cpu_pic_resp; 
  local_addr = msg->write.addr;
  len = msg->write.len;

  cpu_pic_to_mem_pic_msg = PICS2RAlloc(picif);
  volatile struct SimbricksProtoMemH2MWrite *write_in = &msg->write;
  volatile struct SimbricksProtoPicS2RWrite *write_out = &cpu_pic_to_mem_pic_msg->write;

  write_out->req_id = write_in->req_id;
  write_out->addr = translate_local_to_global(local_addr);
  write_out->len = len;
  write_out->timestamp = cur_ts;
  memcpy((void*)write_out->data, (void*)write_in->data, len);

  SimbricksPicIfS2ROutSend(picif, cpu_pic_to_mem_pic_msg, SIMBRICKS_PROTO_PIC_S2R_MSG_WRITE);

  mem_pic_to_cpu_pic_resp = PollPicR2S(picif);
  volatile struct SimbricksProtoPicR2SWritecomp *cpu_pic_write_resp = &mem_pic_to_cpu_pic_resp->writecomp;
  
  msg_resp = M2HAlloc(memif);
  volatile struct SimbricksProtoMemM2HWritecomp *msg_write_resp = &msg_resp->writecomp;

  msg_write_resp->req_id = cpu_pic_write_resp->req_id;
  msg_write_resp->timestamp = cur_ts;
  msg_write_resp->own_type = SIMBRICKS_PROTO_MEM_M2H_MSG_WRITECOMP;

  SimbricksMemIfM2HOutSend(memif, msg_resp, SIMBRICKS_PROTO_MEM_M2H_MSG_WRITECOMP);


  #if CPU_PIC_DEBUG
    printf("received H2M write addr: 0x%lx size: %d\n", local_addr, msg->write.len);
    for (int i = 0; i < (int)len; i++) {
      printf("%X ", msg->write.data[i]);
    }
    printf("\n");
  #endif
  
  SimbricksPicIfR2SInDone(picif, mem_pic_to_cpu_pic_resp);
}

void HandleSimbricksH2MMsg(struct SimbricksMemIf *memif, struct SimbricksPicIf *picif, volatile union SimbricksProtoMemH2M *msg) {


  uint8_t type;

  type = SimbricksMemIfH2MInType(memif, msg);
  switch (type) {
    case SIMBRICKS_PROTO_MEM_H2M_MSG_READ:
      HandleSimbricksH2MReadRequestMsg(memif, picif, msg);
      break;

    case SIMBRICKS_PROTO_MEM_H2M_MSG_WRITE:
      HandleSimbricksH2MWriteRequestMsg(memif, picif, msg);
      break;

    case SIMBRICKS_PROTO_MEM_H2M_MSG_WRITE_POSTED:
      // TODO: handle posted writes
      break;

    case SIMBRICKS_PROTO_MSG_TYPE_SYNC:
      // TODO: update timestamp based on timestamp of sync messages
      break;

    case SIMBRICKS_PROTO_MSG_TYPE_TERMINATE:
      SimbricksBaseIfClose(&picif->base);
      exiting=1;
      break;
    default:
      fprintf(stderr, "Unsupported message type=%u\n", type);
  }

  SimbricksMemIfH2MInDone(memif, msg);
}

void syncOnPicR2SSyncMessage(struct SimbricksPicIf *picif, volatile union SimbricksProtoPicR2S *msg){
    cur_ts = msg->base.header.timestamp;
    SimbricksPicIfR2SInDone(picif, msg);
}

void syncOnMemH2MSyncMessage(struct SimbricksMemIf *memif, volatile union SimbricksProtoMemH2M *msg){
    cur_ts = msg->base.header.timestamp;
    SimbricksMemIfH2MInDone(memif, msg);
}

volatile union SimbricksProtoMemH2M *PollH2M(struct SimbricksMemIf *memif) {
  volatile union SimbricksProtoMemH2M *msg;

  //TODO: the INT64_MAX is a hack to avoid timestamp management. fix this
  while (!(msg = SimbricksMemIfH2MInPoll(memif, __INT64_MAX__)) && !exiting) { 
    if (!msg){
      continue;
    }
    else if (SimbricksMemIfH2MInType(memif, msg) == SIMBRICKS_PROTO_MSG_TYPE_SYNC) { //TODO: handle timestamping with sync messages
      syncOnMemH2MSyncMessage(memif, msg);
      continue;
    }
    break;
  }
  return msg;
}

volatile union SimbricksProtoPicR2S *PollPicR2S(struct SimbricksPicIf *picif) {
  volatile union SimbricksProtoPicR2S *msg;

  while (!(msg = SimbricksPicIfR2SInPoll(picif, __INT64_MAX__)) && !exiting) {
    if (!msg){
      continue;
    }
    else if (SimbricksPicIfR2SInType(picif, msg) == SIMBRICKS_PROTO_MSG_TYPE_SYNC) {
      syncOnPicR2SSyncMessage(picif, msg);
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
            "Usage: cpu_pic [ASID] [MEM-SOCKET] [MEM-SHM] [MEM-LINK-LATENCY] " 
            "[PIC-SOCKET] [PIC-LINK-LATENCY] [SYNC-MODE] [START-TICK] \n");
    return -1;
  } 

  int sync_mem = 1;
  uint64_t next_ts = 0;
  const char *shmPath;
  struct SimbricksBaseIfParams memParams;
  struct SimbricksMemIf memif;

  SimbricksMemIfDefaultParams(&memParams);

  memParams.sock_path = argv[2];
  shmPath = argv[3];
  memParams.link_latency = strtoull(argv[4], NULL, 0);
  memParams.sync_interval = memParams.link_latency;
  memParams.sync_mode = kSimbricksBaseIfSyncOptional;
  memParams.blocking_conn = true;

  if (!MemIfInit(&memif, shmPath, &memParams)) {
    return EXIT_FAILURE;
  }

  struct SimbricksBaseIfParams picParams;
  struct SimbricksPicIf picif;

  SimbricksPicIfDefaultParams(&picParams);

  picParams.sock_path = argv[5];
  picParams.link_latency = strtoull(argv[8], NULL, 0);
  picParams.sync_interval = picParams.link_latency;
  picParams.sync_mode = kSimbricksBaseIfSyncOptional;
  picParams.blocking_conn = true;

  if (!PicIfInit(&picif, NULL, &picParams)) {
    return EXIT_FAILURE;
  }

  volatile union SimbricksProtoMemH2M *cpu_msg;

  printf("start polling\n");
  while (!exiting) {
    do {
      cpu_msg = PollH2M(&memif);
      HandleSimbricksH2MMsg(&memif, &picif, cpu_msg);

      if (sync_mem) {
        next_ts = SimbricksMemIfH2MInTimestamp(&memif);
      }
    } while (!exiting && next_ts <= cur_ts);

    cur_ts = next_ts;
  }
  return 0;
}
