CC=$(HOST)-g++ -D_FILT_OFFSET_BITS=64

CFLAGS	+=\
		  -g\
		  -std=c++11\
		  -DCPP\
#		  -DNOINTER\

INCLUDES := -I$(PWD)\

LIBS 	:=\
	-lpthread\
#	-lefence\

SRCS 	:=\
	translation_page.c\
	cache.c\
	LRU.c\
	interface.cpp\
	page_manager.c\
	dftl.c\
	skiplist.c\
	measure.c\
	queue.c\


OBJS	:=\
	$(patsubst %.c,%.o,$(SRCS))\

OBJS	:=\
	$(patsubst %.cpp,%.o,$(OBJS))\

TARGETOBJ	:=\
	$(addprefix object/,$(OBJS))\

all: LIBDFTL

CACHE: $(TARGETOBJ)
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

LIBDFTL	: libdftl.a dftl_main.c
	$(CC) $(INCLUDES) $(CFLAGS) -o $@ dftl_main.c libdftl.a $(LIBS)

libdftl.a	: $(TARGETOBJ)
	$(AR) r $(@) $(TARGETOBJ)
.c.o	:
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

.cpp.o	:
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

object/%.o: %.c utils.h
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

object/%.o: %.cpp utils.h
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

clean	:
	@$(RM) object/*.o
	@$(RM) LIBDFTL
	@$(RM) *.o
