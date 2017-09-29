#ifndef __PM_H__
#define __PM_H__
#include"utils.h"
#include<pthread.h>
#include<limits.h>
#include<stdint.h>
#include<queue>
typedef struct{
	uint8_t bitset[PAGENUM/8];
	int number;
	int invalid_n;
	bool erased;
}block;

typedef struct{
	block *blocks;
	std::queue<KEYT> *ppa;
	pthread_mutex_t ppa_lock;
	int reserve_block;
	int reserve_block_point;
	int start_block_n;
	int size;
	bool isdata;
}segment;

typedef struct{
	segment *data_sets;
	segment *tp_sets;
	uint64_t *oob;
}PM;

inline uint64_t KEYSET(uint64_t &des,uint32_t src){
	uint64_t temp=src;
	des=des | (temp<<32);
	return des;
}

inline uint64_t FLAGSET(uint64_t &des, bool flag){
	des=des | flag;
	return des;
}
inline KEYT KEYGET(uint64_t des){
	KEYT temp=UINT_MAX;
	return temp&(des>>32);
}
inline bool FLAGGET(uint64_t &des){
	return des & 1;
}
/*for user*/
void PM_init(PM*);
KEYT getDataPPA(PM*);
KEYT getTrPPA(PM*);
void PM_free(PM*);

#endif

