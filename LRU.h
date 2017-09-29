#ifndef __LRU_H__
#define __LRU_H__
#include"utils.h"
#include"cache.h"
typedef struct lru_node{
	struct entry *target;
	struct lru_node *up;
	struct lru_node *down;
}lru_node;

typedef struct LRU{
	int m_size;
	int n_size;
	lru_node *top;
	lru_node *bottom;
}LRU;

void LRU_init(LRU *lru,int );
entry *LRU_get(LRU *lru);
void LRU_insert(LRU *lru,entry *);
void LRU_delete(LRU *lru, entry *);
void LRU_free(LRU*);
#endif
