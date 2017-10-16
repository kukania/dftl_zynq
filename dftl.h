#ifndef __dftl_h__
#define __dftl_h__
#include"cache.h"
#include"skiplist.h"
#include"translation_page.h"
#include"page_manager.h"
typedef struct cache cache;
typedef struct skiplist skiplist;
typedef struct w_entry{
	char *data;
	inter_req *req;
}w_entry;

typedef struct dftl{
	int fd;
	cache *map;
	directory tp_dir;
	skiplist* w_buffer;
	PM *pm;
}dftl_t;

void dftl_init(dftl_t*);
void dftl_write(dftl_t *,KEYT key, char *data, inter_req *req);
void dftl_writeTbuffer(dftl_t*,inter_req*);
void dftl_read(dftl_t *,KEYT key, char *data, inter_req *req);
void dftl_trim(dftl_t*,KEYT key, char *data, inter_req *req);
void dftl_free(dftl_t *);
#endif
