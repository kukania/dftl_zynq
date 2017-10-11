#ifndef __SKIPLIST_HEADER
#define __SKIPLIST_HEADER
#include<stdint.h>
#include"dftl.h"
#include"cache.h"
#define MAX_L 30 //max level number
#define PROB 4 //the probaility of level increasing : 1/PROB => 1/4

typedef struct entry entry;
typedef struct w_entry w_entry;
typedef union container{
	entry *e_data;
	w_entry *w_data;
}container;

typedef struct snode{ //skiplist's node
	KEYT key; 
	uint8_t level;
	container d;
	struct snode **list;
}snode;

typedef struct skiplist{
	uint8_t level;
	int size;
	snode *header;
}skiplist;

//read only iterator. don't using iterater after delete iter's now node
typedef struct{
	skiplist *list;
	snode *now;
} sk_iter;

skiplist *skiplist_init(); //return initialized skiplist*
snode *skiplist_find(skiplist*,KEYT); //find snode having key in skiplist, return NULL:no snode
snode *skiplist_insert(skiplist*,KEYT,void *data,bool); //insert skiplist, return inserted snode
int skiplist_delete(skiplist*,KEYT); //delete by key, return 0:normal -1:empty -2:no key
void skiplist_free(skiplist *list,bool);  //free skiplist
void skiplist_clear(skiplist *list,bool); //clear all snode in skiplist and  reinit skiplist
sk_iter* skiplist_get_iterator(skiplist *list); //get read only iterator
snode *skiplist_get_next(sk_iter* iter); //get next snode by iterator
#endif
