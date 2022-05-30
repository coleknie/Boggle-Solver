#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "llist.h"
#include "hashmap.h"
#include "boggle.h"

#define MAX_WORD_LEN 1024

//constant values on all the dice in global
const char * boggle_dice[] = {
        "AAAFRS", "AAEEEE", "AAFIRS", "ADENNN", "AEEEEM",
        "AEEGMU", "AEGMNN", "AFIRSY", "BJKQXZ", "CCENST",
        "CEIILT", "CEILPT", "CEIPST", "DDHNOT", "DHHLOR",
        "DHLNOR", "DHLNOR", "EIIITT", "EMOTTT", "ENSSSU",
        "FIPRSY", "GORRVW", "IPRRRY", "NOOTUW", "OOOTTU",
};


//initialize a boggle instance (do not edit until after the TODO: comment)
boggle_t * bg_init(int seed, char * dict_fn){
  //You get this first part of the function for free :)

  //set a random seed
  srandom(seed);

  //create the instance
  boggle_t * bg = calloc(sizeof(boggle_t), 1);

  //setup the board by rolling dice
  int used[BOGGLE_NUM_DICE] = {0};
  for(int i=0;i<BOGGLE_NUM_DICE;i++){
    while(1){
      //choose a dice to roll
      int d = random()%BOGGLE_NUM_DICE;
      if(used[d]) continue; //keep choosing if already rolles

      //choose a random side
      int s = random()%BOGGLE_DICE_SIDES;
      bg->board[i/BOGGLE_DIMENSION][i%BOGGLE_DIMENSION] = boggle_dice[d][s];
      used[d] = 1; //mark that dice as rolled
      break;
    }
  }

  FILE * dict_f;
  char word[MAX_WORD_LEN];
  char * line;

  if((dict_f = fopen(dict_fn,"r")) == NULL){
    perror("opening dictionary file");
    exit(1);
  }

  bg->dict = hm_init();

  while(fscanf(dict_f, "%1023s", word) != EOF){
    for(char *c=word;(*c=toupper(*c)) != 0;c++);
    hm_add(bg->dict,word);
  }
  fclose(dict_f);

  return bg;
}

//delete/free the boggle instance (do not edit)
void bg_delete(boggle_t * bg){
  //You get this function for free :)
  hm_delete(bg->dict); 
  free(bg);
}


//print the board using fancy unicode (do not edit)
void bg_print(boggle_t * bg){
  printf(".-----------.\n");
  for (int r = 0; r < 5; r++) {
    printf("| ");
    for (int c = 0; c < 5; c++) {
      printf("%c",bg->board[r][c]);      
      if (bg->board[r][c]=='Q')
	printf("u");
      else
	printf(" ");
    }
    printf("|\n");
  }
  printf("'-----------'\n");
}


//count the points up (do not edit)
// 3 and 4 length words are worth 1 point
// 5 length worth 2
// 6 length worth 3
// 7 length worth 5
// 8 length worth 11
// note we stop searching at 8 length words 
int bg_count_points(boggle_t * bg, llist_t * words){
  int pts=0;
  for(ll_node_t * n = words->head; n; n = n->next){
    if(hm_check(bg->dict,n->val)){
      switch(strlen(n->val)){
      case 0:
      case 1:
      case 2: break;
      case 3:
      case 4:
	pts+=1;
	break;
      case 5:
	pts+=2;
	break;
      case 6:
	pts+=3;
	break;
      case 7:
	pts+=5;
	break;
      default:
	pts+=11;
      }
    }
  }
  return pts; 
}


//TODO: complete the board search algorithm below using your hashmap

//helper functions for the recursion, declaring here so can call below
void _bg_all_words(boggle_t * bg, //boggle instance
		   hashmap_t * hm, //hashmap to track found words
		   int r, int c, //current row (r) / column (c)
		   char * last_word, //what the last word was
		   int last_visited[BOGGLE_DIMENSION][BOGGLE_DIMENSION] //what has been previously visited
		                                                        //don't want to double back
		   );

