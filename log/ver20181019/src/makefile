cxx=g++
CPPFLAGS=-o -Wall -Dunixcli -g -w 
INCLUDE=-I./util/ -I./driver  -I./case

ODBC_VERSION =UNIXODBC

ifeq ($(ODBC_VERSION), UNIXODBC)
LINKLIB=-lodbc -lpthread -DUNIXODBC
else
LIBPATH+=-L/home/kangyong/lib64
LINKLIB=-ltrafodbc64 -lpthread -licuuc -licudata -liconv
endif

OBJS = ./util/util.cpp
OBJS += ./util/config.cpp
OBJS += ./util/global.cpp
OBJS += ./driver/odbcdriver.c
OBJS += ./driver/rowset.c
OBJS += ./driver/SQLColumns.c
OBJS += ./driver/SQLPutData.c
OBJS += ./driver/SQLTables.c
OBJS += ./case/testSQL.cpp
OBJS += ./case/testSQLColumns.cpp
OBJS += ./case/testSQLTables.cpp
OBJS += ./case/testODBC.cpp
OBJS += ./main/main.cpp

SUBDIRS=util driver case main

all:odbctest 
odbctest: $(OBJS)
	$(CXX) $^ $(CPPFLAGS) $(INCLUDE) $(LIBPATH) $(LINKLIB) -o $@ 
	mv odbctest bin
	rm -rf *.o
clean:
	rm -rf *.o *.d odbctest *.a
