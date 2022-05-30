#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <math.h>
#include <string.h>

#include "llist.h"
#include "hashmap.h"

//local function for hashing an input value (do not edit)
unsigned int _hash(hashmap_t * hm, char * val){
  //based on Java's String.hashcode()
  double k = 0;
  double len = (double) strlen(val);
  int i=1;
  for(char * c = val; *c ; c++,i++){
    k += (*c) * pow(31,len-i);
  }

  return (unsigned int) (((unsigned long) k) % hm->num_buckets);
}


//local function: resize the hashmap by doubling the number of buckets and rehashing
void _resize(hashmap_t * hm){

  int temp_num_buckets = hm->num_buckets;
  llist_t ** temp_buckets = hm->buckets;
  hm->num_buckets = hm->num_buckets * 2;

  hm->buckets = calloc(sizeof(llist_t *), hm->num_buckets);

  for(int i = 0; i < hm->num_buckets; i++){
    hm->buckets[i] = ll_init();
  }

  for(int i = 0; i < temp_num_buckets; i++){
    llist_t * cur_bucket = temp_buckets[i];
    ll_node_t * cur_node = cur_bucket->head;
    while(cur_node != NULL){
      hm_add(hm, cur_node->val);
      cur_node = cur_node->next;
    }
    ll_delete(temp_buckets[i]);
  }
  free(temp_buckets);
}


//initliaze a hashmap with initial number of buckets
hashmap_t * hm_init(){  



  //create the hashmap
  hashmap_t * hm = calloc(sizeof(hashmap_t),1);

  //TODO the rest of the setup!

  hm->size = 0;
  hm->num_buckets = HM_INIT_NUM_BUCKETS;
  hm->buckets = calloc(sizeof(llist_t *), HM_INIT_NUM_BUCKETS);

  for(int i = 0; i < hm->num_buckets; i++){
    hm->buckets[i] = ll_init();
  }

  return hm;
}


//delete/deallocate the hashmap
void hm_delete(hashmap_t * hm){

  for(int i = 0; i < hm->num_buckets; i++){
    ll_delete(hm->buckets[i]);
  }

  free(hm->buckets);

  //TODO: properly deallocate

  //delete the hashmap
  free(hm);
}

//add a string value to the hashmap
void hm_add(hashmap_t * hm, char * val){
  // You get this function for free :)
  
  //check the load on the hashtable, if > max load, resize!
  if(((hm->size+1)/(double) hm->num_buckets) > HM_MAX_LOAD){
    _resize(hm); 
  }

  unsigned int h = _hash(hm,val); //get hash
  ll_push(hm->buckets[h],val); //note that list does string dup
  hm->size++; //increase size


}

//see if a string value is in the hashmap
bool hm_check(hashmap_t * hm, char * val){

  unsigned int h = _hash(hm, val);

  llist_t * bucket = hm->buckets[h];

  ll_node_t * cur_node = bucket->head;
  while(cur_node != NULL){
    if(strcmp(cur_node->val, val) == 0){
      return true;
    }
    cur_node = cur_node->next;
  }

  return false; //otherwise false
}


