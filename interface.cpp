#include"interface.h"
#include"dftl.h"
#include<stdlib.h>
#include<stdio.h>
processor process;
extern dftl_t dftl;
void processor_init(){
	process.req_q=new spsc_bounded_queue_t<void*>(QSIZE);
	process.req_num=0;
	process.stopflag=false;
	pthread_mutex_init(&process.empty_lock,NULL);
	pthread_mutex_lock(&process.empty_lock);
	pthread_create(&process.tid,NULL,processor_main,NULL);
}

void make_req(KEYT key, char *data, uint32_t type){
	inter_req *req=(inter_req*)malloc(sizeof(inter_req));
	switch(type){
		case W_TYPE:
			break;
		case R_TYPE:
			break;
		case T_TYPE:
			break;
	}
	req->type=type;
	req->data=data;
	req->ppa=key;
	req->end_req=end_req;
	processor_assign(req);
	process.req_num++;
}
void end_req(void *in_req){
	inter_req* req=(inter_req*)in_req;
	switch(req->type){
		case W_TYPE:
			break;
		case R_TYPE:
			break;
		case T_TYPE:
			break;
	}
	process.end_num++;
}
void processor_assign(inter_req* req){
	while(!process.req_q->enqueue((void*)req)){}
	pthread_mutex_unlock(&process.empty_lock);
}

void *processor_main(void *in){
	while(!process.stopflag){
		inter_req *req;
		void *temp_req;
		while(!process.req_q->dequeue(&temp_req)){
			pthread_mutex_lock(&process.empty_lock);
		}
		req=(inter_req*)temp_req;

		switch(req->type){
			case W_TYPE:
				//dftl_write
				break;
			case R_TYPE:
				//dftl_read
				break;
			case T_TYPE:
				//dftl_trim
				break;
		}
	}
}
void processor_stop(){

}

void inter_write(KEYT key, char *data, inter_req *req){
#ifdef ENABLE_LIBFTL//for libmemio

#else//for posix
	int fd;
	#ifndef NOINTER
		fd=req->fd;
	#else
		fd=dftl.map->fd;
	#endif
 	if(lseek64(fd,((off64_t)PAGESIZE)*key,SEEK_SET)==-1){
		printf("lseek error");
	}
	write(fd,data,PAGESIZE);
	#ifndef NOINTER
		req->end_req(req);
	#endif
#endif
}

void inter_read(KEYT key, char *data, inter_req *req){
#ifdef ENABLE_LIBFTL//for libmemio

#else//for posix
	int fd;
	#ifndef NOINTER
		fd=req->fd;
	#else
		fd=dftl.map->fd;
	#endif
 	if(lseek64(fd,((off64_t)PAGESIZE)*key,SEEK_SET)==-1){
		printf("lseek error");
	}
	read(fd,data,PAGESIZE);
	#ifndef NOINTER
		req->end_req(req);
	#endif
#endif
}

void inter_trim(KEYT key,char *data, inter_req *req){
#ifdef ENABLE_LIBFTL//for libmemio

#else//for posix
	
#endif
}
