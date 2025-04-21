#include "approximate_counter.h"

static int increment_global(struct approximate_counter *ac, int value) {
  if (pthread_mutex_lock(&ac->global_lock)) {
    return -1;
  }
  ac->global_count += value;
  if (pthread_mutex_unlock(&ac->global_lock)) {
    return -1;
  }

  return 0;
}

int init_approximate_counter(struct approximate_counter *ac, size_t thread_cnt, int threshold) {
  ac->thread_cnt = thread_cnt;
  ac->threshold = threshold;
  ac->global_count = 0;
  if (pthread_mutex_init(&ac->global_lock, NULL)) {
    return -1;
  }

  if ((ac->local_counts = (int *)calloc(thread_cnt, sizeof(int))) == NULL) {
    return -1;
  }
  if ((ac->local_locks = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t) * thread_cnt)) == NULL) {
    return -1;
  }
  for (size_t i = 0; i < thread_cnt; i++) {
    if (pthread_mutex_init(&ac->local_locks[i], NULL)) {
      return -1;
    }
  }

  return 0;
}

int increment(struct approximate_counter *ac, size_t thread_idx) {
  if (pthread_mutex_lock(&ac->local_locks[thread_idx])) {
    return -1;
  }
  ac->local_counts[thread_idx]++;
  
  if (ac->local_counts[thread_idx] >= ac->threshold) {
    if (increment_global(ac, ac->local_counts[thread_idx])) {
      return -1;
    }
    ac->local_counts[thread_idx] = 0;
  }

  if (pthread_mutex_unlock(&ac->local_locks[thread_idx])) {
    return -1;
  }

  return 0;
}

int get(struct approximate_counter *ac) {
  int value;

  if (pthread_mutex_lock(&ac->global_lock)) {
    return -1;
  }
  value = ac->global_count;
  if (pthread_mutex_unlock(&ac->global_lock)) {
    return -1;
  }

  return value;
}

