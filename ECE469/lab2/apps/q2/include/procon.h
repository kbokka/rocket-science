#ifndef __USERPROG__
#define __USERPROG__

#define BUFFER_SIZE 7

typedef struct circular_buffer {
  int head;
  int tail;
  char buffer[BUFFER_SIZE];
} circular_buffer;

#define CONSUMER_TO_RUN "consumer.dlx.obj"
#define PRODUCER_TO_RUN "producer.dlx.obj"

#define THE_MSG "Hello World"
#define THE_MSG_LENGTH 11

#endif
