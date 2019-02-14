#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

/*******************************************************************************************************************/
int debugSQLGetStmtAttr_SQL_ATTR_APP_ROW_DESC(TestInfo *pTestInfo)
{
    int ret = 0;
    RETCODE retcode;
 	SQLHANDLE henv = SQL_NULL_HANDLE;
 	SQLHANDLE hdbc = SQL_NULL_HANDLE;
 	SQLHANDLE hstmt = SQL_NULL_HANDLE;
    TestInfo sTestInfo;
    SQLSMALLINT num, i;
    SQLHDESC    hdesci, hdesce;
    SQLINTEGER szStringLength[128] = {0};
    void *DescOut = NULL;
    
    memcpy(&sTestInfo, pTestInfo, sizeof(TestInfo));

    LogMsg(NONE, _T("..............................\n"));
    if (!FullConnectWithOptions(&sTestInfo, CONNECT_ODBC_VERSION_3))
    {
        LogMsg(NONE, _T("Unable to connect\n"));
        return -1;
    }
    henv = sTestInfo.henv;
    hdbc = sTestInfo.hdbc;
    
    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    if (!SQL_SUCCEEDED(retcode)){
        LogMsg(NONE, _T("SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt) fail,line = %d\n"), __LINE__);
        LogMsg(NONE,_T("Try SQLAllocStmt((SQLHANDLE)hdbc, &hstmt)\n"));
        retcode = SQLAllocStmt((SQLHANDLE)hdbc, &hstmt);
        if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
            LogMsg(NONE,_T("SQLAllocStmt hstmt fail.\n"));
            FullDisconnect(&sTestInfo);
            return -1;
        }
    }
    sTestInfo.hstmt = hstmt;
    LogMsg(NONE, _T("server info:\n"));
    addInfoSession(hstmt);
    LogMsg(NONE, _T("..............................\n"));
    
    LogMsg(NONE, _T("%s\n"), _T("SQLGetStmtAttr(hstmt, SQL_ATTR_APP_ROW_DESC, &hdesci, 0, szStringLength)"));
    retcode = SQLGetStmtAttr(hstmt,SQL_ATTR_APP_ROW_DESC, &hdesci, 0, szStringLength);
	if(retcode != SQL_SUCCESS){
		LogMsg(NONE, _T("%s\n"), _T("set stmt attr fail:SQL_ATTR_APP_ROW_DESC"));
		LogAllErrors(henv,hdbc,hstmt);
        ret = -1;
	}
    LogMsg(NONE, _T("%s\n"), _T("SQLAllocHandle(SQL_HANDLE_DESC, (SQLHANDLE)hdbc, &hdesce)"));
    retcode = SQLAllocHandle(SQL_HANDLE_DESC, (SQLHANDLE)hdbc, &hdesce);	
	if(retcode != SQL_SUCCESS){
		LogMsg(NONE, _T("%s\n"), _T("set stmt attr fail:SQL_ATTR_APP_ROW_DESC"));
		LogAllErrors(henv,hdbc,hstmt);
        ret = -1;
	}
    LogMsg(NONE, _T("%s\n"), _T("SQLSetStmtAttr(hstmt, SQL_ATTR_APP_ROW_DESC, (void *)hdesce, 0)"));
	retcode = SQLSetStmtAttr(hstmt, SQL_ATTR_APP_ROW_DESC,(void *)hdesce,0);
    if(retcode != SQL_SUCCESS){
		LogMsg(NONE, _T("%s\n"), _T("set stmt attr fail:SQL_ATTR_APP_ROW_DESC"));
		LogAllErrors(henv,hdbc,hstmt);
        ret = -1;
	}
    LogMsg(NONE, _T("%s\n"), _T("SQLGetStmtAttr(hstmt, SQL_ATTR_APP_ROW_DESC, &DescOut, 0, szStringLength)"));
    retcode = SQLGetStmtAttr(hstmt, SQL_ATTR_APP_ROW_DESC, &DescOut, 0, szStringLength);
	if(retcode != SQL_SUCCESS){
		LogMsg(NONE, _T("%s\n"), _T("get stmt attr fail:SQL_ATTR_APP_ROW_DESC"));
		LogAllErrors(henv,hdbc,hstmt);
        ret = -1;
	}
    LogMsg(NONE, _T("checkout: hdesce ?= DescOut, expect:%p actual:%p\n"), hdesce, DescOut);
    if( DescOut != (void *)hdesce){
        ret = -1;
        LogMsg(NONE, _T("\tnot match.\n"));
    }
    LogMsg(NONE, _T("%s\n"), _T("SQLSetStmtAttr(hstmt, SQL_ATTR_APP_ROW_DESC, (void *)SQL_NULL_HANDLE,0)"));
    retcode = SQLSetStmtAttr(hstmt, SQL_ATTR_APP_ROW_DESC,(void *)SQL_NULL_HANDLE,0);
	if(retcode != SQL_SUCCESS){
		LogMsg(NONE, _T("%s\n"), _T("set stmt attr fail:SQL_ATTR_APP_ROW_DESC"));
		LogAllErrors(henv,hdbc,hstmt);
        ret = -1;
	}
    LogMsg(NONE, _T("%s\n"), _T("SQLGetStmtAttr(hstmt, SQL_ATTR_APP_ROW_DESC, &DescOut, 0, szStringLength)"));
    DescOut = NULL;
	retcode = SQLGetStmtAttr(hstmt, SQL_ATTR_APP_ROW_DESC, &DescOut, 0, szStringLength);
	if(retcode != SQL_SUCCESS){
		LogMsg(NONE, _T("%s\n"), _T("get stmt attr fail:SQL_ATTR_APP_ROW_DESC"));
		LogAllErrors(henv,hdbc,hstmt);
        ret = -1;
	}
    LogMsg(NONE, _T("checkout: hdesci ?= DescOut, expect:%p actual:%p\n"), hdesci, DescOut);
    if( DescOut != (void *)hdesci){
        ret = -1;
        LogMsg(NONE, _T("\tnot match.\n"));
    }
