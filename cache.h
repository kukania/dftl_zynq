#ifndef __CACHE_H__
#define __CACHE_H__
#include"utils.h"
#include"translation_page.h"
#include"skiplist.h"
#include"LRU.h"
typedef struct lru_node lru_node;
typedef struct entry{
	KEYT key;
	tp *mapping;
	bool e_dirty[KEYN];
	bool dirty;
	lru_node *l_entry;
//	int cnt;
}entry;

typedef struct cache{
	int m_size;
	int n_size;
//	int cnt;
	struct skiplist *body;
	struct LRU *lru;
}cache;

void cache_init(cache *);
void cache_free(cache *);
entry* cache_lookup(cache*,inter_req *req,bool);
//entry* cache_tp_insert(cache*, tp *);
void cache_evict(cache *);
bool cache_load(cache *,KEYT, inter_req *req);
void cache_processing(cache *, inter_req *req);
struct snode* cache_LRU(cache *);
//keyset* cache_find(cache *,KEYT);
#endif
