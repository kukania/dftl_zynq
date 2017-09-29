#ifndef __dftl_h__
#define __dftl_h__
#include"cache.h"
#include"translation_page.h"
#include"page_manager.h"
typedef struct dftl{
	cache *map;
	directory tp_dir;
	PM *pm;
}dftl_t;

void dftl_init(dftl_t*);
void dftl_write(dftl_t *,KEYT key, char *data, inter_req *req);
void dftl_read(dftl_t *,KEYT key, char *data, inter_req *req);
void dftl_trim(dftl_t*,KEYT key, char *data, inter_req *req);
void dftl_free(dftl_t *);
#endif
