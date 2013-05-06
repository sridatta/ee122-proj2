#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>

#include <time.h>
#include <sys/time.h>
#include <math.h>

#include "packet.h"
#include "sender.h"

const int NUM_PACKETS = 500;

int main(int argc, char *argv[]){

  if(argc != 6){
    printf("usage: host port R stream_id file\n");
    exit(0);
  }

  char *endptr;
  long R = strtol(argv[3], &endptr, 10);
  char *stream_id = argv[4];

  if(endptr != (argv[3] + strlen(argv[3]))) {
    fprintf(stderr, "R must be an integer (milliseconds).\n");
    exit(1);
  }

  FILE *fd = fopen(argv[5], "r");
  if(NULL == fd) {
    fprintf(stderr, "fopen() error\n");
    return 1;
  }


  int status;
  int sockfd;

  struct addrinfo hints;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_DGRAM;

  struct addrinfo *res, *p;

  if ((status = getaddrinfo(argv[1], argv[2], &hints, &res)) != 0) {
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    exit(1);
  }

  for(p = res; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
        fprintf(stderr, "socket() error");
        continue;
    }

    break;
  }

  if(p == NULL){
    fprintf(stderr, "No valid addresses");
    exit(2);
  }


  ee122_packet pkt;
  pkt.R = R;
  pkt.stream = *stream_id;
  pkt.num_expected = NUM_PACKETS;
  pkt.avg_len = 0;

  struct timespec sleep_spec;
  sleep_spec.tv_sec = 0;

  struct timeval curr_time;

  int seq_no;
  int read_count;
  int write_count;

  while(read_count = fread(&pkt.payload, sizeof(char), 512, fd)) {
    printf("sending. read bytes %d\n", read_count);
    gettimeofday(&curr_time, NULL);

    sleep_spec.tv_nsec = rand_poisson(R)*1000000;
    pkt.seq_number = seq_no;
    pkt.timestamp = curr_time;

    sendto(sockfd, &pkt, sizeof(ee122_packet), 0, p->ai_addr, p->ai_addrlen);
    nanosleep(&sleep_spec, &sleep_spec);
  }

  freeaddrinfo(res);
  fclose(fd);
  close(sockfd);
  exit(0);
}

long rand_poisson(long lambda){
  long k=0;
  double L=exp(-lambda), p=1;
  do {
    ++k;
    p *= rand()/(double)RAND_MAX;
  } while (p > L);
  return --k;
}
