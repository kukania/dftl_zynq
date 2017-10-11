#ifndef __Q_H__
#define __Q_H__
#include"utils.h"
typedef struct qnode{

}qnode;

typedef struct queue{
	KEYT *body;
	int f;
	int r;
	int m_size;
	int n_size;
}queue;

void queue_init(queue*,int size);
KEYT* queue_dequeue(queue*);
int queue_enqueue(queue*,KEYT value);
void queue_free(queue*);
#endif
