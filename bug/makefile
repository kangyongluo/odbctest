cxx=g++
CPPFLAGS=-o -Wall -Dunixcli -g -w 
INCLUDE=-I.

ODBC_VERSION =UNIXODBC

ifeq ($(ODBC_VERSION), UNIXODBC)
LINKLIB=-lodbc -lpthread -DUNIXODBC
else
LIBPATH+=-L/home/kangyong/lib64
LINKLIB=-ltrafodbc64 -lpthread -licuuc -licudata
endif

M-OBJS = M-2982
M-OBJS += M-2713
M-OBJS += M-2714
M-OBJS += M-2707
M-OBJS += M-4703
M-OBJS += M-4710
M-OBJS += M-6893
M-OBJS += M-7691
M-OBJS += M-8380
M-OBJS += M-8676
M-OBJS += M-9638
M-OBJS += M-9089
M-OBJS += M-9094
M-OBJS += M-9933
M-OBJS += M-SQLColumns
M-OBJS += M-SQLPutData
M-OBJS += M-SQLSetConnectOption
M-OBJS += M-SQLSetStmtOption
M-OBJS += M-SQLGetDiagField
M-OBJS += M-SQLBrowseConnect
M-OBJS += M-SQLDescribeCol
M-OBJS += M-SQLGetStmtAttr
M-OBJS += M-SQLGetDiagRec
M-OBJS += M-XXXX
M-OBJS += M-SQLGetDescFiled
M-OBJS += SQLTablePrivileges 

all:$(M-OBJS) M-CLEAN

M-2982:M-2982.o util.o
	$(CXX) $^ $(CPPFLAGS) $(INCLUDE) $(LIBPATH) $(LINKLIB) -o $@ 
M-2714:M-2714.o util.o
	$(CXX) $^ $(CPPFLAGS) $(INCLUDE) $(LIBPATH) $(LINKLIB) -o $@ 
M-2713:M-2713.o util.o
	$(CXX) $^ $(CPPFLAGS) $(INCLUDE) $(LIBPATH) $(LINKLIB) -o $@ 
M-2707:M-2707.o util.o
	$(CXX) $^ $(CPPFLAGS) $(INCLUDE) $(LIBPATH) $(LINKLIB) -o $@ 
M-4703:M-4703.o util.o
	$(CXX) $^ $(CPPFLAGS) $(INCLUDE) $(LIBPATH) $(LINKLIB) -o $@ 
M-4710:M-4710.o util.o
	$(CXX) $^ $(CPPFLAGS) $(INCLUDE) $(LIBPATH) $(LINKLIB) -o $@ 
M-6893:M-6893.o util.o
	$(CXX) $^ $(CPPFLAGS) $(INCLUDE) $(LIBPATH) $(LINKLIB) -o $@ 
M-8676:M-8676.o util.o
	$(CXX) $^ $(CPPFLAGS) $(INCLUDE) $(LIBPATH) $(LINKLIB) -o $@ 
M-9638:M-9638.o util.o
	$(CXX) $^ $(CPPFLAGS) $(INCLUDE) $(LIBPATH) $(LINKLIB) -o $@ 
M-7691:M-7691.o util.o
	$(CXX) $^ $(CPPFLAGS) $(INCLUDE) $(LIBPATH) $(LINKLIB) -o $@ 
M-8380:M-8380.o util.o
	$(CXX) $^ $(CPPFLAGS) $(INCLUDE) $(LIBPATH) $(LINKLIB) -o $@ 
M-9089:M-9089.o util.o
	$(CXX) $^ $(CPPFLAGS) $(INCLUDE) $(LIBPATH) $(LINKLIB) -o $@ 
M-9094:M-9094.o util.o
	$(CXX) $^ $(CPPFLAGS) $(INCLUDE) $(LIBPATH) $(LINKLIB) -o $@ 
M-9933:M-9933.o util.o
	$(CXX) $^ $(CPPFLAGS) $(INCLUDE) $(LIBPATH) $(LINKLIB) -o $@ 
M-SQLColumns:M-SQLColumns.o util.o
	$(CXX) $^ $(CPPFLAGS) $(INCLUDE) $(LIBPATH) $(LINKLIB) -o $@ 
M-SQLPutData:M-SQLPutData.o util.o
	$(CXX) $^ $(CPPFLAGS) $(INCLUDE) $(LIBPATH) $(LINKLIB) -o $@ 
M-SQLSetConnectOption:M-SQLSetConnectOption.o util.o
	$(CXX) $^ $(CPPFLAGS) $(INCLUDE) $(LIBPATH) $(LINKLIB) -o $@ 
