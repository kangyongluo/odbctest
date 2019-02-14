
SQL_TYPE=TRAFODION
#SQL_TYPE=MySQL

CFLAGS=-O -Wall -DDEBUG -std=c99 -g -w -Wno-missing-braces
CPFLAGS=-O -Wall -Wno-deprecated

ifeq ($(SQL_TYPE),TRAFODION)
INCLUDEPATH+=-I/home/lib/odbc/drivers_unix/linux64
endif

LINKLIB=-lstdc++ -lm -lrt -lpthread
ifeq ($(SQL_TYPE),TRAFODION)
#LINKLIB += -L /home/lib/odbc/drivers_unix/linux64 -ltrafodbc64 -lpthread -lz -licuuc -licudata
LINKLIB += -L /home/lib/r2.3_20170714_0531 -ltrafodbc64 -lpthread -lz -licuuc -licudata
CFLAGS+= -D__TRAFODION__ -fshort-wchar -D__SQLWCHAR_ISSHORT
else
LINKLIB += -lodbc
endif


all: testgetsetco debugForkeys debugSQLColumnPrivileges debugSQLSetStmtAttr debugSQLBindParameter debugSQLBindCol21\
	debugSqlgetdataver3 debugSQLGetData 

testgetsetco: 
	gcc odbc.c testgetsetco.c  $(CFLAGS) $(LINKLIB) $(LINKLIBS)  -o $@
debugForkeys: 
	gcc odbc.c debugForkeys.c  $(CFLAGS) $(LINKLIB) $(LINKLIBS)  -o $@
debugSQLColumnPrivileges: 
	gcc odbc.c debugSQLColumnPrivileges.c  $(CFLAGS) $(LINKLIB) $(LINKLIBS)  -o $@
debugSQLSetStmtAttr: 
	gcc odbc.c debugSQLSetStmtAttr.c  $(CFLAGS) $(LINKLIB) $(LINKLIBS)  -o $@
debugSQLBindParameter: 
	gcc odbc.c debugSQLBindParameter.c  $(CFLAGS) $(LINKLIB) $(LINKLIBS)  -o $@
debugSQLBindCol21: 
	gcc odbc.c debugSQLBindCol21.c  $(CFLAGS) $(LINKLIB) $(LINKLIBS)  -o $@
debugSqlgetdataver3: 
	gcc odbc.c debugSqlgetdataver3.c  $(CFLAGS) $(LINKLIB) $(LINKLIBS)  -o $@
debugSQLGetData: 
	gcc odbc.c debugSQLGetData.c  $(CFLAGS) $(LINKLIB) $(LINKLIBS)  -o $@

install:

uninstall:
	

clean:
	rm -f $(OBJS) *.d *.o testgetsetco debugForkeys debugSQLColumnPrivileges debugSQLSetStmtAttr debugSQLBindParameter debugSqlgetdataver3 debugSQLGetData 
