#ifndef __AW_PACKET_H__
#define __AW_PACKET_H__

#include <time.h>
#include <stdint.h>

typedef struct {
    uint32_t seq_number; /* 4 bytes */
    struct timeval timestamp; /* 8 bytes */
    uint32_t R; /* 4 bytes */
    uint32_t num_expected; /* 4 bytes */
    float avg_len; /* 4 bytes */
    char stream; /* 1 bytes */
    char garbage[128-(4+sizeof(struct timeval)+4+1+4+4)];
} ee122_packet;

unsigned char * serialize_packet(unsigned char * buffer, ee122_packet p) {
    ((uint32_t*)buffer)[0] = htonl(p.seq_number);
    ((uint32_t*)buffer)[1] = htonl(p.timestamp.tv_sec);
    ((uint32_t*)buffer)[2] = htonl(p.timestamp.tv_usec);
    ((uint32_t*)buffer)[3] = htonl(p.R);
    ((float*)buffer)[4] = p.avg_len;
    buffer[5*4] = p.stream;
}

ee122_packet deserialize_packet(unsigned char* buffer){
  ee122_packet p;
  p.seq_number = ntohl(((uint32_t*)buffer)[0]);
  p.timestamp.tv_sec = ntohl(((uint32_t*)buffer)[1]);
  p.timestamp.tv_usec = ntohl(((uint32_t*)buffer)[2]);
  p.R = ntohl(((uint32_t*)buffer)[3]);
  p.avg_len = ntohl(((float*)buffer)[4]);
  p.stream = buffer[5*4];
  return p;
}

#endif
