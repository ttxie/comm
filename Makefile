.PHONY: all install clean distclean

CC = gcc

KERNELSRCS = $(shell echo example/*.c)
KERNELOBJS = $(KERNELSRCS:.c=.o)
KERNELPROGS = $(KERNELSRCS:.c=)

SRCS = $(shell echo src/*.c)
OBJS = $(SRCS:.c=.o)

ALIB = src/libcomm.a


CFLAGS = -g -O3 -W -fPIC
INCLUDES = -I./src/ -I/usr/local/include/ -I/usr/local/mysql/include
LIBS = -L./src -L/usr/local/mysql/lib/  -L/usr/local/lib/ -lcomm -lm -lhiredis -llua -lz -lmysqlclient  -Wl,-E



############# main ##############

all: $(KERNELPROGS)

$(KERNELPROGS): $(ALIB) $(KERNELOBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $@.o $(LIBS)
        
$(ALIB): $(OBJS)
	ar -rc src/libcomm.a $(OBJS)
	ranlib src/libcomm.a


.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ -c $<

install: all
	mkdir -p /usr/local/include/comm/
	cp -f src/*.h /usr/local/include/comm/
	cp -f src/libcomm.a /usr/local/lib


clean:
	rm -f  $(KERNELPROGS) $(OBJS) $(KERNELOBJS)
	rm -f src/libcomm.a

distclean:
	rm -f /usr/local/lib/libcomm.a
	rm -rf /usr/local/include/comm/
        


