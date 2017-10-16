#ifndef __INF_H__
#define __INF_H__
#include<pthread.h>
#include"utils.h"
#include"lockfreeq.h"
#define W_TYPE	0
#define R_TYPE	1
#define T_TYPE	4
#define TP_W_TYPE 2
#define TP_R_TYPE 3
#define TP_MR_TYPE 8
typedef struct inter_req{
	uint32_t type;
	char *data;
	KEYT key;
	KEYT ppa;
	int fd;
	pthread_mutex_t lock;
	inter_req *parent;
	char *meta;
	void *algo;
	void (*end_req)(void *);
}inter_req; 

typedef struct processor{
	spsc_bounded_queue_t<void *> *req_q;
	spsc_bounded_queue_t<void *> *meta_q;
	pthread_mutex_t empty_lock;
	pthread_t tid;
	int req_num;
	int end_num;
	bool stopflag;
}processor;

void processor_init();
void processor_assign(inter_req*);
void processor_stop();
void processor_wait();
void processor_free();
void make_req(KEYT key, char *data, uint32_t type);
void end_req(void*);
void *processor_main(void*);
void inter_write(KEYT key, char *data,inter_req*);
void inter_read(KEYT key, char *data,inter_req*);
void inter_trim(KEYT key, char *data,inter_req*);
#endif
