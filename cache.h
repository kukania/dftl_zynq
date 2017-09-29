#ifndef __CACHE_H__
#define __CACHE_H__
#include"utils.h"
#include"translation_page.h"
#include"skiplist.h"
#include"LRU.h"
typedef struct entry{
	keyset sets;
	bool dirty;
}entry;

typedef struct cache{
	int m_size;
	int n_size;
	int cnt;
	struct skiplist *body;
	struct LRU *lru;
	int fd;
}cache;

void cache_init(cache *,int);
void cache_free(cache *);
void cache_insert(cache*, entry *);
void cache_evict(cache *);
entry* cache_find(cache *,KEYT);
#endif