//entry function for finding all the words
llist_t * bg_all_words(boggle_t *bg){

  //0'ed out -- currently visited nowhere (all 0's), and the current word is the empty string
  int visited[BOGGLE_DIMENSION][BOGGLE_DIMENSION] = {0};
  char cur_word[9] = {0}; //empty string 
  // Note using byte array of length 9 b/c max word length is 8!

  //initialize a hashmap to track status
  hashmap_t * hm = hm_init();

  //start a recurssion at every space on the board
  for(int i=0;i<BOGGLE_DIMENSION;i++){
    for(int j=0;j<BOGGLE_DIMENSION;j++){
      _bg_all_words(bg, hm, i, j, cur_word, visited);
    }
  }

  llist_t * words = ll_init();

  for(int i = 0; i < hm->num_buckets; i++){
    llist_t * cur_bucket = hm->buckets[i];
    ll_node_t * cur_node = cur_bucket->head;
    while(cur_node != NULL){
      ll_push(words, cur_node->val);
      cur_node = cur_node->next;
    }
  }
  
  hm_delete(hm); //delete the hashmap

  return words;
  //return NULL; //clearly, this won't be null when your done.
}



void _bg_all_words(boggle_t *bg, hashmap_t * hm, int r, int c,
		   char * last_word, int last_visited[BOGGLE_DIMENSION][BOGGLE_DIMENSION]){

  if(r >= BOGGLE_DIMENSION || r < 0 || c >= BOGGLE_DIMENSION || c < 0 || strlen(last_word) >= 8 || last_visited[r][c] == 1){
    return;
  }

  char cur_word[9] = {0}; //use to store the current word

  int index = strlen(last_word);

  for(int i = 0; i < strlen(last_word); i++){
    cur_word[i] = last_word[i];
  }
  
  cur_word[index] = bg->board[r][c];
  int a = strlen(cur_word);

  if(bg->board[r][c] == 'Q'){
    cur_word[index+1] = 'U';
    cur_word[index+2] = '\0';
  }
  else{
    cur_word[index+1] = '\0';
  }


  if(hm_check(bg->dict, cur_word) && !hm_check(hm, cur_word) && strlen(cur_word) >= 3 && strlen(cur_word) <= 8){
    hm_add(hm, cur_word);
  }


  int visited[BOGGLE_DIMENSION][BOGGLE_DIMENSION] = {0};

  for(int i = 0; i < BOGGLE_DIMENSION; i++){
    for(int j = 0; j < BOGGLE_DIMENSION; j++){
      visited[i][j] = last_visited[i][j];
    }
  }

  visited[r][c] = 1;



  // 1. Return out of recurssion if ...
  //    - the word is too long (only words up to 8)
  //    - the row/colomn is off the board
  //    - already visited this dice
  // 2. Consider the current word which is the last word with letter at row/column appended to it
  //    - be careful about Q's (they get a free U)!
  // 3. Is the current word a word I haven't seen? If so, save it in the hashmap
  // 4. Mark this row/column as visited
  // 5. Perform 8 recurssion for all possible next row/colimns
  //    - note that the first thing you do in this function is check
  //      if the recursion should return so there's no need to check,
  //      just do the recursive call!
  //
    
  //don't really want to recurse until your read */
  
  //Perform 8 recursive calls on all surrounding dice...

  _bg_all_words(bg,hm,r-1,c,cur_word,visited);
  _bg_all_words(bg,hm,r-1,c-1,cur_word,visited);
  _bg_all_words(bg,hm,r-1,c+1,cur_word,visited);
  _bg_all_words(bg,hm,r+1,c,cur_word,visited);
  _bg_all_words(bg,hm,r+1,c-1,cur_word,visited);
  _bg_all_words(bg,hm,r+1,c+1,cur_word,visited);
  _bg_all_words(bg,hm,r,c-1,cur_word,visited);
  _bg_all_words(bg,hm,r,c+1,cur_word,visited);

}

