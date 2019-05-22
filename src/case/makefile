
include ../makefile.inc

LIBNAME = libcase.a


INCLUDEPATH=-I./ -I../util 
LIBPATH=-L./

SRCS = $(wildcard *.cpp *.c)
OBJS = $(patsubst %.cpp,%.o, $(patsubst %.c, %.o, $(patsubst %.cc, %.o, $(SRCS))))


all:$(LIBNAME) 
$(LIBNAME): $(OBJS)	
	$(AR)  r $@  $(OBJS) 

clean:
	@for d in $(SUBDIRS); do (cd $$d && $(MAKE) clean); done	
	rm -rf *.o *.d $(LIBNAME) *.a
