#include"interface.h"
#include"dftl.h"
#include<stdlib.h>
#include<stdio.h>
processor process;
extern dftl_t dftl;
pthread_mutex_t task_lock;
void processor_init(){
	process.req_q=new spsc_bounded_queue_t<void*>(QSIZE);
	process.meta_q=new spsc_bounded_queue_t<void*>(QSIZE);
	process.req_num=0;
	process.stopflag=false;
	pthread_mutex_init(&process.empty_lock,NULL);
	pthread_mutex_lock(&process.empty_lock);
	pthread_create(&process.tid,NULL,processor_main,NULL);
	pthread_mutex_init(&task_lock,NULL);
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
	req->key=key;
	req->end_req=end_req;
	processor_assign(req);
	process.req_num++;
}
void end_req(void *in_req){
	inter_req* req=(inter_req*)in_req;
	inter_req* parent=req->parent;
	switch(req->type){
		case W_TYPE:
#ifndef ENABLE_LIBFTL
			free(req->data);//free data
#else

#endif
			break;
		case TP_W_TYPE:
#ifndef ENABLE_LIBFTL
			free((tp*)req->data);//free data;
#else

#endif
			break;
		case R_TYPE:
#ifndef ENABLE_LIBFTL
			free(req->data);//read_done;
#else

#endif
			break;
		case TP_R_TYPE:
#ifndef NOINTER
			//read meta for req;
			parent->meta=req->data;
			parent->type=TP_MR_TYPE|req->type;
			while(!process.meta_q->enqueue((void*)parent)){}
			pthread_mutex_unlock(&req->lock);
#else

#endif
			//req=parent;//free parent
			break;
		case T_TYPE:
			break;
	}
	if(req){
		free(req);
	}
	process.end_num++;
}
void processor_assign(inter_req* req){
	while(!process.req_q->enqueue((void*)req)){
			sleep(0.1);
	}
	pthread_mutex_unlock(&task_lock);
}

void *processor_main(void *in){
	printf("process start!\n");
	while(!process.stopflag){
		inter_req *req;
		void *temp_req;
		while(1){
			if(process.meta_q->dequeue(&temp_req)){
				break;
			}
			if(process.req_q->dequeue(&temp_req)){
				//pthread_mutex_lock(&process.empty_lock);
				break;
			}
			else{
				if(process.stopflag) break;
			}
			sleep(0.1);
		}
		req=(inter_req*)temp_req;
		KEYT key=req->key;
		char *data=req->data;
		switch(req->type){
			case W_TYPE:
				//dftl_write
				dftl_write(&dftl,key,data,req);
				break;
			case R_TYPE:
				dftl_read(&dftl,key,data,req);
				//dftl_read
				break;
			case T_TYPE:
				//dftl_trim
				break;
			default:
				req->type=(req->type&1?R_TYPE:W_TYPE);
				cache_processing(dftl.map,req);
				break;
		}
	}
}
void processor_stop(){

}
void processor_wait(){
	while(process.req_num!=process.end_num+881){
		printf("%d:%d\n",process.req_num,process.end_num);
		sleep(1);
		break;
	}
	process.stopflag=true;
	pthread_join(process.tid,NULL);
}
void processor_free(){
	delete process.req_q;
	delete process.meta_q;

}
void inter_write(KEYT key, char *data, inter_req *req){
#ifdef ENABLE_LIBFTL//for libmemio

#else//for posix
	int fd;
	fd=dftl.fd;
	if(lseek64(fd,((off64_t)PAGESIZE)*key,SEEK_SET)==-1){
		printf("lseek error");
	}
	write(fd,data,PAGESIZE);
#ifndef NOINTER
	if(req->end_req!=end_req){
		printf("??\n");
	}
	req->end_req(req);
#else
	if(req!=NULL && req->type==TP_W_TYPE){
		free((tp*)data);
	}
	else{
		free(data);
	}
	free(req);
#endif
#endif

}

void inter_read(KEYT key, char *data, inter_req *req){
#ifdef ENABLE_LIBFTL//for libmemio

#else//for posix
	int fd;
	fd=dftl.fd;
	if(lseek64(fd,((off64_t)PAGESIZE)*key,SEEK_SET)==-1){
		printf("lseek error");
	}
	read(fd,data,PAGESIZE);
#ifndef NOINTER
	req->end_req(req);
#else
	free(req);
#endif
#endif
}

void inter_trim(KEYT key,char *data, inter_req *req){
#ifdef ENABLE_LIBFTL//for libmemio

#else//for posix

#endif
}
