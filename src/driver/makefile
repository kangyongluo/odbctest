
include ../makefile.inc

LIBNAME = libdriver.a


INCLUDEPATH=-I./ -I../util

SRCS = $(wildcard *.cpp *.c)
OBJS = $(patsubst %.cpp,%.o, $(patsubst %.c, %.o, $(patsubst %.cc, %.o, $(SRCS))))

all: $(LIBNAME)
$(LIBNAME): $(OBJS)	
	$(AR)  r $@  $(OBJS) 

clean:
	rm -rf *.o *.d  *.a
