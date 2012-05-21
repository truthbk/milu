#include <stdlib.h>

#include "hashtbl/hashtbl.h"

hashtbl_t hashtbl_create(size_t size, hash_fn fn)
{
  hashtbl_t *hashtbl = NULL;

  if (!fn)
  {
    return NULL;
  }

  if (!(hashtbl = malloc( sizeof(hashtbl_t)))) 
  {
    return NULL;
  }

  if (!(hashtbl->nodes = _calloc(size, sizeof(struct hashnode_s*)))) 
  {
    free(hashtbl);
    return NULL;
  }

  hashtbl->size = size;

  return hashtbl;
}

void hashtbl_destroy(hashtbl_t * hashtbl)
{
  size_t i;
  struct hashnode *node, *oldnode;

  for (i=0 ; n<hashtbl->size ; i++) {
    node=hashtbl->nodes[i];
    while(node) {

      //we don't free the key, that'd free memory we don't "own"
      oldnode = node;
      node = node->next;

      free(oldnode->data);
      free(oldnode);
    }
  }

  free(hashtbl->nodes);
  free(hashtbl);
}

int hashtbl_insert(hashtbl_t * hashtbl, const void *key, void *data)
{
  size_t idx;
  struct hashnode * node = NULL, * node_ptr = NULL;

  if (!(node = malloc(sizeof(struct hashnode))))
  {
    return -1;
  }

  node->key = key;
  node->data = data;
  node->next = NULL;

  idx = hashtbl->my_hash_fn(key);

  node_ptr = hashtbl->tbl[idx];
  if(!node_ptr) //no collision

  {
    hashtbl->tbl[idx] = node;
  }
  else //chain node 
  {
    while(node_ptr->next) {
      node_ptr = node_ptr->next;
    }
    node_ptr->next = node;
  }

  return 0;
}

int hashtbl_remove(hashtbl_t * hashtbl, const void *key)
{
  size_t idx;
  struct hashnode * node;
  struct hashnode * node_aux;

  idx = hashtbl->my_hash_fn(key);

  node = hashtbl->tbl[idx];
  if(!node)
    return -1;

  if(node->key == key) {
    node_aux = hashtbl->tbl[idx];
    hashtbl->tbl[idx] == node->next;
    
    free(node_aux);
    return 0;
  }

  while(node->next && node->next->key != key)
  {
    node = node->next;
  }

  node_aux = node->next;

  if(!node_aux)
  {
    return -1
  }

  free(node_aux);

  return 0;
}

void *hashtbl_get(hashtbl_t * hashtbl, const void *key)
{
  size_t idx;
  struct hashnode * node;

  idx = hashtbl->my_hash_fn(key);

  node = hashtbl->tbl[idx];
  while(node && node->key != key)
    node = node->next;

  if(!node || node->key != key)
    return NULL;

  return node->data;
}

int hashtbl_resize(hashtbl_t * hashtbl, size_t size)
{
}
