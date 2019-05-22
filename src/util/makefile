
include ../makefile.inc

LIBNAME = libutil.a


INCLUDEPATH=-I./ -I../util  -I../config -I../case -I../case
LIBPATH=-L./

SRCS = $(wildcard *.cpp *.c)
OBJS = $(patsubst %.cpp,%.o, $(patsubst %.c, %.o, $(patsubst %.cc, %.o, $(SRCS))))


all: $(LIBNAME)
$(LIBNAME): $(OBJS)	
	$(AR)  r $@  $(OBJS) 

clean:
	rm -rf *.o *.d *.a
