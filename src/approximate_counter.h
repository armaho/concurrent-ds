#ifndef APPROXIMATE_COUNTER_H
#define  APPROXIMATE_COUNTER_H

#include <stdlib.h>
#include <pthread.h>

struct approximate_counter {
  int threshold;
  size_t thread_cnt;

  int global_count;
  pthread_mutex_t global_lock;

  int *local_counts;
  pthread_mutex_t *local_locks;
};

int init_approximate_counter(struct approximate_counter *ac, size_t thread_cnt, int threshold);

int increment(struct approximate_counter *ac, size_t thread_idx);

int get(struct approximate_counter *ac);

#endif // !APPROXIMATE_COUNTER_H
