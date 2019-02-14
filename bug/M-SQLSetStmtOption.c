#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

/*******************************************************************************************************************/
int M_SQLSetStmtOptions_SQL_MAX_ROWS(TestInfo *pTestInfo)
{
    SQLLEN len;
    RETCODE retcode;
    SQLHANDLE   henv = SQL_NULL_HANDLE;
    SQLHANDLE   hdbc = SQL_NULL_HANDLE;
    SQLHANDLE   hstmt = SQL_NULL_HANDLE;
    int ret = 0;
    TCHAR szDropDDL[] = _T("drop table tb");
    TCHAR szCreateDDL[] =_T("create table tb(c1 char(200))\n");
    TCHAR szInsert[] = _T("insert into tb values(12345678901234567890123456789012345678901234567890)");
    TCHAR szSelect[] = _T("select * from tb");  
    TCHAR szBuf[512] = {0};
    SQLUINTEGER pvParamInt;

   if(!FullConnectWithOptions(pTestInfo, CONNECT_ODBC_VERSION_3)){
        LogMsg(NONE, _T("connect fail: line %d\n"), __LINE__);
        return -1;
    }
    henv = pTestInfo->henv;
    hdbc = pTestInfo->hdbc;
    hstmt = (SQLHANDLE)pTestInfo->hstmt;
            

    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    if (!SQL_SUCCEEDED(retcode)){
        LogMsg(NONE, _T("SQLAllocHandle fail.\n"));
        LogMsg(NONE,_T("Try SQLAllocStmt\n"));
        retcode = SQLAllocStmt((SQLHANDLE)hdbc, &hstmt);
        if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
            LogMsg(NONE,_T("SQLAllocStmt hstmt fail.\n"));
            disconnect(henv, hdbc, hstmt);
            return -1;
        }
    }
#if 1
    retcode = SQLSetStmtOption(hstmt, SQL_MAX_ROWS, 10);
    if(retcode != SQL_SUCCESS){
        LogMsg(NONE, _T("call SQLSetConnectOption fail !\n"));
        ret = -1;
        LogAllErrors(henv,hdbc,hstmt);
        goto TEST_FAIL;
    }
    pvParamInt = 0;
    retcode = SQLGetStmtOption(hstmt,SQL_MAX_ROWS,&pvParamInt);
    if(retcode != SQL_SUCCESS){
        LogMsg(NONE, _T("call SQLGetStmtOption fail !\n"));
        ret = -1;
        LogAllErrors(henv,hdbc,hstmt);
        goto TEST_FAIL;
    }
    LogMsg(NONE, _T("SQL_MAX_ROWS:%d\n"), pvParamInt);
#endif

#if 0
    LogMsg(NONE, _T("%s\n"), szDropDDL);
    retcode = SQLExecDirect(hstmt, (SQLTCHAR *)szDropDDL, SQL_NTS);
    LogMsg(NONE, _T("%s\n"), szCreateDDL);
    retcode = SQLExecDirect(hstmt, (SQLTCHAR *)szCreateDDL, SQL_NTS);
    if(retcode != SQL_SUCCESS){
        LogMsg(NONE, _T("create table fail !\n"));
        ret = -1;
        LogAllErrors(henv,hdbc,hstmt);
        goto TEST_FAIL;
    }
#endif
#if 0
    LogMsg(NONE, _T("%s\n"), szInsert);
    retcode = SQLExecDirect(hstmt, (SQLTCHAR *)szInsert, SQL_NTS);
    if(retcode != SQL_SUCCESS){
        LogMsg(NONE, _T("insert fail !\n"));
        ret = -1;
        LogAllErrors(henv,hdbc,hstmt);
        goto TEST_FAIL;
    }
#endif
#if 0
    retcode = SQLBindCol(hstmt, 
                            1, 
                             SQL_C_TCHAR,
                             (SQLPOINTER)szBuf,
                             (SQLLEN) sizeof(szBuf), 
                             &len);
    retcode = SQLExecDirect(hstmt, (SQLTCHAR *)szSelect, SQL_NTS);
    if(retcode != SQL_SUCCESS){
        LogMsg(NONE, _T("select fail !\n"));
        ret = -1;
        LogAllErrors(henv,hdbc,hstmt);
        goto TEST_FAIL;
    }
    while ((retcode = SQLFetch(hstmt)) == SQL_SUCCESS) {
        LogMsg(NONE, _T("len = %d values:%s\n"), len, szBuf);
        memset(szBuf, 0, sizeof(szBuf));
    }
#endif
TEST_FAIL:
    disconnect(henv, hdbc, hstmt);
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
        _tcslen(oriInfo.Password) == 0/* ||
        _tcslen(oriInfo.Schema) == 0*/
        ){
        help();
#ifndef unixcli
        printf("Please enter any key exit\n");
        getchar();
#endif
        return -1;
    }
    M_SQLSetStmtOptions_SQL_MAX_ROWS(&oriInfo);
#ifndef unixcli
    printf("Please enter any key exit\n");
    getchar();
#endif
    return 0;
}


