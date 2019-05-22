
include ../makefile.inc

INCLUDEPATH=-I.. -I../util -I../case -I../driver
LIBPATH=-L.

ifeq ($(ODBC_VERSION),unixODBC)
LINKLIB = -lodbc
else
LIBPATH +=$(TRAFODION_LIB_PATH)
LINKLIB =-ltrafodbc64 -lpthread -licuuc -licudata
endif
LINKLIB +=-lstdc++ -lm -lrt -lpthread -lodbc

LINKLIBS=../case/libcase.a
LINKLIBS +=../driver/libdriver.a
LINKLIBS +=../util/libutil.a

OBJS=main.o

APP=odbctest
all:$(APP) 

$(APP): $(OBJS) 
	$(CXX) $^  $(CPPFLAGS) $(LIBPATH) $(LINKLIB) $(LINKLIBS)  -o $@
	cp -rf $(APP) ../bin
clean:
	rm -f $(OBJS) *.d $(APP) 

