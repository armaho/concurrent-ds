#include "approximate_counter.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>

struct thread_input {
  int idx;
  int inc;
  struct approximate_counter *ac;
};

void *increment_thread(void *args) {
  struct thread_input *thread_i = (struct thread_input *)args;

  for (int i = 0; i < thread_i->inc; i++) {
    increment(thread_i->ac, thread_i->idx);
  }

  return NULL;
}

int main(int argc, char **argv) {
  if (argc != 4) {
    fprintf(stderr, "Usage: act [thread count] [increment in each thread] [threshold]\n");
    exit(EX_USAGE);
  }

  int thread_cnt = atoi(argv[1]);
  int thread_inc = atoi(argv[2]);
  int threshold = atoi(argv[3]);

  struct approximate_counter ac;
  if (init_approximate_counter(&ac, thread_cnt, threshold)) {
    exit(EX_SOFTWARE);
  }

  pthread_t thread_info[thread_cnt];
  struct thread_input t_input[thread_cnt];

  for (int i = 0; i < thread_cnt; i++) {
    t_input[i].idx = i;
    t_input[i].inc = thread_inc;
    t_input[i].ac = &ac;

    if (pthread_create(&thread_info[i], NULL, increment_thread, (void *)&t_input[i])) {
      exit(EX_SOFTWARE);
    }
  }

  for (int i = 0; i < thread_cnt; i++) {
    if (pthread_join(thread_info[i], NULL)) {
      exit(EX_SOFTWARE);
    }
  }

  printf("total count: %d\n", get(&ac));

  return 0;
}