M-SQLSetStmtOption:M-SQLSetStmtOption.o util.o
	$(CXX) $^ $(CPPFLAGS) $(INCLUDE) $(LIBPATH) $(LINKLIB) -o $@ 
M-SQLGetDiagField:M-SQLGetDiagField.o util.o
	$(CXX) $^ $(CPPFLAGS) $(INCLUDE) $(LIBPATH) $(LINKLIB) -o $@ 
M-SQLBrowseConnect:M-SQLBrowseConnect.o util.o
	$(CXX) $^ $(CPPFLAGS) $(INCLUDE) $(LIBPATH) $(LINKLIB) -o $@ 
M-SQLGetDiagRec:M-SQLGetDiagRec.o util.o
	$(CXX) $^ $(CPPFLAGS) $(INCLUDE) $(LIBPATH) $(LINKLIB) -o $@ 
M-XXXX:M-xxxx.o util.o
	$(CXX) $^ $(CPPFLAGS) $(INCLUDE) $(LIBPATH) $(LINKLIB) -o $@ 
M-SQLGetDescFiled:M-SQLGetDescFiled.o util.o
	$(CXX) $^ $(CPPFLAGS) $(INCLUDE) $(LIBPATH) $(LINKLIB) -o $@
M-SQLDescribeCol:M-SQLDescribeCol.o util.o
	$(CXX) $^ $(CPPFLAGS) $(INCLUDE) $(LIBPATH) $(LINKLIB) -o $@
M-SQLGetStmtAttr:M-SQLGetStmtAttr.o util.o
	$(CXX) $^ $(CPPFLAGS) $(INCLUDE) $(LIBPATH) $(LINKLIB) -o $@
SQLTablePrivileges:SQLTablePrivileges.o util.o
	$(CXX) $^ $(CPPFLAGS) $(INCLUDE) $(LIBPATH) $(LINKLIB) -o $@
SQLGetFunctions:bugSQLGetFunctions.c
	$(CXX) $^ $(CPPFLAGS) $(INCLUDE) $(LIBPATH) $(LINKLIB) -o debug 
SQLGetTypeInfo:bugSQLGetTypeInfo.c
	$(CXX) $^ $(CPPFLAGS) $(INCLUDE) $(LIBPATH) $(LINKLIB) -o debug 
SQLTables:bugSQLTables.c
	$(CXX) $^ $(CPPFLAGS) $(INCLUDE) $(LIBPATH) $(LINKLIB) -o debug 
SQLPutData:bugSQLPutData.c
	$(CXX) $^ $(CPPFLAGS) $(INCLUDE) $(LIBPATH) $(LINKLIB) -o debug
SQLGetInfo:bugSQLGetInfo.c
	$(CXX) $^ $(CPPFLAGS) $(INCLUDE) $(LIBPATH) $(LINKLIB) -o debug
DataType:bugDataType.c
	$(CXX) $^ $(CPPFLAGS) $(INCLUDE) $(LIBPATH) $(LINKLIB) -o debug
SQLDescribeCol:bugSQLDescribeCol.c
	$(CXX) $^ $(CPPFLAGS) $(INCLUDE) $(LIBPATH) $(LINKLIB) -o debug
tenant:bugTenantName.c
	$(CXX) $^ $(CPPFLAGS) $(INCLUDE) $(LIBPATH) $(LINKLIB) -o debug
lob:bugLob.c
	$(CXX) $^ $(CPPFLAGS) $(INCLUDE) $(LIBPATH) $(LINKLIB) -o debug
guomi:bugGuoMi.c
	$(CXX) $^ $(CPPFLAGS) $(INCLUDE) $(LIBPATH) $(LINKLIB) -o debug
arm:bugARM.c
	$(CXX) $^ $(CPPFLAGS) $(INCLUDE) $(LIBPATH) $(LINKLIB) -o debug
SQLCopyDesc:bugSQLCopyDesc.c
	$(CXX) $^ $(CPPFLAGS) $(INCLUDE) $(LIBPATH) $(LINKLIB) -o debug
SQLColumns:bugSQLColumns.c
	$(CXX) $^ $(CPPFLAGS) $(INCLUDE) $(LIBPATH) $(LINKLIB) -o debug
rowset:bugRowset.c
	$(CXX) $^ $(CPPFLAGS) $(INCLUDE) $(LIBPATH) $(LINKLIB) -o debug
SQLParamData:bugSQLParamData.c
	$(CXX) $^ $(CPPFLAGS) $(INCLUDE) $(LIBPATH) $(LINKLIB) -o debug
M-CLEAN:
	rm -rf *.o
clean:
	rm -rf *.o *.d debug *.a *.o $(M-OBJS) 
