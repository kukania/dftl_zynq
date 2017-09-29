#ifndef __TP_H__
#define __TP_H__
#include"utils.h"
#include"interface.h"
typedef struct inter_req inter_req;
typedef struct{
	KEYT lpn;
	KEYT ppa;
}keyset;

typedef struct{
	keyset sets[KEYN];
}translation_page;
typedef translation_page tp;

typedef struct{
	keyset tp_lotation[DIRNUM];
}directory;
keyset *tp_find_key(translation_page*,KEYT);
keyset *tp_find_map(directory *dir, KEYT);
keyset tp_write(translation_page* ,inter_req *);
tp* tp_read(KEYT ppa, int fd);
void tp_update(directory *,int dirnum, tp*,int fd);
int convert_ptod(KEYT ppa);
#endif