TEST_FAIL:

    FullDisconnect(&sTestInfo);
    LogMsg(TIME_STAMP, _T("complete test:%s\n"), (ret == 0) ? _T("sucess") : _T("fail"));
    
    return ret;
}
/*******************************************************************************************************************/
void help(void)
{
    printf("\t-h:print this help\n");
    printf("Connection related options. You can connect using either:\n");
    printf("\t-d Data_Source_Name\n");
    printf("\t-u user\n");
    printf("\t-p password\n");
    printf("\t-s schema name\t");
    printf("sample:\t./debug -d TRAF_ANSI -u trafodion -p traf123 -s ODBCTEST\n");
}
//g++  xxx.c -o debug -I /usr/lib64 -L /usr/lib64 -ltrafodbc64 -lpthread -lz -licuuc -licudata -Dunixcli
//g++ xxx.c -o -Wall -Dunixcli -g -w  -I.  -lodbc -lpthread -DUNIXODBC -o debug
//./debug -d TRAF_ANSI -u trafodion -p traf123
int main(int argc, char *argv[])
{
    TestInfo oriInfo;
    char buf[256] = {0};
    int num;
 
    memset(&oriInfo, 0, sizeof(oriInfo));
   
    if(argc <= 1){
        help();
        return -1;
    }
    for(num = 1; num < argc; num++){
        if(_tcscmp(argv[num], _T("-h")) == 0){
            help();
            return 0;
        }
        else if((_tcscmp(argv[num], _T("-d")) == 0) && (argc > (num + 1))){
            num++;
            _tcscpy (oriInfo.DataSource, argv[num]);
        }
        else if((_tcscmp(argv[num], _T("-u")) == 0) && (argc > (num + 1))){
            num++;
            _tcscpy (oriInfo.UserID, argv[num]);
        }
        else if((_tcscmp(argv[num], _T("-p")) == 0) && (argc > (num + 1))){
            num++;
            _tcscpy (oriInfo.Password, argv[num]);
        }
        else if((_tcscmp(argv[num], _T("-s")) == 0) && (argc > (num + 1))){
            num++;
            _tcscpy (oriInfo.Schema, argv[num]);
        }
    }
    if(_tcslen(oriInfo.DataSource) == 0 ||
        _tcslen(oriInfo.UserID) == 0 ||
        _tcslen(oriInfo.Password) == 0
        ){
        help();
#ifndef unixcli
        printf("Please enter any key exit\n");
        getchar();
#endif
        return -1;
    }
    debugSQLGetStmtAttr_SQL_ATTR_APP_ROW_DESC(&oriInfo);
#ifndef unixcli
    printf("Please enter any key exit\n");
    getchar();
#endif
    return 0;
}

