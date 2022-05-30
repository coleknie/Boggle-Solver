#include <stdlib.h>
#include <string.h>

#include "llist.h"

//initialize a linked list
llist_t * ll_init(){
  llist_t * ll = malloc(sizeof(llist_t));
  ll->size = 0;
  ll->head = NULL;
  return ll;
}

//delete a linked list
void ll_delete(llist_t * ll){
  ll_node_t * cur_node = ll->head;
  ll_node_t * next_node = cur_node;
  while(cur_node != NULL){
    free (cur_node->val);
    next_node = cur_node->next;
    free(cur_node);
    cur_node = next_node;
  }
  free(ll);
}


//push an item onto the front of the list
void ll_push(llist_t * ll, char * s){
  if(ll->size == 0){
    ll->head = malloc(sizeof(ll_node_t));
    ll->head->val = strdup(s);
    ll->head->next = NULL;
  }
  else{
    ll_node_t * temp_node = ll->head;
    ll_node_t * new_node = malloc(sizeof(ll_node_t));
    new_node-> val = strdup(s);
    new_node->next = ll->head;
    ll->head = new_node;
  }
  ll->size++;
}




