#ifndef __UTIL_H__
#define __UTIL_H__
#include<sys/time.h>
#include<unistd.h>
#include<stdint.h>
#define KEYT uint32_t
#define KEYN (1024)
#define PAGESIZE (8192)
#define INPUTSIZE (128*1024)
#define CACHEMEM (10*(1024*1024))//M
#define DIRNUM (INPUTSIZE/KEYN)
#define QSIZE 1024

#define SEQUENCE 1


#define PAGENUM (4096)
#define SEGNUM (32*20)
#define MAXPAGE (32*20*4096) //1G=32*4096
#define DTPBLOCK (4)


#ifndef BOOL
#define BOOL
#ifndef CPP
typedef enum{false,true} bool;
#endif
#endif
#endif
