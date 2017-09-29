#include"dftl.h"
#include"page_manager.h"
#include"translation_page.h"
#include"interface.h"
#include<stdlib.h>
extern dftl_t dftl;
keyset* tp_find_key(translation_page *input, KEYT key){
	return &input->sets[key%1024];
}
int convert_ptod(KEYT ppa){
	int number=0;
	while(ppa/KEYN){
		number++;
		ppa/=1024;
	}
	return number;
}
keyset* tp_find_map(directory *dir, KEYT key){
	int number=convert_ptod(key);
	return &dir->tp_lotation[number];
}

KEYT tp_write(tp* input, int fd){
	KEYT in;
	inter_req *tp_req=(inter_req*)malloc(sizeof(inter_req));
	tp_req->type=TP_W_TYPE;
	tp_req->ppa=getTrPPA(dftl.pm);//get ppa from manager
	tp_req->data=(char*)input;
	tp_req->fd=fd;
	tp_req->end_req=end_req;
	inter_write(tp_req->ppa,tp_req->data,tp_req);
	return in;
}

tp* tp_read(KEYT ppa,int fd){
	tp * res=(tp*)malloc(sizeof(translation_page));
	inter_req *tp_req=(inter_req*)malloc(sizeof(inter_req));
	tp_req->type=TP_R_TYPE;
	tp_req->ppa=ppa;
	tp_req->end_req=end_req;
	pthread_mutex_init(&tp_req->lock,NULL);
#ifdef ENABLE_LIBFTL
	#ifdef ASYNC
		
	#else
		pthread_mutex_lock(&tp_req->lock);
	#endif
#else
	tp_req->data=(char*)malloc(PAGESIZE);
#endif
	inter_read(tp_req->ppa,tp_req->data,tp_req);
	pthread_mutex_lock(&tp_req->lock);
	return res;
}

void tp_update(directory *dir, int dirnum, tp* target,int fd){
	tp * read_s=tp_read(dir->tp_lotation[dirnum].ppa,fd);
	int target_idx=0;
	for(int i=0; i<KEYN; i++){
		if(target->sets[i].lpn==0 && target->sets[i].ppa==0)
			continue;
		else{
			read_s->sets[i]=target->sets[i];
		}
	}
	KEYT after_ppa=tp_write(read_s,fd);
	dir->tp_lotation[dirnum].ppa=after_ppa;
	free(read_s);
}
