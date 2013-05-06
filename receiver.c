#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#include <time.h>
#include <sys/time.h>
#include <math.h>

#include "packet.h"
#include "receiver.h"

const int NUM_PACKETS = 500;

int main(int argc, char *argv[]){

  if(argc != 3){
    printf("usage: port file\n");
    exit(0);
  }

  FILE* fd = fopen(argv[2], "w");

  srand(time(0)); // init random

  int status;
  int sockfd;

  struct addrinfo hints;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_flags = AI_PASSIVE;

  struct addrinfo *res, *p;

  if ((status = getaddrinfo(NULL, argv[1], &hints, &res)) != 0) {
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    exit(1);
  }

  for(p = res; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
        perror("listener: socket");
        continue;
    }

    if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockfd);
      perror("listener: bind");
      continue;
    }

    break;
  }

  if (p == NULL) {
    fprintf(stderr, "listener: failed to bind socket\n");
    return 2;
  }

  struct timeval timeout;

  timeout.tv_sec = 4;
  timeout.tv_usec = 0;
  setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,(struct timeval *)&timeout, sizeof(struct timeval));

  struct sockaddr src_addr;
  int src_len = sizeof src_addr;
  ee122_packet pkt;

  int num_rcv = 0;
  int bytes_read = 0;
  unsigned long num_expected = -1;
  float avg_len;

  unsigned char buff[128];
  long R;

  struct timeval last_time, curr_time, diff_time;
  double sum = 0.0;
  while(bytes_read = recvfrom(sockfd, buff, sizeof(ee122_packet), 0, &src_addr, &src_len)){
    pkt = deserialize_packet(buff);
    if(bytes_read == -1){
      if(num_rcv > 0){
        break;
      }
    } else {

      if(num_rcv == 0) {
        R = pkt.R;
      } else {
        if(pkt.R != R){
          perror("R CHANGED ON US!\n");
        }
      }

      num_rcv++;
      num_expected = pkt.num_expected;
      avg_len = pkt.avg_len;
      gettimeofday(&curr_time, NULL);
      last_time = pkt.timestamp;
      timeval_subtract(&diff_time, &curr_time, &last_time);
      sum += ((double)(diff_time.tv_sec)) + (diff_time.tv_usec / 1000000.0);

      fwrite(&pkt.garbage, sizeof(char), bytes_read, fd);
      //printf("%f\n", avg_len);
    }
  }

  printf("%d,%d,%lu,%lf,%lf,%f\n", pkt.R, num_rcv, num_expected, 100*(((float)num_rcv)/num_expected), avg_len, sum/num_rcv);

  freeaddrinfo(res);
  close(sockfd);
  exit(0);
}

int timeval_subtract (struct timeval *result, struct timeval *x, struct timeval *y) { 
	/* Perform the carry for the later subtraction by updating y. */
  if (x->tv_usec < y->tv_usec) {
    int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
    y->tv_usec -= 1000000 * nsec;
    y->tv_sec += nsec;
  }
  if (x->tv_usec - y->tv_usec > 1000000) {
    int nsec = (x->tv_usec - y->tv_usec) / 1000000;
    y->tv_usec += 1000000 * nsec;
    y->tv_sec -= nsec;
																														       }
														      
  /* Compute the time remaining to wait.
     tv_usec is certainly positive. */
  result->tv_sec = x->tv_sec - y->tv_sec;
  result->tv_usec = x->tv_usec - y->tv_usec;

	return x->tv_sec < y->tv_sec;
}
