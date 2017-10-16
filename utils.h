#ifndef __UTIL_H__
#define __UTIL_H__
#include<sys/time.h>
#include<unistd.h>
#include<stdint.h>
#include"measure.h"
#define KEYT uint32_t
#define KEYN 1024
#define PAGESIZE (8192)
#define INPUTSIZE (1*128*1024)
#define CACHEMEM (1*(1024*1024))//M
#define KEYRANGE (1*(128*1024*1024))//()=1T
#define DIRNUM (KEYRANGE/KEYN)
#define QSIZE 1024

#define SEQUENCE 1


#define PAGENUM (4096)
#define SEGNUM (32*20)
#define MAXPAGE (32*20*4096) //1G=32*4096
#define DTPBLOCK (4)

#ifndef NPRINTOPTION
#define MT(t) measure_stamp((t))
#define MS(t) measure_start((t))
#define ME(t,s) measure_end((t),(s))
#define MP(t) measure_pop((t))
#define MC(t) measure_calc((t))
#define MR(t) measure_res((t))
#define MA(t) measure_adding((t))
#else
#define MS(t) donothing(t)
#define ME(t,s) donothing2((t),(s))
#define MP(t) donothing((t))
#define MC(t) donothing((t))
#endif

#ifndef BOOL
#define BOOL
#ifndef CPP
typedef enum{false,true} bool;
#endif
#endif
#endif
