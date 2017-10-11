#ifndef __CACHE_H__
#define __CACHE_H__
#include"utils.h"
#include"translation_page.h"
#include"skiplist.h"
#include"LRU.h"
typedef struct entry{
	tp *mapping;
	bool e_dirty[KEYN];
	bool dirty;
	int cnt;
}entry;

typedef struct cache{
	int m_size;
	int n_size;
	int cnt;
	struct skiplist *body;
	struct LRU *lru;
}cache;

void cache_init(cache *);
void cache_free(cache *);
entry* cache_insert(cache*, keyset,bool);
void cache_evict(cache *);
tp* cache_load(cache *,KEYT );
struct snode* cache_LRU(cache *);
keyset* cache_find(cache *,KEYT);
#endif
