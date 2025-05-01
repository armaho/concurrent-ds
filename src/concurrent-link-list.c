#include "concurrent-link-list.h"

static struct concurrent_link_list_node *create_concurrent_link_list_node(int val, struct concurrent_link_list_node *next) {
  struct concurrent_link_list_node *node = malloc(sizeof(struct concurrent_link_list_node));
  
  if (node == NULL) {
    return NULL;
  }

  node->val = val;
  node->next = next;
  
  if (pthread_mutex_init(&node->lock, NULL) == -1) {
    free(node);
    return NULL;
  }

  return node;
}

int init_concurrent_link_list(struct concurrent_link_list *ll) {
  ll->head = create_concurrent_link_list_node(0, NULL);
  
  if (ll->head == NULL) {
    return -1;
  }
  return 0;
}

int push_concurrent_link_list(struct concurrent_link_list *ll, int val) {
  struct concurrent_link_list_node *node = create_concurrent_link_list_node(val, NULL);

  if (node == NULL) {
    return -1;
  }

  struct concurrent_link_list_node *last_node = ll->head;
  if (pthread_mutex_lock(&ll->head->lock)) {
    return -1;
  }

  while (last_node->next != NULL) {
    struct concurrent_link_list_node *next = last_node->next;

    if (pthread_mutex_lock(&next->lock)) {
      assert(pthread_mutex_unlock(&last_node->lock) == 0);
      return -1;
    }

    assert(pthread_mutex_unlock(&last_node->lock) == 0);

    last_node = last_node->next;
  }

  last_node->next = node;
  assert(pthread_mutex_unlock(&last_node->lock) == 0);
  return 0;
}

int search_concurrent_link_list(struct concurrent_link_list *ll, int val) {
  if (pthread_mutex_lock(&ll->head->lock)) {
    return -1;
  }

  struct concurrent_link_list_node *last_node = ll->head->next;
  
  if (last_node == NULL) {
    assert(pthread_mutex_unlock(&ll->head->lock) == 0);
    return 0;
  }

  if (pthread_mutex_lock(&last_node->lock)) {
    assert(pthread_mutex_unlock(&ll->head->lock) == 0);
    return -1;
  }

  assert(pthread_mutex_unlock(&ll->head->lock) == 0);

  while (last_node->val != val) {
    struct concurrent_link_list_node *next = last_node->next;
    
    if (next == NULL) {
      assert(pthread_mutex_unlock(&last_node->lock) == 0);
      return 0;
    }

    if (pthread_mutex_lock(&next->lock)) {
      assert(pthread_mutex_unlock(&last_node->lock) == 0);
      return -1;
    }

    assert(pthread_mutex_unlock(&last_node->lock) == 0);
    last_node = next;
  }

  return 1;
}

int remove_concurrent_link_list(struct concurrent_link_list *ll, int val) {
  struct concurrent_link_list_node *par = ll->head;
  if (pthread_mutex_lock(&par->lock)) {
    return -1;
  }

  struct concurrent_link_list_node *current = par->next;
  if (pthread_mutex_lock(&current->lock)) {
    assert(pthread_mutex_unlock(&par->lock) == 0);
    return -1;
  }

  while (current->val != val) {
    struct concurrent_link_list_node *next = current->next;
    if (next == NULL) {
      assert(pthread_mutex_unlock(&par->lock) == 0);
      assert(pthread_mutex_unlock(&current->lock) == 0);

      return 0;
    }

    if (pthread_mutex_lock(&next->lock)) {
      assert(pthread_mutex_unlock(&par->lock) == 0);
      assert(pthread_mutex_unlock(&current->lock) == 0);

      return -1;
    }

    assert(pthread_mutex_unlock(&par->lock) == 0);
    par = current;
    current = next;
  }

  par->next = current->next;
  free(current);
  return 0;
}

