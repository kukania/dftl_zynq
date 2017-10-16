#ifndef __LRU_H__
#define __LRU_H__
#include"utils.h"
#include"cache.h"
#include"skiplist.h"
typedef struct LRU LRU;
typedef struct skiplist skiplist;
typedef struct lru_node{
	struct entry *target;
	struct lru_node *up;
	struct lru_node *down;
	struct LRU* content;
}lru_node;

typedef struct LRU{
	int m_size;
	int n_size;
	lru_node *top;
	lru_node *bottom;
	skiplist *c_body;
}LRU;

void LRU_init(LRU *lru,int,skiplist *);
entry *LRU_get(LRU *lru);
lru_node *LRU_insert(LRU *lru,entry *);
bool LRU_delete(lru_node *lru);
void LRU_update(lru_node *lru);
void LRU_free(LRU*);
#endif
