#ifndef __CONCURRENT_LINK_LIST__
#define __CONCURRENT_LINK_LIST__

#include <pthread.h>
#include <stdlib.h>
#include <assert.h>
#include "pthread_helper.h"

struct concurrent_link_list_node {
  int val;
  pthread_mutex_t lock;
  struct concurrent_link_list_node *next;
};

struct concurrent_link_list {
  struct concurrent_link_list_node *head; 
};

int init_concurrent_link_list(struct concurrent_link_list *ll);

int push_concurrent_link_list(struct concurrent_link_list *ll, int val);

int search_concurrent_link_list(struct concurrent_link_list *ll, int val);

int remove_concurrent_link_list(struct concurrent_link_list *ll, int val);

#endif // !__CONCURRENT_LINK_LIST__
