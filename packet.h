#ifndef __AW_PACKET_H__
#define __AW_PACKET_H__

#include <time.h>
#include <stdint.h>

const int PAY_LEN = 512;

typedef struct {
    uint32_t seq_number; /* 4 bytes */
    struct timeval timestamp; /* 8 bytes */
    uint32_t R; /* 4 bytes */
    uint32_t num_expected; /* 4 bytes */
    uint32_t window_size; /* 4 bytes */
    float timeout; /* 4 bytes */
    float avg_len; /* 4 bytes */
    char stream; /* 1 bytes */
    char payload[512];
} ee122_packet;

unsigned char * serialize_packet(unsigned char * buffer, ee122_packet p) {
    ((uint32_t*)buffer)[0] = htonl(p.seq_number);
    ((uint32_t*)buffer)[1] = htonl(p.timestamp.tv_sec);
    ((uint32_t*)buffer)[2] = htonl(p.timestamp.tv_usec);
    ((uint32_t*)buffer)[3] = htonl(p.R);
    ((uint32_t*)buffer)[4] = htonl(p.num_expected);
    ((uint32_t*)buffer)[5] = htonl(p.window_size);
    buffer[6*4] = ((char*)&p.timeout)[3];
    buffer[6*4+1] = ((char*)&p.timeout)[2];
    buffer[6*4+2] = ((char*)&p.timeout)[1];
    buffer[6*4+3] = ((char*)&p.timeout)[0];
    buffer[7*4] = ((char*)&p.avg_len)[3];
    buffer[7*4+1] = ((char*)&p.avg_len)[2];
    buffer[7*4+2] = ((char*)&p.avg_len)[1];
    buffer[7*4+3] = ((char*)&p.avg_len)[0];
    buffer[8*4] = p.stream;
    memcpy(&buffer[8*4+1], p.payload, sizeof(p.payload));
}

ee122_packet deserialize_packet(unsigned char* buffer){
  ee122_packet p;
  p.seq_number = ntohl(((uint32_t*)buffer)[0]);
  p.timestamp.tv_sec = ntohl(((uint32_t*)buffer)[1]);
  p.timestamp.tv_usec = ntohl(((uint32_t*)buffer)[2]);
  p.R = ntohl(((uint32_t*)buffer)[3]);
  p.num_expected = ntohl(((uint32_t*) buffer)[4]);
  p.window_size = ntohl(((uint32_t*) buffer)[5]);
  ((char*) &p.timeout)[0] = buffer[6*4+3];
  ((char*) &p.timeout)[1] = buffer[6*4+2];
  ((char*) &p.timeout)[2] = buffer[6*4+1];
  ((char*) &p.timeout)[3] = buffer[6*4];
  ((char*) &p.avg_len)[0] = buffer[7*4+3];
  ((char*) &p.avg_len)[1] = buffer[7*4+2];
  ((char*) &p.avg_len)[2] = buffer[7*4+1];
  ((char*) &p.avg_len)[3] = buffer[7*4];
  p.stream = buffer[8*4];
  memcpy(p.payload, &buffer[8*4+1], sizeof(p.payload));
  return p;
}

#endif
