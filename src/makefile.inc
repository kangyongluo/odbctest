SHELL		= /bin/bash

ODBC_VERSION = unixODBC

ifeq ($(ODBC_VERSION),unixODBC)
CFLAGS=-Wall -g -w -Dunixcli -DUNIXODBC
CPPFLAGS=-Wall -g -w -Dunixcli 
else
CFLAGS=-o -Wall -g -w -Dunixcli 
CPPFLAGS=-o -Wall -g -w -Dunixcli
TRAFODION_LIB_PATH=-L /home/kangyong/lib64
endif


CXX		= g++
AS		= as
LD		= ld
CC		= gcc
CPP		= $(CC) -E
AR		= ar
RANLIB		= ranlib
STRIP		= strip
MAKE		= make


ALLFLAGS	= $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS)


#
# Explicit rules
#
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDEPATH) -o $@ $< -c

%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(INCLUDEPATH) -o $@ $< -c

%.o: %.cc
	$(CXX) $(CPPFLAGS) $(INCLUDEPATH) -o $@ $< -c
